#include "defs.h"
#include "rays.h"
#include "movegen.h"
#include "attacks.h"
#include "outposts.h"
#include "eval.h"
#include "transptable.h"
#include "tuning.h"

extern HASH * myHASH;
extern posFeatured ft;

int MATERIAL_VALUES_TUNABLE[2][6] = {
    [OPENING] = {
        [PAWN] = 100,
        [ROOK] = 465,
        [KNIGHT] = 304,
        [BISHOP] = 336,
        [QUEEN] = 1190,
        [KING] = 0
    },
    [ENDGAME] = {
        [PAWN] = 86,
        [ROOK] = 565,
        [KNIGHT] = 328,
        [BISHOP] = 357,
        [QUEEN] = 995,
        [KING] = 0
    }
};

U64 Eval::detail::FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
U64 Eval::detail::DISTANCE[64][64];
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
U64 Eval::detail::OUTPOST_MASK[2][64];
U64 Eval::detail::CONNECTED_MASK[64];
U64 Eval::detail::OUTPOST_PROTECTION[2][64];
U64 Eval::detail::KINGZONE[2][64];
U64 Eval::detail::FORWARD_BITS[2][64];
int Eval::detail::PHASE_WEIGHT_SUM = 0;
U64 Eval::detail::KING_PAWN_MASKS[2][2][8] = {
        [WHITE] = {
          [0] = {
            (ONE << f2) | (ONE << g2) | (ONE << h2),
            (ONE << f2) | (ONE << g2) | (ONE << h3),
            (ONE << f2) | (ONE << g3) | (ONE << h2),
            (ONE << f2) | (ONE << g3) | (ONE << h4),
            (ONE << f2) | (ONE << g3) | (ONE << h3),
            (ONE << g2) | (ONE << h2),
            (ONE << g2) | (ONE << h3),
            (ONE << g2) | (ONE << g3) | (ONE << f2),
          },
          [1] = {
            (ONE << a2) | (ONE << b2) | (ONE << c2),
            (ONE << a3) | (ONE << b2) | (ONE << c2),
            (ONE << a2) | (ONE << b3) | (ONE << c2),
            (ONE << a4) | (ONE << b3) | (ONE << c2),
            (ONE << a3) | (ONE << b3) | (ONE << c2),
            (ONE << a2) | (ONE << b2),
            (ONE << a3) | (ONE << b2),
            (ONE << b2) | (ONE << b3) | (ONE << c2)
          }
        },
        [BLACK] = {
          [0] = {
            (ONE << f7) | (ONE << g7) | (ONE << h7),
            (ONE << f7) | (ONE << g7) | (ONE << h6),
            (ONE << f7) | (ONE << g6) | (ONE << h7),
            (ONE << f7) | (ONE << g6) | (ONE << h5),
            (ONE << f7) | (ONE << g6) | (ONE << h6),
            (ONE << g7) | (ONE << h7),
            (ONE << g7) | (ONE << h6),
            (ONE << g7) | (ONE << g6) | (ONE << f7),
          },
          [1] = {
            (ONE << a7) | (ONE << b7) | (ONE << c7),
            (ONE << a6) | (ONE << b7) | (ONE << c7),
            (ONE << a7) | (ONE << b6) | (ONE << c7),
            (ONE << a5) | (ONE << b6) | (ONE << c7),
            (ONE << a6) | (ONE << b6) | (ONE << c7),
            (ONE << a7) | (ONE << b7),
            (ONE << a6) | (ONE << b7),
            (ONE << b7) | (ONE << b6) | (ONE << c7)
          }
        }
    };


void Eval::init() {
  // Initialize passed pawn masks
  for (int square = 0; square < 64; square++) {

    detail::CONNECTED_MASK[square] = ((ONE << (square + 1)) & ~FILE_A) | ((ONE << (square - 1)) & ~FILE_H);

    for (int i =0; i < 64; i++){
      detail::DISTANCE[square][i] = std::max(abs(_col(square) - _col(i)), abs(_row(square) - _row(i)));
    }

    for (auto color : { WHITE, BLACK }) {

      U64 forwardRay = Rays::getRay(color == WHITE ? Rays::NORTH : Rays::SOUTH, square);

      detail::FORWARD_BITS[color][square] = forwardRay;

      detail::PASSED_PAWN_MASKS[color][square] = forwardRay | _eastN(forwardRay, 1) | _westN(forwardRay, 1);

      detail::OUTPOST_MASK[color][square] = detail::PASSED_PAWN_MASKS[color][square] & detail::NEIGHBOR_FILES[_col(square)];

      U64 sqv = ONE << square;
      detail::OUTPOST_PROTECTION[color][square] = color == WHITE ? ((sqv >> 9) & ~FILE_H) | ((sqv >> 7) & ~FILE_A)
                                                                 : ((sqv << 9) & ~FILE_A) | ((sqv << 7) & ~FILE_H);

      U64 kingAttack = Attacks::getNonSlidingAttacks(KING, square, WHITE);
      detail::KINGZONE[color][square] = color == WHITE ? sqv | kingAttack | (kingAttack << 8)
                                                       : sqv | kingAttack | (kingAttack >> 8);
    }
  }

  // Initialize PHASE_WEIGHT_SUM
  detail::PHASE_WEIGHT_SUM += 16 * detail::PHASE_WEIGHTS[PAWN];
  detail::PHASE_WEIGHT_SUM +=  4 * detail::PHASE_WEIGHTS[KNIGHT];
  detail::PHASE_WEIGHT_SUM +=  4 * detail::PHASE_WEIGHTS[BISHOP];
  detail::PHASE_WEIGHT_SUM +=  4 * detail::PHASE_WEIGHTS[ROOK];
  detail::PHASE_WEIGHT_SUM +=  2 * detail::PHASE_WEIGHTS[QUEEN];
}

evalBits Eval::Setupbits(const Board &board){
  evalBits eB;

  for (auto color : { WHITE, BLACK }) {
    U64 pBB = board.getPieces(color, PAWN);
    eB.EnemyPawnAttackMap[!color] = color == WHITE ? ((pBB << 9) & ~FILE_A) | ((pBB << 7) & ~FILE_H)
                                                   : ((pBB >> 9) & ~FILE_H) | ((pBB >> 7) & ~FILE_A);

    U64 king = board.getPieces(color, KING);
    eB.EnemyKingZone[!color] = detail::KINGZONE[color][_bitscanForward(king)];
  }

  eB.RammedCount = _popCount((board.getPieces(BLACK, PAWN) >> 8) & board.getPieces(WHITE, PAWN));
  eB.OutPostedLines[0] = 0, eB.OutPostedLines[1] = 0;
  eB.KingAttackers[0] = 0, eB.KingAttackers[1] = 0;
  eB.KingAttackPower[0] = 0, eB.KingAttackPower[1] = 0;
  eB.Passers[0] = 0, eB.Passers[1] = 0;
  eB.AttackedSquares[0] = 0, eB.AttackedSquares[1] = 0;
  return eB;
}

void Eval::SetupTuning(int phase, PieceType piece, int value){
  MATERIAL_VALUES_TUNABLE [phase][piece] = value;
}

int Eval::getMaterialValue(int phase, PieceType pieceType) {
  return phase == OPENING ? opS(MATERIAL_VALUES[pieceType])
                          : egS(MATERIAL_VALUES[pieceType]);
}

inline bool Eval::IsItDeadDraw (int w_N, int w_B, int w_R, int w_Q,
                        int b_N, int b_B, int b_R, int b_Q){

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
    } else if (( w_N < 3 && w_B ==0) || (w_B == 1 &&  w_N == 0)){
      if ((b_N < 3 && b_B == 0)||(b_B == 1 && b_N == 0)){
        return true;
      }
    }
  } else {                             // ладьи есть
    if (w_R == 1 && b_R == 1){
      if (w_N + w_B < 2 && b_N + b_B < 2){    // тут немного криво, так как BR vs R выигрывают
        return true;
      }
    } else if (w_R == 1 && b_R == 0){
      if (w_N + w_B == 0 && b_N + b_B > 1){
        return true;
      }
    } else if (b_R == 1 && w_R == 0){
      if (b_N + b_B == 0 && w_N + w_B > 1){
        return true;
      }
    }
  }

  return false;
}

inline int Eval::kingShieldSafety(const Board &board, Color color, int Q_count, evalBits * eB){
      //King safety - замена pawnsShieldingKing
    // идея в том, чтобы
    // а. Найти позицию короля
    // b. Для каждой выбранной позиции мы имеем некую маску и скор того, насколько она "безопасна"

    // Упрощённо будем считать, что если нет ферзя у врага, то мы в безопасности.
    if (Q_count == 0){
      if (TRACK) ft.KingLowDanger[color]++;
      return KING_LOW_DANGER;
    }
    U64 pawnMap = board.getPieces(color, PAWN);

    //проверяем где наш король. Проверка по сути на то, куда сделали рокировку.
    //если король не рокирован-выгнан с рокировки, то король не в безопасности, начислить штраф
    //0 - kingSide; 1 - QueenSide
    CastleSide cSide = NoCastle;
    if (KSIDE_CASTLE[color] & board.getPieces(color, KING)){
      cSide = KingSide;
    } else if (QSIDE_CASTLE[color] & board.getPieces(color, KING)){
      cSide = QueenSide;
    }

    if (cSide == NoCastle){
      if (TRACK) ft.KingHighDanger[color]++;
      return KING_HIGH_DANGER;
    }
    // Cycle through all masks, if one of them is true,
    // Apply bonus for safety and score
    for (int i = 0; i < 8; i++){
      if ((pawnMap & detail::KING_PAWN_MASKS[color][cSide][i]) == detail::KING_PAWN_MASKS[color][cSide][i]){
                eB->KingAttackPower[getOppositeColor(color)] += SAFE_SHIELD_SAFETY[cSide][i];
                if (TRACK){
                  if (cSide == KingSide)  ft.KingShieldKS[i][color]++;
                  if (cSide == QueenSide) ft.KingShieldQS[i][color]++;
                }
                return cSide == KingSide ? KING_PAWN_SHIELD_KS[i] : KING_PAWN_SHIELD_QS[i];
            }
    }

      // если не одна из масок не прошла, то король в опасности.
      // вернуть штраф к нашей позиции
      if (TRACK) ft.KingMedDanger[color]++;
      return KING_MED_DANGER;

}

int Eval::getPhase(const Board &board) {
  int phase = detail::PHASE_WEIGHT_SUM;

  for (auto pieceType : {ROOK, KNIGHT, BISHOP, QUEEN}) {
    phase -= _popCount(board.getPieces(WHITE, pieceType)) * detail::PHASE_WEIGHTS[pieceType];
    phase -= _popCount(board.getPieces(BLACK, pieceType)) * detail::PHASE_WEIGHTS[pieceType];
  }

  // Make sure phase is not negative
  phase = std::max(0, phase);

  // Transform phase from the range 0 - PHASE_WEIGHT_SUM to 0 - PHASE_WEIGHT_MAX
  return ((phase * MAX_PHASE) + (detail::PHASE_WEIGHT_SUM / 2)) / detail::PHASE_WEIGHT_SUM;
}

inline int Eval::evaluateQUEEN(const Board & board, Color color, evalBits * eB){
  int s = 0;

  U64 pieces = board.getPieces(color, QUEEN);
  Color otherColor = getOppositeColor(color);

  // Apply penalty for each Queen attacked by enemy pawn
  s += HANGING_PIECE[QUEEN] * (_popCount(pieces & eB->EnemyPawnAttackMap[color]));
  if (TRACK) ft.HangingPiece[QUEEN][color] += (_popCount(pieces & eB->EnemyPawnAttackMap[color]));

  while (pieces) {
    int square = _popLsb(pieces);

    if (TRACK){
      int relSqv = color == WHITE ? _mir(square) : square;
      ft.QueenPsqtBlack[relSqv][color]++;
    }

    // Mobility calculations
    // Note: Queen NOT using scanning mobility
    U64 attackBitBoard = board.getMobilityForSquare(QUEEN, color, square, eB->EnemyPawnAttackMap[color]);
    s += QUEEN_MOBILITY[_popCount(attackBitBoard)];
    if (TRACK) ft.QueenMobility[_popCount(attackBitBoard)][color]++;

    // Save our attacks for further use
    eB->AttackedSquares[color] |= attackBitBoard;

    // QueenAttackMinor
    U64 QueenAttackMinor = (board.getPieces(otherColor, KNIGHT) | board.getPieces(otherColor, BISHOP)) & attackBitBoard;
    s += MINOR_ATTACKED_BY[QUEEN] * _popCount(QueenAttackMinor);
    if (TRACK) ft.MinorAttackedBy[QUEEN][color] += _popCount(QueenAttackMinor);

    //QueenAttackRook
    s += ROOK_ATTACKED_BY[QUEEN] * _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));
    if (TRACK) ft.RookAttackedBy[QUEEN][color] += _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));

    // See if a Queen is attacking an enemy unprotected pawn
    s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
    if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

    // If Queen attacking squares near enemy king
    // Adjust our kind Danger code
    int kingAttack = _popCount(attackBitBoard & eB->EnemyKingZone[color]);
    if (kingAttack > 0){
      eB->KingAttackers[color]++;
      eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[QUEEN];
    }
  }

  return s;
}

inline int Eval::evaluateROOK(const Board & board, Color color, evalBits * eB){
  int s = 0;
  Color otherColor = getOppositeColor(color);
  U64 pieces = board.getPieces(color, ROOK);
  U64 mobZoneAdjusted  = eB->EnemyPawnAttackMap[color] & ~board.getPieces(otherColor, QUEEN);

  // Apply penalty for each Rook attacked by enemy pawn
  s += HANGING_PIECE[ROOK] * (_popCount(pieces & eB->EnemyPawnAttackMap[color]));
  if (TRACK) ft.HangingPiece[ROOK][color] += (_popCount(pieces & eB->EnemyPawnAttackMap[color]));

  while (pieces) {
    int square = _popLsb(pieces);
    if (TRACK){
      int relSqv = color == WHITE ? _mir(square) : square;
      ft.RookPsqtBlack[relSqv][color]++;
    }

    // Mobility
    // Rooks are scanning through rooks and queens
    U64 attackBitBoard = board.getMobilityForSquare(ROOK, color, square, mobZoneAdjusted);
    s += ROOK_MOBILITY[_popCount(attackBitBoard)];
    if (TRACK) ft.RookMobility[_popCount(attackBitBoard)][color]++;

    // Save our attacks for further use
    eB->AttackedSquares[color] |= attackBitBoard;

    // RookAttackMinor
    U64 RookAttackMinor = (board.getPieces(otherColor, KNIGHT) | board.getPieces(otherColor, BISHOP)) & attackBitBoard;
    s += MINOR_ATTACKED_BY[ROOK] * _popCount(RookAttackMinor);
    if (TRACK) ft.MinorAttackedBy[ROOK][color] += _popCount(RookAttackMinor);

    //RookAttackRook
    s += ROOK_ATTACKED_BY[ROOK] * _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));
    if (TRACK) ft.RookAttackedBy[ROOK][color] += _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));

    // If Rook attacking squares near enemy king
    // Adjust our kind Danger code
    int kingAttack = _popCount(attackBitBoard & eB->EnemyKingZone[color]);
    if (kingAttack > 0){
      eB->KingAttackers[color]++;
      eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[ROOK];
    }

    // See if a Rook is attacking an enemy unprotected pawn
    s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
    if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

    // Open/semiopen file detection
    // we differentiate between open/semiopen based on
    // if there are enemys protected outpost here
    U64 file = detail::FILES[_col(square)];
    if ( ((file & board.getPieces(color, PAWN)) == 0)
      && ((file & board.getPieces(otherColor, PAWN)) == 0)){
      s += ROOK_OPEN_FILE_BONUS[((file & eB->OutPostedLines[otherColor]) != 0)];
      if (TRACK) ft.RookOpenFile[((file & eB->OutPostedLines[otherColor]) != 0)][color]++;
    }
    else if ((file & board.getPieces(color, PAWN)) == 0){
      s += ROOK_SEMI_FILE_BONUS[((file & eB->OutPostedLines[otherColor]) != 0)];
      if (TRACK) ft.RookHalfFile[((file & eB->OutPostedLines[otherColor]) != 0)][color]++;
    }
  }

  return s;
}

inline int Eval::evaluateBISHOP(const Board & board, Color color, evalBits * eB){
  int s = 0;

  U64 pieces = board.getPieces(color, BISHOP);
  Color otherColor = getOppositeColor(color);
  U64 mobZoneAdjusted  = eB->EnemyPawnAttackMap[color] & ~(board.getPieces(otherColor, QUEEN) | board.getPieces(otherColor, ROOK));

  // Bishop has penalty based on count of rammed pawns
  s += eB->RammedCount * _popCount(pieces) * BISHOP_RAMMED_PENALTY;
  if (TRACK) ft.BishopRammed[color] += eB->RammedCount * _popCount(pieces);

  // Apply a penalty for each Bishop attacked by enemy pawn
  s += HANGING_PIECE[BISHOP] * (_popCount(pieces & eB->EnemyPawnAttackMap[color]));
  if (TRACK) ft.HangingPiece[BISHOP][color] += (_popCount(pieces & eB->EnemyPawnAttackMap[color]));

    while (pieces) {

      int square = _popLsb(pieces);
      if (TRACK){
        int relSqv = color == WHITE ? _mir(square) : square;
        ft.BishopPsqtBlack[relSqv][color]++;
      }

      // Mobility
      // Bishops mobility are scanning through bishops and queens
      U64 attackBitBoard = board.getMobilityForSquare(BISHOP, color, square, mobZoneAdjusted);
      s += BISHOP_MOBILITY[_popCount(attackBitBoard)];
      if (TRACK) ft.BishopMobility[_popCount(attackBitBoard)][color]++;

      // Save our attacks for further use
      eB->AttackedSquares[color] |= attackBitBoard;

      // BishopAttackMinor
      U64 BishopAttackMinor = (board.getPieces(otherColor, KNIGHT) | board.getPieces(otherColor, BISHOP)) & attackBitBoard;
      s += MINOR_ATTACKED_BY[BISHOP] * _popCount(BishopAttackMinor);
      if (TRACK) ft.MinorAttackedBy[BISHOP][color] += _popCount(BishopAttackMinor);

      //BishopAttackRook
      s += ROOK_ATTACKED_BY[BISHOP] * _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));
      if (TRACK) ft.RookAttackedBy[BISHOP][color] += _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));

      // Bonus for bishop having central squares in mobility
      // it would mean they are not attacked by enemy pawn
      // or contain our own piece
      s += BISHOP_CENTER_CONTROL * _popCount(attackBitBoard & CENTER);
      if (TRACK) ft.BishopCenterControl[color] +=  _popCount(attackBitBoard & CENTER);

      // Apply a penalty when our bishop has two our own pawns in front of it
      if (board.getPieces(color, PAWN) & detail::OUTPOST_PROTECTION[otherColor][square] &&
          (color == WHITE ? _row(square) < 2 : 7 - _row(square) < 2) &&
          _popCount(attackBitBoard) < 4){
        s += BISHOP_GLORIFIED_PAWN;
        if (TRACK) ft.BishopGlorifiedPawn[color]++;
      }



      // If Bishop attacking squares near enemy king
      // Adjust our kind Danger code
      int kingAttack = _popCount(attackBitBoard & eB->EnemyKingZone[color]);
      if (kingAttack > 0){
        eB->KingAttackers[color]++;
        eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[BISHOP];
      }

      // See if a Bishop is attacking an enemy unprotected pawn
      s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
      if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

      // OUTPOSTED BISHOP
      // We use separed PSQT for protected and unprotected outposts
      // Unprotected outposts are only considered outposts
      // if there is pawn in front spawn of outposted piece
      if ((board.getPieces(getOppositeColor(color), PAWN) & detail::OUTPOST_MASK[color][square]) == ZERO){
        int relSqv = color == WHITE ? _mir(square) : square;
        if (detail::OUTPOST_PROTECTION[color][square] & board.getPieces(color, PAWN)){
          s += BISHOP_PROT_OUTPOST_BLACK[relSqv];
          eB->OutPostedLines[color] = eB->OutPostedLines[color] | detail::FILES[_col(square)];
          if (TRACK) ft.BishopOutProtBlack[relSqv][color]++;
        } else if ((board.getPieces(getOppositeColor(color), PAWN) & detail::PASSED_PAWN_MASKS[color][square]) != ZERO){
          s += BISHOP_OUTPOST_BLACK[relSqv];
          if (TRACK) ft.BishopOutBlack[relSqv][color]++;
        }
      }
    }

  return s;
}

inline int Eval::evaluateKNIGHT(const Board & board, Color color, evalBits * eB){
  int s = 0;
  U64 pieces = board.getPieces(color, KNIGHT);
  Color otherColor = getOppositeColor(color);
  U64 mobZoneAdjusted  = eB->EnemyPawnAttackMap[color] & ~(board.getPieces(otherColor, QUEEN) | board.getPieces(otherColor, ROOK));

  // Apply penalty for each Knight attacked by opponents pawn
  s += HANGING_PIECE[KNIGHT] * (_popCount(pieces & eB->EnemyPawnAttackMap[color]));
  if (TRACK) ft.HangingPiece[KNIGHT][color] += (_popCount(pieces & eB->EnemyPawnAttackMap[color]));

    while (pieces) {

      int square = _popLsb(pieces);
      if (TRACK){
        int relSqv = color == WHITE ? _mir(square) : square;
        ft.KnightPsqtBlack[relSqv][color]++;
      }

      // Mobility
      U64 attackBitBoard = board.getMobilityForSquare(KNIGHT, color, square, mobZoneAdjusted);
      s += KNIGHT_MOBILITY[_popCount(attackBitBoard)];
      if (TRACK) ft.KnigthMobility[_popCount(attackBitBoard)][color]++;

      // Save our attacks for further use
      eB->AttackedSquares[color] |= attackBitBoard;

      // KnightAttackMinor
      U64 KnightAttackMinor = (board.getPieces(otherColor, KNIGHT) | board.getPieces(otherColor, BISHOP)) & attackBitBoard;
      s += MINOR_ATTACKED_BY[KNIGHT] * _popCount(KnightAttackMinor);
      if (TRACK) ft.MinorAttackedBy[KNIGHT][color] += _popCount(KnightAttackMinor);

      //RookAttackRook
      s += ROOK_ATTACKED_BY[KNIGHT] * _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));
      if (TRACK) ft.RookAttackedBy[KNIGHT][color] += _popCount(attackBitBoard & board.getPieces(otherColor, ROOK));


      // If Knight attacking squares near enemy king
      // Adjust our kind Danger code
      int kingAttack = _popCount(attackBitBoard & eB->EnemyKingZone[color]);
      if (kingAttack > 0){
        eB->KingAttackers[color]++;
        eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[KNIGHT];
      }

      // See if a Knight is attacking an enemy unprotected pawn
      s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
      if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

      // OUTPOSTED KNIGHT
      // We use separed PSQT for protected and unprotected outposts
      // Unprotected outposts are only considered outposts
      // if there is pawn in front spawn of outposted piece
      if ((board.getPieces(getOppositeColor(color), PAWN) & detail::OUTPOST_MASK[color][square]) == ZERO){
        int relSqv = color == WHITE ? _mir(square) : square;
        if (detail::OUTPOST_PROTECTION[color][square] & board.getPieces(color, PAWN)){
          s += KNIGHT_PROT_OUTPOST_BLACK[relSqv];
          eB->OutPostedLines[color] = eB->OutPostedLines[color] | detail::FILES[_col(square)];
          if (TRACK) ft.KnightOutProtBlack[relSqv][color]++;
        } else if ((board.getPieces(getOppositeColor(color), PAWN) & detail::PASSED_PAWN_MASKS[color][square]) != ZERO){
          s += KNIGHT_OUTPOST_BLACK[relSqv];
          if (TRACK) ft.KnightOutBlack[relSqv][color]++;
        }
      }
    }
  return s;
}

inline int Eval::evaluateKING(const Board & board, Color color, evalBits * eB){
  int s = 0;

  U64 pieces = board.getPieces(color, KING);
  int square = _popLsb(pieces);
  Color otherColor = getOppositeColor(color);

  // Mobility
  U64 attackBitBoard = board.getMobilityForSquare(KING, color, square, eB->EnemyPawnAttackMap[color]);
  s += KING_MOBILITY[_popCount(attackBitBoard)];
  if (TRACK) ft.KingMobility[_popCount(attackBitBoard)][color]++;
  if (TRACK){
      int relSqv = color == WHITE ? _mir(square) : square;
      ft.KingPsqtBlack[relSqv][color]++;
  }

  // Save our attacks for further use
  eB->AttackedSquares[color] |= attackBitBoard;

  // See if our king is on the Openish-files
  // Test for Open - SemiOpenToUs - SemiOpenToEnemy
  // General idea is from SF HCE
  U64 file       = detail::FILES[_col(square)];
  U64 ourPawns   = board.getPieces(color, PAWN);
  U64 enemyPawns = board.getPieces(otherColor, PAWN);

  // See if a King is attacking an enemy unprotected pawn
  s += KING_ATTACK_PAWN * _popCount(attackBitBoard & enemyPawns);
  if (TRACK) ft.KingAttackPawn[color] += _popCount(attackBitBoard & enemyPawns);

  if (((file & ourPawns) == 0) && ((file & enemyPawns) == 0)){
    s += KING_OPEN_FILE;
    if (TRACK) ft.KingOpenFile[color]++;
  } else if ((file & ourPawns) == 0){
    s += KING_OWN_SEMI_FILE;
    if (TRACK) ft.KingSemiOwnFile[color]++;
  } else if ((file & enemyPawns) == 0){
    s += KING_ENEMY_SEMI_LINE;
    if (TRACK) ft.KingSemiEnemyFile[color]++;
  }

  U64 tmpPawns = eB->Passers[color];
  while (tmpPawns != ZERO) {

    // Evaluate distance of our king to each of our own passers
    int passerSquare = _popLsb(tmpPawns);
    s += KING_PASSER_DISTANCE_FRIENDLY[Eval::detail::DISTANCE[square][passerSquare]];
    if (TRACK) ft.KingFriendlyPasser[Eval::detail::DISTANCE[square][passerSquare]][color]++;


    // if we are within 1 square to passer
    // determine if King is ahead of pawn, behind or on equal rank
    if (Eval::detail::DISTANCE[square][passerSquare] == 1){
      int kingRank = color == WHITE ? _row(square) : 7 - _row(square);
      int pawnRank = color == WHITE ? _row(passerSquare) : 7 - _row(passerSquare);
      if (kingRank > pawnRank){
        s += KING_AHEAD_PASSER;
        if (TRACK) ft.KingAheadPasser[color]++;
      } else if (kingRank == pawnRank){
        s += KING_EQUAL_PASSER;
        if (TRACK) ft.KingEqualPasser[color]++;
      } else if (kingRank < pawnRank){
        s += KING_BEHIND_PASSER;
        if (TRACK) ft.KingBehindPasser[color]++;
      }
    }
  }

  tmpPawns = eB->Passers[getOppositeColor(color)];
  while (tmpPawns != ZERO) {

    // Evaluate distance of our king to each of enemys passers
    int passerSquare = _popLsb(tmpPawns);
    s += KING_PASSER_DISTANCE_ENEMY[Eval::detail::DISTANCE[square][passerSquare]];
    if (TRACK) ft.KingEnemyPasser[Eval::detail::DISTANCE[square][passerSquare]][color]++;
  }

  return s;
}

inline int Eval::evaluatePAWNS(const Board & board, Color color, evalBits * eB){
  int s = 0;

  U64 pawns = board.getPieces(color, PAWN);
  U64 tmpPawns = pawns;

  while (tmpPawns != ZERO) {

    int square = _popLsb(tmpPawns);
    int pawnCol = _col(square);
    int r = color == WHITE ? _row(square) : 7 - _row(square);

    if (TRACK){
      int relSqv = color == WHITE ? _mir(square) : square;
      ft.PawnPsqtBlack[relSqv][color]++;
    }

    // add bonuses if the pawn is passed
    if ((board.getPieces(getOppositeColor(color), PAWN) & detail::PASSED_PAWN_MASKS[color][square]) == ZERO){
      eB->Passers[color] = eB->Passers[color] | (ONE << square);

      s += PASSED_PAWN_RANKS[r] + PASSED_PAWN_FILES[pawnCol];
      if (TRACK){
        ft.PassedPawnRank[r][color]++;
        ft.PassedPawnFile[pawnCol][color]++;
      }
      // if the pawn is passed evaluate how far
      // is it from other passers (_col-wise)
      U64 tmpPassers = eB->Passers[color];
      while (tmpPassers != ZERO){
        int tPasSquare = _popLsb(tmpPassers);
        int tPasCol = _col(tPasSquare);

        s += PASSED_PASSED_DISTANCE[abs(tPasCol - pawnCol)];
        if (TRACK) ft.PassedPassedDistance[abs(tPasCol - pawnCol)][color]++;
      }

      // Add pawn to the passers list for further use
      eB->Passers[color] = eB->Passers[color] | (ONE << square);
    }

    // add penalties for the doubled pawns
    if (_popCount(tmpPawns & detail::FILES[pawnCol]) > 0){
      if (TRACK) ft.PawnDoubled[color]++;
      s += DOUBLED_PAWN_PENALTY;
    }

    // score a pawn if it is isolated
    if (!(detail::NEIGHBOR_FILES[pawnCol] & pawns)){
      if (TRACK) ft.PawnIsolated[color]++;
      s += ISOLATED_PAWN_PENALTY;
    }

    // test on if a pawn is connected
    if ((detail::CONNECTED_MASK[square] & pawns) != 0){
      if (TRACK) ft.PawnConnected[r][color]++;
      s += PAWN_CONNECTED[r];
    }
  }

  return s;
}

inline int Eval::PiecePawnInteraction(const Board &board, Color color, evalBits & eB){
  int s = 0;
  Color otherColor = getOppositeColor(color);
  U64 blocked = ZERO;
  U64 pieces, tmpPawns = ZERO;

  // 1. Blocked pawns - separate for passers and non-passers

  // Get major blockers and pawns
  //  a. Non-passer pawns
  pieces = board.getPieces(color, KNIGHT) | board.getPieces(color, BISHOP) |
           board.getPieces(color, ROOK)   | board.getPieces(color, QUEEN);
  tmpPawns = board.getPieces(otherColor, PAWN) ^ eB.Passers[otherColor];

  // Shift stuff, give evaluation
  // Do not forget to add pawns to BlockedBB for later use
  tmpPawns = otherColor == WHITE ? tmpPawns << 8 : tmpPawns >> 8;
  s += PAWN_BLOCKED * (_popCount(pieces & tmpPawns));
  blocked |= (pieces & tmpPawns);
  if (TRACK) ft.PawnBlocked[color] += (_popCount(pieces & tmpPawns));

  // same stuff, but with passed pawns
  tmpPawns = otherColor == WHITE ? eB.Passers[otherColor] << 8 : eB.Passers[otherColor] >> 8;
  s += PASSER_BLOCKED * (_popCount(pieces & tmpPawns));
  blocked |= (pieces & tmpPawns);
  if (TRACK) ft.PassersBlocked[color] += (_popCount(pieces & tmpPawns));

  // 2. Minors immediately behind our pawns - separate for passers and non-passers
  // Get minor pieces, shift them, and see if our pawn is & with them
  // Same as above, separate for passers and non-passers
  pieces = board.getPieces(color, KNIGHT) | board.getPieces(color, BISHOP);
  pieces = color == WHITE ? pieces << 8 : pieces >> 8;
  tmpPawns = board.getPieces(color, PAWN) ^ eB.Passers[color];
  s += MINOR_BEHIND_PAWN * _popCount(tmpPawns & pieces);
  if (TRACK) ft.MinorBehindPawn[color] += _popCount(tmpPawns & pieces);

  s += MINOR_BEHIND_PASSER * _popCount(eB.Passers[color] & pieces);
  if (TRACK) ft.MinorBehindPasser[color] += _popCount(eB.Passers[color] & pieces);

  // Passer - piece evaluation

  tmpPawns = eB.Passers[color] & ENEMY_SIDE[color];
  pieces   = board.getAllPieces(color) | board.getAllPieces(otherColor);
  int forward = color == WHITE ? 8 : -8;
  U64 posAdvance = ~eB.AttackedSquares[otherColor] | eB.AttackedSquares[color];

  while (tmpPawns != ZERO) {

    int square = _popLsb(tmpPawns);
    int r = color == WHITE ? _row(square) : 7 - _row(square);

    // 3. Free passer evaluation
    // Add bonus for each passed pawn that has no piece blocking its advance
    // rank - based evaluation
    if ((detail::FORWARD_BITS[color][square] & pieces) == ZERO){
      s += PASSED_PAWN_FREE[r];
      if (TRACK) ft.PassedPawnFree[r][color]++;
    }

    // 4. Moving passer evaluation
    // Add bonus when passed pawn nex square is not attacked
    // and pawn can be advanced
    if ((((ONE << (square + forward)) & pieces) == 0) &&
        (((ONE << (square + forward)) & posAdvance) != 0)){
          s += PASSED_PAWN_POS_ADVANCE[r];
          if (TRACK) ft.PassedPawnPosAdvance[r][color]++;
        }

  }


  return s;
}


int Eval::evaluate(const Board &board, Color color) {

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

  bool DrawishMaterial = IsItDeadDraw(w_N, w_B, w_R, w_Q, b_N, b_B, b_R, b_Q);
  if (DrawishMaterial && w_P == 0 && b_P == 0){
    return 0;
  }

  score += (w_P - b_P) * MATERIAL_VALUES[PAWN];
  score += (w_N - b_N) * MATERIAL_VALUES[KNIGHT];
  score += (w_B - b_B) * MATERIAL_VALUES[BISHOP];
  score += (w_R - b_R) * MATERIAL_VALUES[ROOK];
  score += (w_Q - b_Q) * MATERIAL_VALUES[QUEEN];

  if (TRACK){
    ft.PawnValue[color]+= w_P;
    ft.PawnValue[otherColor]+= b_P;

    ft.KnightValue[color]+= w_N;
    ft.KnightValue[otherColor]+= b_N;

    ft.BishopValue[color]+= w_B;
    ft.BishopValue[otherColor]+= b_B;

    ft.RookValue[color]+= w_R;
    ft.RookValue[otherColor]+= b_R;

    ft.QueenValue[color]+= w_Q;
    ft.QueenValue[otherColor]+= b_Q;
  }

  // Piece square tables
  score += board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(otherColor);

  // Create evalBits stuff
  evalBits eB = Eval::Setupbits(board);

  // Pawn structure
  int pScore = 0;
  pawn_HASH_Entry pENTRY  = myHASH->pHASH_Get(board.getPawnStructureZKey().getValue());

  #ifndef _TUNE_
  if (pENTRY.posKey != 0){
    eB.Passers[WHITE] = pENTRY.wPassers;
    eB.Passers[BLACK] = pENTRY.bPassers;

    if (color == BLACK) {
      score -= pENTRY.score;
    } else {
      score += pENTRY.score;
    }

  }
  else
  #endif
  {
    // PawnSupported
    // Apply bonus for each pawn protected by allied pawn
    pScore += PAWN_SUPPORTED * _popCount(board.getPieces(WHITE, PAWN) & eB.EnemyPawnAttackMap[BLACK]);
    pScore -= PAWN_SUPPORTED * _popCount(board.getPieces(BLACK, PAWN) & eB.EnemyPawnAttackMap[WHITE]);
    if (TRACK){
      ft.PawnSupported[WHITE] +=_popCount(board.getPieces(WHITE, PAWN) & eB.EnemyPawnAttackMap[BLACK]);
      ft.PawnSupported[BLACK] +=_popCount(board.getPieces(BLACK, PAWN) & eB.EnemyPawnAttackMap[WHITE]);
    }

    // Evaluate pawn-by-pawn terms
    // Passed, isolated, doubled
    pScore += evaluatePAWNS(board, WHITE, &eB) - evaluatePAWNS(board, BLACK, &eB);

    myHASH->pHASH_Store(board.getPawnStructureZKey().getValue(), eB.Passers[WHITE], eB.Passers[BLACK], pScore);

    score += color == WHITE ? pScore : -pScore;
  }

  // Evaluate pieces
  score +=  evaluateBISHOP(board, color, &eB) - evaluateBISHOP(board, otherColor, &eB)
          + evaluateKNIGHT(board, color, &eB) - evaluateKNIGHT(board, otherColor, &eB)
          + evaluateROOK  (board, color, &eB) - evaluateROOK  (board, otherColor, &eB)
          + evaluateQUEEN (board, color, &eB) - evaluateQUEEN (board, otherColor, &eB)
          + evaluateKING  (board, color, &eB)  - evaluateKING  (board, otherColor, &eB);

  score +=  PiecePawnInteraction(board, color, eB)
          - PiecePawnInteraction(board, otherColor, eB);

  // King pawn shield
  // Tapering is included in, so we count it in both phases
  score += kingShieldSafety(board, color, b_Q, &eB) - kingShieldSafety(board, otherColor, w_Q, &eB);

  // evluate king danger
  int attackScore = eB.KingAttackPower[color] * COUNT_TO_POWER[std::min(7, eB.KingAttackers[color])] / 128;
  score += gS(std::max(0, attackScore), 0);
  attackScore = eB.KingAttackPower[otherColor] * COUNT_TO_POWER[std::min(7, eB.KingAttackers[otherColor])] / 128;
  score -= gS(std::max(0, attackScore), 0);

  // Bishop pair
  if (w_B > 1){
    score += BISHOP_PAIR_BONUS;
    if (TRACK) ft.BishopPair[color]++;
  }

  if (b_B > 1){
    score -= BISHOP_PAIR_BONUS;
    if (TRACK) ft.BishopPair[otherColor]++;
  }

  if (TRACK){
    ft.FinalEval = score;
    ft.OCBscale  = false;
  }

  // Calculation of the phase value
  int phase = getPhase(board);

  // adjust EG eval based on pawns left
  int NormalScale = 64;
  int MaxScale = 128;
  int StrongPawn = egS(score) > 0 ? w_P : b_P;
  int Scale = std::min(MaxScale, 32 + 8 * StrongPawn);


  // Interpolate between opening/endgame scores depending on the phase
  int final_eval = ((opS(score) * (MAX_PHASE - phase)) + (egS(score) * phase * Scale / NormalScale)) / MAX_PHASE;

  if (w_Q == 0 && b_Q == 0 &&
      w_R == 0 && b_R == 0 &&
      w_N == 0 && b_N == 0 &&
      w_B == 1 && b_B == 1){
    U64 bothBishops = board.getPieces(color, BISHOP) | board.getPieces(otherColor, BISHOP);
    if (_popCount(bothBishops & WHITE_SQUARES) == 1){
        final_eval = final_eval / 2;
        if (TRACK) ft.OCBscale = true;
    }
  }

  return final_eval + TEMPO;
}

int Eval::evalTestSuite(const Board &board, Color color)
{
  return 0;
}
