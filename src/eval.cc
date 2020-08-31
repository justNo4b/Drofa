#include "defs.h"
#include "rays.h"
#include "movegen.h"
#include "eval.h"
#include "transptable.h"

extern HASH myHASH;

U64 Eval::detail::FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
U64 Eval::detail::NEIGHBOR_FILES[8]{
    FILE_B,
    FILE_A | FILE_C,
    FILE_B | FILE_D,
    FILE_C | FILE_E,
    FILE_D | FILE_F,
    FILE_E | FILE_G,
    FILE_F | FILE_H,
    FILE_G
};
U64 Eval::detail::PASSED_PAWN_MASKS[2][64];
U64 Eval::detail::PAWN_SHIELD_MASKS[2][64];
int Eval::detail::PHASE_WEIGHT_SUM = 0;
U64 Eval::detail::KING_OO_MASKS[2][2] = {
        [WHITE] = {
            [0] = 0xC0ull,
            [1] = 0x7ull
        },
        [BLACK] = {
            [0] = 0xC000000000000000ull,
            [1] = 0x700000000000000ull
        }
    };
U64 Eval::detail::KING_PAWN_MASKS[2][2][7] = {
        [WHITE] = {
          [0] = {
            [0] = 0xE000ull,
            [1] = 0x806000ull,
            [2] = 0x40A000ull,
            [3] = 0x80402000ull,
            [4] = 0xC02000ull,
            [5] = 0xC000ull,
            [6] = 0x804000ull
          },
          [1] = {
            [0] = 0x700ull,
            [1] = 0x10600ull,
            [2] = 0x20500ull,
            [3] = 0x1020400ull,
            [4] = 0x30400ull,
            [5] = 0,
            [6] = 0
          }
        },
        [BLACK] = {
          [0] = {
              [0] = 0xE0000000000000ull,
              [1] = 0x60800000000000ull,
              [2] = 0xA0400000000000ull,
              [3] = 0x20408000000000ull,
              [4] = 0x20C00000000000ull,
              [5] = 0xC0000000000000ull,
              [6] = 0x40800000000000ull
          },
          [1] = {
            [0] = 0x7000000000000ull,
            [1] = 0x6010000000000ull,
            [2] = 0x5020000000000ull,
            [3] = 0x4020100000000ull,
            [4] = 0x4030000000000ull,
            [5] = 0,
            [6] = 0
          }
        }
    };

void Eval::init() {
  // Initialize king pawn shield masks
  for (int i = 0; i < 64; i++) {
    U64 square = ONE << i;

    detail::PAWN_SHIELD_MASKS[WHITE][i] = ((square << 8) | ((square << 7) & ~FILE_H) |
        ((square << 9) & ~FILE_A)) & RANK_2;
    detail::PAWN_SHIELD_MASKS[BLACK][i] = ((square >> 8) | ((square >> 7) & ~FILE_A) |
        ((square >> 9) & ~FILE_H)) & RANK_7;
  }

  // Initialize passed pawn masks
  for (int square = 0; square < 64; square++) {
    U64 currNorthRay = Rays::getRay(Rays::NORTH, square);
    U64 currSouthRay = Rays::getRay(Rays::SOUTH, square);

    detail::PASSED_PAWN_MASKS[WHITE][square] =
        currNorthRay | _eastN(currNorthRay, 1) | _westN(currNorthRay, 1);
    detail::PASSED_PAWN_MASKS[BLACK][square] =
        currSouthRay | _westN(currSouthRay, 1) | _eastN(currSouthRay, 1);
  }

  // Initialize PHASE_WEIGHT_SUM
  detail::PHASE_WEIGHT_SUM += detail::PHASE_WEIGHTS[PAWN] * 16;
  detail::PHASE_WEIGHT_SUM += detail::PHASE_WEIGHTS[KNIGHT] * 4;
  detail::PHASE_WEIGHT_SUM += detail::PHASE_WEIGHTS[BISHOP] * 4;
  detail::PHASE_WEIGHT_SUM += detail::PHASE_WEIGHTS[ROOK] * 4;
  detail::PHASE_WEIGHT_SUM += detail::PHASE_WEIGHTS[QUEEN] * 2;
}

int Eval::getMaterialValue(PieceType pieceType) {
  return MATERIAL_VALUES[OPENING][pieceType];
}

bool Eval::hasBishopPair(const Board &board, Color color) {
  return ((board.getPieces(color, BISHOP) & BLACK_SQUARES) != ZERO)
      && ((board.getPieces(color, BISHOP) & WHITE_SQUARES) != ZERO);
}

int Eval::evaluateMobility(const Board &board, GamePhase phase, Color color) {
  int score = 0;

  // Special case for pawn moves
  U64 pawns = board.getPieces(color, PAWN);
  U64 singlePawnPushes, doublePawnPushes, pawnAttacks;
  if (color == WHITE) {
    singlePawnPushes = (pawns << 8) & board.getNotOccupied();
    doublePawnPushes = ((singlePawnPushes & RANK_3) << 8) & board.getNotOccupied();
    pawnAttacks = ((pawns << 7) & ~FILE_H) | ((pawns << 9) & ~FILE_A);
  } else {
    singlePawnPushes = (pawns >> 8) & board.getNotOccupied();
    doublePawnPushes = ((singlePawnPushes & RANK_6) >> 8) & board.getNotOccupied();
    pawnAttacks = ((pawns >> 7) & ~FILE_A) | ((pawns >> 9) & ~FILE_H);
  }
  pawnAttacks &= board.getAttackable(getOppositeColor(color));
  score += _popCount(singlePawnPushes | doublePawnPushes | pawnAttacks) * MOBILITY_BONUS[phase][PAWN];

  // All other pieces
  for (auto pieceType : {ROOK, KNIGHT, BISHOP, QUEEN, KING}) {
    U64 pieces = board.getPieces(color, pieceType);

    while (pieces) {
      int square = _popLsb(pieces);
      U64 attackBitBoard = board.getAttacksForSquare(pieceType, color, square);
      score += _popCount(attackBitBoard) * MOBILITY_BONUS[phase][pieceType];
    }
  }

  return score;
}

int Eval::rooksOnOpenFiles(const Board &board, Color color) {
  Color otherColor = getOppositeColor(color);
  int numRooks = 0;

  for (U64 file : detail::FILES) {
    if ((file & board.getPieces(color, ROOK))
        && ((file & board.getPieces(color, PAWN)) == 0)
        && ((file & board.getPieces(otherColor, PAWN)) == 0)
        ) {
      numRooks += _popCount(file & board.getPieces(color, ROOK));
    }
  }
  return numRooks;
}

int Eval::rooksOnSemiFiles(const Board &board, Color color) {
  int numRooks = 0;

  for (U64 file : detail::FILES) {
    if ((file & board.getPieces(color, ROOK))
        && ((file & board.getPieces(color, PAWN)) == 0)
        ) {
      numRooks += _popCount(file & board.getPieces(color, ROOK));
    }
  }
  return numRooks;
}

int Eval::passedPawns(const Board &board, Color color) {
  int passed = 0;
  U64 pawns = board.getPieces(color, PAWN);

  while (pawns != ZERO) {
    int square = _popLsb(pawns);
    if ((board.getPieces(getOppositeColor(color), PAWN) & detail::PASSED_PAWN_MASKS[color][square]) == ZERO) passed++;
    pawns &= ~detail::FILES[square % 8];
  }

  return passed;
}

int Eval::doubledPawns(const Board &board, Color color) {
  int doubled = 0;

  for (U64 file : detail::FILES) {
    int pawnsOnFile = _popCount(file & board.getPieces(color, PAWN));

    if (pawnsOnFile > 1) doubled += (pawnsOnFile - 1);
  }

  return doubled;
}

int Eval::isolatedPawns(const Board &board, Color color) {
  int isolated = 0;

  U64 pawns = board.getPieces(color, PAWN);

  for (int fileNumber = 0; fileNumber < 8; fileNumber++) {
    U64 fileMask = detail::FILES[fileNumber];
    U64 neighborFileMask = detail::NEIGHBOR_FILES[fileNumber];

    if ((fileMask & pawns) && !(neighborFileMask & pawns)) isolated++;
  }

  return isolated;
}

int Eval::pawnsShieldingKing(const Board &board, Color color) {
  int kingSquare = _bitscanForward(board.getPieces(color, KING));
  return _popCount(detail::PAWN_SHIELD_MASKS[color][kingSquare] & board.getPieces(color, PAWN));
}

bool Eval::IsItDeadDraw (int w_P, int w_N, int w_B, int w_R, int w_Q,
int b_P, int b_N, int b_B, int b_R, int b_Q){

if (w_P > 0 || b_P > 0){
  return false;           // если есть хоть 1 пешка, то ещё играем
}

if (w_Q > 0 || b_Q > 0 ){
  return false;           // есть есть хоть 1 королева, то ещё играем
} 

if (w_R == 0 && b_R == 0){ // нет пешек, нет королев, нет ладей
  if (w_B == 0 && b_B == 0){  // нет пешек, ладей, слонов.
      if (w_N < 3 && b_N < 3){ // меньше 2х коней = ничья
        return true;
      }
  } else if (w_N == 0 && b_N == 0){ // нет пешек, королев, ладей, коней
      if (abs( w_B - b_B) < 2){
        return true;
      }
  }else if (( w_N < 3 && w_B ==0) || (w_B == 1 &&  w_N == 0)){
    if ((b_N < 3 && b_B == 0)||(b_B == 1 && b_N == 0)){
      return true;
    }
  }
}else {                             // ладьи есть
  if (w_R == 1 && b_R == 1){
    if (w_N + w_B < 2 && b_N + b_B < 2){    // тут немного криво, так как BR vs R выигрывают
      return true;
    } 
  }else if (w_R == 1 && b_R == 0){
    if (w_N + w_B == 0 && b_N + b_B > 1){
      return true;
    }
  }else if (b_R == 1 && w_R == 0){
    if (b_N + b_B == 0 && w_N + w_B > 1){
      return true;
    }
  }
}

return false;

}

int Eval::kingSafety(const Board &board, Color color, int Q_count){
      //King safety - замена pawnsShieldingKing
    // идея в том, чтобы
    // а. Найти позицию короля
    // b. Для каждой выбранной позиции мы имеем некую маску и скор того, насколько она "безопасна"

    // Упрощённо будем считать, что если нет ферзя у врага, то мы в безопасности.
    if (Q_count == 0){
      return 0;
    }
    U64 pawnMap = board.getPieces(color, PAWN);
    //проверяем где наш король. Проверка по сути на то, куда сделали рокировку.
    //если король не рокирован-выгнан с рокировки, то король не в безопасности, начислить штраф

         //0 - kingSide; 1 - QueenSide

    if ((detail::KING_OO_MASKS[color][0] & board.getPieces(color, KING)) != 0){
      // если это верно, то мы находимся на королевском фланге
      // сравниваем с масками, возвращаем бонус если маска совпала
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][0]) == detail::KING_PAWN_MASKS[color][0][0]){
          return KING_SAFE;    
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][1]) == detail::KING_PAWN_MASKS[color][0][1]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][2]) == detail::KING_PAWN_MASKS[color][0][2]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][3]) == detail::KING_PAWN_MASKS[color][0][3]){
          return KING_LOW_DANGER;     
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][4]) == detail::KING_PAWN_MASKS[color][0][4]){
          return KING_LOW_DANGER;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][5]) == detail::KING_PAWN_MASKS[color][0][5]){
          return KING_LOW_DANGER;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][6]) == detail::KING_PAWN_MASKS[color][0][6]){
          return KING_LOW_DANGER;
      }
      // если не одна из масок не прошла, то король в опасности.
      // вернуть штраф к нашей позиции
      return KING_MED_DANGER;
    }      

    if ((detail::KING_OO_MASKS[color][1] & board.getPieces(color, KING)) != 0){
      // если это верно, то мы находимся на ферзевом
      // сравниваем с масками, возвращаем бонус если маска совпала
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][0]) == detail::KING_PAWN_MASKS[color][1][0]){
          return KING_SAFE;    
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][1]) == detail::KING_PAWN_MASKS[color][1][1]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][2]) == detail::KING_PAWN_MASKS[color][1][2]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][3]) == detail::KING_PAWN_MASKS[color][1][3]){
          return KING_LOW_DANGER;     
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][4]) == detail::KING_PAWN_MASKS[color][1][4]){
          return KING_LOW_DANGER;
      }
      // если не одна из масок не прошла, то король в опасности.
      // вернуть штраф к нашей позиции
      return KING_MED_DANGER;
    } 

      // мы не рокированы по факту при живом ферзе.
      // это очень опасно, вернуть штраф
    return KING_HIGH_DANGER;
}

int Eval::evaluatePawnStructure(const Board &board, Color color, GamePhase phase) {

  pawn_HASH_Entry pENTRY  = myHASH.pHASH_Get(board.getPawnStructureZKey().getValue());
  if (pENTRY.posKey != 0){
    int whiteScore;
    if (phase == OPENING){
      whiteScore = pENTRY.score_OPENING;       
    }else{
      whiteScore = pENTRY.score_ENDGAME;    
    }

    if (color == BLACK) {
      return -whiteScore;
    } else {
      return whiteScore;
    }
 
  }

  int scores[2] = {0};

  // Passed pawns
  int passedPawnDiff = passedPawns(board, WHITE) - passedPawns(board, BLACK);
  scores[OPENING] += PASSED_PAWN_BONUS[OPENING] * passedPawnDiff;
  scores[ENDGAME] += PASSED_PAWN_BONUS[ENDGAME] * passedPawnDiff;

  // Doubled pawns
  int doubledPawnDiff = doubledPawns(board, WHITE) - doubledPawns(board, BLACK);
  scores[OPENING] += DOUBLED_PAWN_PENALTY[OPENING] * doubledPawnDiff;
  scores[ENDGAME] += DOUBLED_PAWN_PENALTY[ENDGAME] * doubledPawnDiff;

  // Isolated pawns
  int isolatedPawnDiff = isolatedPawns(board, WHITE) - isolatedPawns(board, BLACK);
  scores[OPENING] += ISOLATED_PAWN_PENALTY[OPENING] * isolatedPawnDiff;
  scores[ENDGAME] += ISOLATED_PAWN_PENALTY[ENDGAME] * isolatedPawnDiff;

  myHASH.pHASH_Store(board.getPawnStructureZKey().getValue(), scores[ENDGAME], scores[OPENING]);

  if (color == BLACK) {
    return -scores[phase];
  } else {
    return scores[phase];
  }
}

template<GamePhase phase>

int Eval::evaluateForPhase(const Board &board, Color color) {
  int score = 0;

  Color otherColor = getOppositeColor(color);

  // Material value

  int w_P = _popCount(board.getPieces(color, PAWN));
  int b_P = _popCount(board.getPieces(otherColor, PAWN));
  int w_N = _popCount(board.getPieces(color, KNIGHT));
  int b_N = _popCount(board.getPieces(otherColor, KNIGHT));
  int w_B = _popCount(board.getPieces(color, BISHOP));
  int b_B = _popCount(board.getPieces(otherColor, BISHOP));
  int w_R = _popCount(board.getPieces(color, ROOK));
  int b_R = _popCount(board.getPieces(otherColor, ROOK));
  int w_Q = _popCount(board.getPieces(color, QUEEN));
  int b_Q = _popCount(board.getPieces(otherColor, QUEEN));

  if (IsItDeadDraw(w_P, w_N, w_B, w_R, w_Q, b_P, b_N, b_B, b_R, b_Q)){
    return 0;
  }

  score += MATERIAL_VALUES[phase][PAWN] * ( w_P - b_P);
  score += MATERIAL_VALUES[phase][KNIGHT] * (w_N - b_N);
  score += MATERIAL_VALUES[phase][BISHOP] * ( w_B - b_B);
  score += MATERIAL_VALUES[phase][ROOK] * ( w_R - b_R);
  score += MATERIAL_VALUES[phase][QUEEN] * ( w_Q - b_Q);

  // Piece square tables
  score += board.getPSquareTable().getScore(phase, color) - board.getPSquareTable().getScore(phase, otherColor);

  // Mobility
  score += evaluateMobility(board, phase, color) - evaluateMobility(board, phase, otherColor);

  // Rook on open file
  score += ROOK_OPEN_FILE_BONUS[phase] * (rooksOnOpenFiles(board, color) - rooksOnOpenFiles(board, otherColor));

  //Rook on semi-open-file
  score += ROOK_SEMI_FILE_BONUS[phase] * (rooksOnSemiFiles(board, color) - rooksOnSemiFiles(board, otherColor));

  // Bishop pair
  score += hasBishopPair(board, color) ? BISHOP_PAIR_BONUS[phase] : 0;
  score -= hasBishopPair(board, otherColor) ? BISHOP_PAIR_BONUS[phase] : 0;

  // Pawn structure
  score += evaluatePawnStructure(board, color, phase);

  // King pawn shield
  // Tapering is included in, so we count it in both phases
  // As of 5.08.20 400 game testing did not showed advantage for any king safety implementation.
  // Changes commitet for further use though
  score += kingSafety(board, color, b_Q) - kingSafety(board, otherColor, w_Q);


  return score;
}

int Eval::getPhase(const Board &board) {
  int phase = detail::PHASE_WEIGHT_SUM;

  for (auto pieceType : {ROOK, KNIGHT, BISHOP, QUEEN}) {
    phase -= _popCount(board.getPieces(WHITE, pieceType)) * detail::PHASE_WEIGHTS[pieceType];
    phase -= _popCount(board.getPieces(BLACK, pieceType)) * detail::PHASE_WEIGHTS[pieceType];
  }

  // Transform phase from the range 0 - PHASE_WEIGHT_SUM to 0 - PHASE_WEIGHT_MAX
  return ((phase * MAX_PHASE) + (detail::PHASE_WEIGHT_SUM / 2)) / detail::PHASE_WEIGHT_SUM;
}

int Eval::evaluate(const Board &board, Color color) {
  int openingScore = evaluateForPhase<OPENING>(board, color);
  int endgameScore = evaluateForPhase<ENDGAME>(board, color);
  int phase = getPhase(board);

  // Interpolate between opening/endgame scores depending on the phase
  return ((openingScore * (MAX_PHASE - phase)) + (endgameScore * phase)) / MAX_PHASE;
}

int Eval::evalTestSuite(const Board &board, Color color)
{
    //King safety - замена pawnsShieldingKing
    // идея в том, чтобы
    // а. Найти позицию короля
    // b. Для каждой выбранной позиции мы имеем некую маску и скор того, насколько она "безопасна"

    // Упрощённо будем считать, что если нет ферзя у врага, то мы в безопасности.
    if (_popCount(board.getPieces(getOppositeColor(color), QUEEN)) == 0){
      return 0;
    }
    U64 pawnMap = board.getPieces(color, PAWN);
    //проверяем где наш король. Проверка по сути на то, куда сделали рокировку.
    //если король не рокирован-выгнан с рокировки, то король не в безопасности, начислить штраф

         //0 - kingSide; 1 - QueenSide

    if ((detail::KING_OO_MASKS[color][0] & board.getPieces(color, KING)) != 0){
      // если это верно, то мы находимся на королевском фланге
      // сравниваем с масками, возвращаем бонус если маска совпала
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][0]) == detail::KING_PAWN_MASKS[color][0][0]){
          return KING_SAFE;    
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][1]) == detail::KING_PAWN_MASKS[color][0][1]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][2]) == detail::KING_PAWN_MASKS[color][0][2]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][3]) == detail::KING_PAWN_MASKS[color][0][3]){
          return KING_LOW_DANGER;     
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][4]) == detail::KING_PAWN_MASKS[color][0][4]){
          return KING_LOW_DANGER;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][5]) == detail::KING_PAWN_MASKS[color][0][5]){
          return KING_LOW_DANGER;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][0][6]) == detail::KING_PAWN_MASKS[color][0][6]){
          return KING_LOW_DANGER;
      }
      // если не одна из масок не прошла, то король в опасности.
      // вернуть штраф к нашей позиции
      return KING_HIGH_DANGER;
    }      

    if ((detail::KING_OO_MASKS[color][1] & board.getPieces(color, KING)) != 0){
      // если это верно, то мы находимся на ферзевом
      // сравниваем с масками, возвращаем бонус если маска совпала
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][0]) == detail::KING_PAWN_MASKS[color][1][0]){
          return KING_SAFE;    
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][1]) == detail::KING_PAWN_MASKS[color][1][1]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][2]) == detail::KING_PAWN_MASKS[color][1][2]){
          return KING_SAFE;
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][3]) == detail::KING_PAWN_MASKS[color][1][3]){
          return KING_LOW_DANGER;     
      }
      if ((pawnMap & detail::KING_PAWN_MASKS[color][1][4]) == detail::KING_PAWN_MASKS[color][1][4]){
          return KING_LOW_DANGER;
      }
      // если не одна из масок не прошла, то король в опасности.
      // вернуть штраф к нашей позиции
      return KING_HIGH_DANGER;
    } 

      // мы не рокированы по факту при живом ферзе.
      // это очень опасно, вернуть штраф
    return KING_HIGH_DANGER;
}