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

int MATERIAL_VALUES_TUNABLE[6] = {
        [PAWN] = 0,
        [ROOK] = 0,
        [KNIGHT] = 0,
        [BISHOP] = 0,
        [QUEEN] = 0,
        [KING] = 0
};

U64 Eval::detail::FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
U64 Eval::detail::DISTANCE[64][64];
U64 Eval::detail::NEIGHBOR_FILES[8] = {
    FILE_B,
    FILE_A | FILE_C,
    FILE_B | FILE_D,
    FILE_C | FILE_E,
    FILE_D | FILE_F,
    FILE_E | FILE_G,
    FILE_F | FILE_H,
    FILE_G
};
U64 Eval::detail::TWO_PLUS_FILES[8] = {
    FILE_A | FILE_B | FILE_C,
    FILE_A | FILE_B | FILE_C | FILE_D,
    FILE_A | FILE_B | FILE_C | FILE_D | FILE_E,
    FILE_B | FILE_C | FILE_D | FILE_E | FILE_F,
    FILE_C | FILE_D | FILE_E | FILE_F | FILE_G,
    FILE_D | FILE_E | FILE_F | FILE_G | FILE_H,
    FILE_E | FILE_F | FILE_G | FILE_H,
    FILE_F | FILE_G | FILE_H,
};

U64 Eval::detail::STORM_FILES[8] = {
    FILE_A | FILE_B | FILE_C,
    FILE_A | FILE_B | FILE_C,
    FILE_B | FILE_C | FILE_D,
    FILE_C | FILE_D | FILE_E,
    FILE_D | FILE_E | FILE_F,
    FILE_E | FILE_F | FILE_G,
    FILE_F | FILE_G | FILE_H,
    FILE_F | FILE_G | FILE_H
};

U64 Eval::detail::PASSED_PAWN_MASKS[2][64];
U64 Eval::detail::OUTPOST_MASK[2][64];
U64 Eval::detail::CONNECTED_MASK[64];
U64 Eval::detail::OUTPOST_PROTECTION[2][64];
U64 Eval::detail::KINGZONE[2][64];
U64 Eval::detail::FORWARD_BITS[2][64];
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
      // When king is on the side files (A and H) include
      // squares on the C and F files to the kingzone
      if (_col(square) == 0){
        detail::KINGZONE[color][square] = detail::KINGZONE[color][square] | detail::KINGZONE[color][square] << 1;
      }else if (_col(square) == 7){
        detail::KINGZONE[color][square] = detail::KINGZONE[color][square] | detail::KINGZONE[color][square] >> 1;
      }

    }
  }
}

evalBits Eval::Setupbits(const Board &board){
  evalBits eB;
  U64 doubleAttacked[2] = {0};
  U64 pawnFrontSpans[2] = {0};
  U64 attFrontSpawn[2]  = {0};
  for (auto color : { WHITE, BLACK }) {
    U64 pBB = board.getPieces(color, PAWN);

    eB.EnemyPawnAttackMap[!color] = color == WHITE ? ((pBB << 9) & ~FILE_A) | ((pBB << 7) & ~FILE_H)
                                                   : ((pBB >> 9) & ~FILE_H) | ((pBB >> 7) & ~FILE_A);

    attFrontSpawn[color] = eB.EnemyPawnAttackMap[!color] | (color == WHITE ? eB.EnemyPawnAttackMap[!color] << 8 : eB.EnemyPawnAttackMap[!color] >> 8);
    attFrontSpawn[color] |= color == WHITE ? attFrontSpawn[color] << 16 : attFrontSpawn[color] >> 16;
    attFrontSpawn[color] |= color == WHITE ? attFrontSpawn[color] << 32 : attFrontSpawn[color] >> 32;

    doubleAttacked[color] =  color == WHITE ? ((pBB << 9) & ~FILE_A) & ((pBB << 7) & ~FILE_H)
                                            : ((pBB >> 9) & ~FILE_H) & ((pBB >> 7) & ~FILE_A);

    pawnFrontSpans[color]  = pBB | (color == WHITE ? pBB << 8 : pBB >> 8);
    pawnFrontSpans[color] |= color == WHITE ?  pawnFrontSpans[color] << 16 :  pawnFrontSpans[color] >> 16;
    pawnFrontSpans[color] |= color == WHITE ?  pawnFrontSpans[color] << 32 :  pawnFrontSpans[color] >> 32;


    U64 king = board.getPieces(color, KING);
    eB.EnemyKingZone[!color] = detail::KINGZONE[color][_bitscanForward(king)];
  }

  eB.PossibleGenOutposts[WHITE] = pawnFrontSpans[BLACK] & ~attFrontSpawn[BLACK];
  eB.PossibleProtOutposts[WHITE] = ~attFrontSpawn[BLACK] & eB.EnemyPawnAttackMap[BLACK];

  eB.PossibleGenOutposts[BLACK] = pawnFrontSpans[WHITE] & ~attFrontSpawn[WHITE];
  eB.PossibleProtOutposts[BLACK] = ~attFrontSpawn[WHITE] & eB.EnemyPawnAttackMap[WHITE];

  eB.RammedCount =  _popCount((board.getPieces(BLACK, PAWN) >> 8) & board.getPieces(WHITE, PAWN)) +
                   (_popCount((board.getPieces(BLACK, PAWN) >> 8) & doubleAttacked[WHITE]) +
                    _popCount((board.getPieces(WHITE, PAWN) << 8) & doubleAttacked[BLACK]) / 2);

  eB.KingAttackers[0] = 0, eB.KingAttackers[1] = 0;
  eB.KingAttackPower[0] = START_ATTACK_VALUE, eB.KingAttackPower[1] = START_ATTACK_VALUE;
  eB.Passers[0] = 0, eB.Passers[1] = 0;
  eB.AttackedSquares[0] = 0, eB.AttackedSquares[1] = 0;
  eB.AttackedByKing[0] = 0, eB.AttackedByKing[1] = 0;
  return eB;
}

void Eval::SetupTuning(PieceType piece, int value){
  MATERIAL_VALUES_TUNABLE [piece] = value;
}

int Eval::getMaterialValue(int phase, PieceType pieceType) {
  return phase == OPENING ? opS(MATERIAL_VALUES[pieceType])
                          : egS(MATERIAL_VALUES[pieceType]);
}

inline bool Eval::IsItDeadDraw (const Board &board, Color color){
  Color otherColor = getOppositeColor(color);

  // если есть хоть одна пешка или королева, то играем
  if ((board.getPieces(color, QUEEN) | board.getPieces(otherColor, QUEEN)) ||
      (board.getPieces(color, PAWN) | board.getPieces(otherColor, PAWN))){
    return false;
  }

  int ownBishop = _popCount(board.getPieces(color, BISHOP));
  int oppBishop = _popCount(board.getPieces(otherColor, BISHOP));
  int ownKnight = _popCount(board.getPieces(color, KNIGHT));
  int oppKnight = _popCount(board.getPieces(otherColor, KNIGHT));
  int ownRook   = _popCount(board.getPieces(color, ROOK));
  int oppRook   = _popCount(board.getPieces(otherColor, ROOK));

  if (!(board.getPieces(color, ROOK) | board.getPieces(otherColor, ROOK))){ // нет пешек, нет королев, нет ладей
    if (!(board.getPieces(color, BISHOP) | board.getPieces(otherColor, BISHOP))){  // нет пешек, ладей, слонов.
        if (_popCount(board.getPieces(color, KNIGHT)) < 3 && _popCount(board.getPieces(otherColor, KNIGHT)) < 3){ // меньше 2х коней = ничья
          return true;
        }
    } else if (!(board.getPieces(color, KNIGHT) | board.getPieces(otherColor, KNIGHT))){ // нет пешек, королев, ладей, коней
        if (abs(ownBishop - oppBishop) < 2){
          return true;
        }
    } else if ((ownKnight < 3 && ownBishop == 0) || (ownBishop == 1 &&  ownKnight == 0)){
      if ((oppKnight < 3 && oppBishop == 0)||(oppBishop == 1 && oppKnight == 0)){
        return true;
      }
    }
  } else {                             // ладьи есть
    if (ownRook == 1 && oppRook == 1){
      if (ownKnight + ownBishop < 2 && oppKnight + oppBishop < 2){    // тут немного криво, так как BR vs R выигрывают
        return true;
      }
    } else if (ownRook == 1 && oppRook == 0){
      if (ownKnight + ownBishop == 0 && oppKnight + oppBishop > 1){
        return true;
      }
    } else if (oppRook == 1 && ownRook == 0){
      if (oppKnight + oppBishop == 0 && ownKnight + ownBishop > 1){
        return true;
      }
    }
  }

  return false;
}

inline int Eval::kingShieldSafety(const Board &board, Color color, evalBits * eB){
      //King safety - замена pawnsShieldingKing
    // идея в том, чтобы
    // а. Найти позицию короля
    // b. Для каждой выбранной позиции мы имеем некую маску и скор того, насколько она "безопасна"

    Color otherColor = getOppositeColor(color);
    // Упрощённо будем считать, что если нет ферзя у врага, то мы в безопасности.
    if (!board.getPieces(getOppositeColor(color), QUEEN)){
      if (TRACK) ft.KingLowDanger[color]++;
      eB->KingAttackPower[otherColor] += -1 * opS(KING_LOW_DANGER);
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
      eB->KingAttackPower[otherColor] += (-1 * opS(KING_HIGH_DANGER));
      return KING_HIGH_DANGER;
    }
    // Cycle through all masks, if one of them is true,
    // Apply bonus for safety and score
    for (int i = 0; i < 8; i++){
      if ((pawnMap & detail::KING_PAWN_MASKS[color][cSide][i]) == detail::KING_PAWN_MASKS[color][cSide][i]){
                if (TRACK){
                  if (cSide == KingSide)  ft.KingShieldKS[i][color]++;
                  if (cSide == QueenSide) ft.KingShieldQS[i][color]++;
                }
                eB->KingAttackPower[otherColor] += cSide == KingSide ? (-1 * opS( KING_PAWN_SHIELD_KS[i])) : (-1 * opS(KING_PAWN_SHIELD_QS[i]));
                return cSide == KingSide ? KING_PAWN_SHIELD_KS[i] : KING_PAWN_SHIELD_QS[i];
            }
    }

      // если не одна из масок не прошла, то король в опасности.
      // вернуть штраф к нашей позиции
      if (TRACK) ft.KingMedDanger[color]++;
      eB->KingAttackPower[otherColor] += (-1 * opS(KING_MED_DANGER));
      return KING_MED_DANGER;

}

inline int Eval::evaluateQUEEN(const Board & board, Color color, evalBits * eB){
  int s = 0;

  Color otherColor = getOppositeColor(color);
  U64 pieces = board.getPieces(color, QUEEN);
  int enemyKingSquare = _bitscanForward(board.getPieces(otherColor, KING));

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

    // See if a Queen is attacking an enemy unprotected pawn
    s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
    if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

    // If Queen attacking squares near enemy king
    // Adjust our kind Danger code
    int kingAttack = _popCount(attackBitBoard & eB->EnemyKingZone[color]);
    U64 kingChecks = attackBitBoard & board.getAttacksForSquare(QUEEN, getOppositeColor(color), enemyKingSquare);
    int kingChecksCount = _popCount(kingChecks);
    int KingFaceChecksCount = _popCount(kingChecks & board.getAttacksForSquare(KING, getOppositeColor(color), enemyKingSquare) & eB->AttackedSquares[color]);
    if (kingAttack > 0 || kingChecksCount > 0){
      eB->KingAttackers[color]++;
      eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[QUEEN];
      eB->KingAttackPower[color] += (kingChecksCount - KingFaceChecksCount) * PIECE_CHECK_POWER[QUEEN];
      eB->KingAttackPower[color] += KingFaceChecksCount * QUEEN_FACE_CHECK;
    }

    // Save our attacks for further use
    eB->AttackedSquares[color] |= attackBitBoard;
  }

  return s;
}

inline int Eval::evaluateROOK(const Board & board, Color color, evalBits * eB){
  int s = 0;
  Color otherColor = getOppositeColor(color);
  U64 pieces = board.getPieces(color, ROOK);
  U64 mobZoneAdjusted  = eB->EnemyPawnAttackMap[color] & ~board.getPieces(otherColor, QUEEN);
  U64 rammedPawns = (board.getPieces(BLACK, PAWN) >> 8) & board.getPieces(WHITE, PAWN);
  int enemyKingSquare = _bitscanForward(board.getPieces(otherColor, KING));

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

    // Rook Attack Queen
    s += QUEEN_ATTACKED_BY[ROOK] * _popCount(attackBitBoard & board.getPieces(otherColor, QUEEN));
    if (TRACK) ft.QueenAttackedBy[ROOK][color] += _popCount(attackBitBoard & board.getPieces(otherColor, QUEEN));

    // If Rook attacking squares near enemy king
    // Adjust our kind Danger code
    int kingAttack = _popCount(attackBitBoard & eB->EnemyKingZone[color]);
    int kingChecks = _popCount(attackBitBoard & board.getAttacksForSquare(ROOK, getOppositeColor(color), enemyKingSquare));
    if (kingAttack > 0 || kingChecks > 0){
      eB->KingAttackers[color]++;
      eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[ROOK];
      eB->KingAttackPower[color] += kingChecks * PIECE_CHECK_POWER[ROOK];
    }

    // See if a Rook is attacking an enemy unprotected pawn
    s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
    if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

    // Open/semiopen file detection
    // we differentiate between open/semiopen based on
    // if there are enemys protected outpost here
    U64 file = detail::FILES[_col(square)];
    U64 outPostedPieces = eB->PossibleProtOutposts[otherColor] & (board.getPieces(otherColor, BISHOP) | board.getPieces(otherColor, KNIGHT));

    if ( ((file & board.getPieces(color, PAWN)) == 0)
      && ((file & board.getPieces(otherColor, PAWN)) == 0)){
      s += ROOK_OPEN_FILE_BONUS[((file & outPostedPieces) != 0)];
      if (TRACK) ft.RookOpenFile[((file & outPostedPieces) != 0)][color]++;
    }
    else if ((file & board.getPieces(color, PAWN)) == 0){
      s += ROOK_SEMI_FILE_BONUS[((file & outPostedPieces) != 0)];
      if (TRACK) ft.RookHalfFile[((file & outPostedPieces) != 0)][color]++;
    }else if ((file & board.getPieces(color, PAWN) & eB->EnemyPawnAttackMap[color]) != 0){
      s += ROOK_LINE_TENSION;
      if (TRACK) ft.RookTensionLine[color]++;
    }else if ((file & rammedPawns) != 0){
      s += ROOK_RAMMED_LINE;
      if (TRACK) ft.RookRammedLine[color]++;
    }
  }

  return s;
}

inline int Eval::evaluateBISHOP(const Board & board, Color color, evalBits * eB){
  int s = 0;

  U64 pieces = board.getPieces(color, BISHOP);
  Color otherColor = getOppositeColor(color);
  U64 mobZoneAdjusted  = eB->EnemyPawnAttackMap[color] & ~(board.getPieces(otherColor, QUEEN) | board.getPieces(otherColor, ROOK));
  int enemyKingSquare = _bitscanForward(board.getPieces(otherColor, KING));

  // Bishop pair
  if (_popCount(pieces) > 1){
    s += BISHOP_PAIR_BONUS;
    if (TRACK) ft.BishopPair[color]++;
  }

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

      // Bishop Attack Queen
      s += QUEEN_ATTACKED_BY[BISHOP] * _popCount(attackBitBoard & board.getPieces(otherColor, QUEEN));
      if (TRACK) ft.QueenAttackedBy[BISHOP][color] += _popCount(attackBitBoard & board.getPieces(otherColor, QUEEN));

      // If Bishop attacking squares near enemy king
      // Adjust our kind Danger code
      int kingAttack = _popCount(attackBitBoard & eB->EnemyKingZone[color]);
      int kingChecks = _popCount(attackBitBoard & board.getAttacksForSquare(BISHOP, getOppositeColor(color), enemyKingSquare));
      if (kingAttack > 0 || kingChecks > 0){
        eB->KingAttackers[color]++;
        eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[BISHOP];
        eB->KingAttackPower[color] += kingChecks * PIECE_CHECK_POWER[BISHOP];
      }

      // See if a Bishop is attacking an enemy unprotected pawn
      s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
      if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

      // OUTPOSTED BISHOP
      // See if bishop can jump into the good protected outpost
      s += BISHOP_POS_PROUTPOST_JUMP * _popCount(DECENT_BISHOP_PROT_OUTPOST[color] & eB->PossibleProtOutposts[color] & attackBitBoard);
      if (TRACK) ft.BishopProtOutJump[color] += _popCount(DECENT_BISHOP_PROT_OUTPOST[color] & eB->PossibleProtOutposts[color] & attackBitBoard);
      // See if bishop can jump itno the general Outpost
      s += BISHOP_POS_GENOUTPOST_JUMP * _popCount(DECENT_BISHOP_GEN_OUTPOST & eB->PossibleGenOutposts[color] & attackBitBoard);
      if (TRACK) ft.BishopGenOutJump[color] += _popCount(DECENT_BISHOP_GEN_OUTPOST & eB->PossibleGenOutposts[color] & attackBitBoard);

      // We use separed PSQT for protected and unprotected outposts
      // Unprotected outposts are only considered outposts
      // if there is pawn in front spawn of outposted piece
      if (eB->PossibleProtOutposts[color] & (ONE << square)){
        s += BISHOP_PROT_OUTPOST_BLACK[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]];
        if (TRACK) ft.BishopOutProtBlack[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]][color]++;
      } else if (eB->PossibleGenOutposts[color] & (ONE << square)){
        s += BISHOP_OUTPOST_BLACK[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]];
        if (TRACK) ft.BishopOutBlack[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]][color]++;
      }
    }

  return s;
}

inline int Eval::evaluateKNIGHT(const Board & board, Color color, evalBits * eB){
  int s = 0;
  U64 pieces = board.getPieces(color, KNIGHT);
  Color otherColor = getOppositeColor(color);
  U64 mobZoneAdjusted  = eB->EnemyPawnAttackMap[color] & ~(board.getPieces(otherColor, QUEEN) | board.getPieces(otherColor, ROOK));
  int enemyKingSquare = _bitscanForward(board.getPieces(otherColor, KING));

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
      int kingChecks = _popCount(attackBitBoard & board.getAttacksForSquare(KNIGHT, getOppositeColor(color), enemyKingSquare));
      if (kingAttack > 0 || kingChecks > 0){
        eB->KingAttackers[color]++;
        eB->KingAttackPower[color] += kingAttack * PIECE_ATTACK_POWER[KNIGHT];
        eB->KingAttackPower[color] += kingChecks * PIECE_CHECK_POWER[KNIGHT];
      }

      // See if a Knight is attacking an enemy unprotected pawn
      s += HANGING_PIECE[PAWN] * _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));
      if (TRACK) ft.HangingPiece[PAWN][color] += _popCount(attackBitBoard & board.getPieces(getOppositeColor(color), PAWN));

      // Bishop Attack Queen
      s += QUEEN_ATTACKED_BY[KNIGHT] * _popCount(attackBitBoard & board.getPieces(otherColor, QUEEN));
      if (TRACK) ft.QueenAttackedBy[KNIGHT][color] += _popCount(attackBitBoard & board.getPieces(otherColor, QUEEN));

      // OUTPOSTED KNIGHT
      // See if a Knight can jump into the protected outpost
      // Only outposts with decently good score is counted here
      s += KNIGHT_POS_PROUTPOST_JUMP * _popCount(DECENT_KNIGHT_PROT_OUTPOST[color] & eB->PossibleProtOutposts[color] & attackBitBoard);
      if (TRACK) ft.KnightProtOutJump[color] += _popCount(DECENT_KNIGHT_PROT_OUTPOST[color] & eB->PossibleProtOutposts[color] & attackBitBoard);
      // See if a knigth can jump into the general outpost
      s += KNIGHT_POS_GENOUTPOST_JUMP * _popCount(DECENT_KNIGHT_GEN_OUTPOST[color] & eB->PossibleGenOutposts[color] & attackBitBoard);
      if (TRACK) ft.KnightGenOutJump[color] += _popCount(DECENT_KNIGHT_GEN_OUTPOST[color] & eB->PossibleGenOutposts[color] & attackBitBoard);

      // We use separed PSQT for protected and unprotected outposts
      // Unprotected outposts are only considered outposts
      // if there is pawn in front spawn of outposted piece

      if (eB->PossibleProtOutposts[color] & (ONE << square)){
        s += KNIGHT_PROT_OUTPOST_BLACK[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]];
        if (TRACK) ft.KnightOutProtBlack[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]][color]++;
      } else if (eB->PossibleGenOutposts[color] & (ONE << square)){
        s += KNIGHT_OUTPOST_BLACK[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]];
        if (TRACK) ft.KnightOutBlack[color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square]][color]++;
      }
    }
  return s;
}

inline int Eval::evaluateKING(const Board & board, Color color, evalBits * eB){
  int s = 0;

  U64 pieces = board.getPieces(color, KING);
  int square = _popLsb(pieces);
  int kingCol = _col(square);
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
  eB->AttackedByKing[color] |= attackBitBoard;

  // See if our king is on the Openish-files
  // Test for Open - SemiOpenToUs - SemiOpenToEnemy
  // General idea is from SF HCE
  U64 file       = detail::FILES[kingCol];
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

  // Evaluate if King is on pawnless flang
  // Inspired by Eth code
  if ((detail::TWO_PLUS_FILES[_col(square)] & (ourPawns | enemyPawns)) == 0){
    s += KING_PAWNLESS_FLANG;
    if (TRACK) ft.KingPawnless[color]++;
  }

  // Evaluate pawn strom
  U64 dangerousPawns = enemyPawns & Eval::detail::STORM_FILES[kingCol] & ENEMY_SIDE[otherColor];
  int forward = otherColor == WHITE ? 8 : -8;

  while (dangerousPawns){

    int pSquare = _popLsb(dangerousPawns);
    int r = otherColor == WHITE ? _row(pSquare) : 7 - _row(pSquare);
    s += (((ONE << (pSquare + forward)) & ourPawns) == 0) ? PAWN_STORM_UNBLOCKED[r] : PAWN_STORM_BLOCKED[r];

    if (TRACK){

          if (((ONE << (pSquare + forward)) & ourPawns) == 0){
            ft.PawnStormUnblocked[r][color]++;
          }else{
            ft.PawnStormBlocked[r][color]++;
          }
    } 
  }

  return s;
}

inline int Eval::probePawnStructure(const Board & board, Color color, evalBits * eB){
  // Pawn structure
  int pScore = 0;
  pawn_HASH_Entry pENTRY  = myHASH->pHASH_Get(board.getPawnStructureZKey().getValue());

  #ifndef _TUNE_
  if (pENTRY.posKey != 0){
    eB->Passers[WHITE] = pENTRY.wPassers;
    eB->Passers[BLACK] = pENTRY.bPassers;

    return color == WHITE ? pENTRY.score : -pENTRY.score;
  }
  else
  #endif
  {
    pScore += evaluatePAWNS(board, WHITE, eB) - evaluatePAWNS(board, BLACK, eB);
    myHASH->pHASH_Store(board.getPawnStructureZKey().getValue(), eB->Passers[WHITE], eB->Passers[BLACK], pScore);
    return color == WHITE ? pScore : -pScore;
  }
}

inline int Eval::evaluatePAWNS(const Board & board, Color color, evalBits * eB){
  int s = 0;
  Color otherColor = getOppositeColor(color);

  U64 pawns = board.getPieces(color, PAWN);
  U64 otherPawns = board.getPieces(otherColor, PAWN);
  U64 tmpPawns = pawns;

  while (tmpPawns != ZERO) {

    int square = _popLsb(tmpPawns);
    int forwardSqv = color == WHITE ? square + 8 : square - 8;
    int relSqv = color == WHITE ? REFLECTED_SQUARE[_mir(square)] : REFLECTED_SQUARE[square];
    int pawnCol = _col(square);
    int edgeDistance = _endgedist(square);
    int r = color == WHITE ? _row(square) : 7 - _row(square);

    if (TRACK){
      int relSqv = color == WHITE ? _mir(square) : square;
      ft.PawnPsqtBlack[relSqv][color]++;
      if (board.getPieces(otherColor, QUEEN) != 0) ft.PawnPsqtBlackIsQ[relSqv][color]++;
      if (board.getPieces(color, QUEEN) != 0) ft.PawnPsqtBlackIsOwn[relSqv][color]++;
    }

    // add bonuses if the pawn is passed
    if ((otherPawns & detail::PASSED_PAWN_MASKS[color][square]) == ZERO){
      eB->Passers[color] = eB->Passers[color] | (ONE << square);

      s += PASSED_PAWN_RANKS[r] + PASSED_PAWN_FILES[edgeDistance];
      if (TRACK){
        ft.PassedPawnRank[r][color]++;
        ft.PassedPawnFile[edgeDistance][color]++;
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
    } else{
      // check if the pawn is a candidate passer
      // 1) we can just push this pawn and it will become passed
      // 2) we can push it itno trading into all becoming passed
      U64 canSupport = detail::OUTPOST_PROTECTION[color][forwardSqv] & pawns;
      U64 canEnemies = detail::OUTPOST_PROTECTION[otherColor][forwardSqv] & otherPawns;
      if ((otherPawns & (ONE << forwardSqv)) == ZERO){
        if (((otherPawns & detail::PASSED_PAWN_MASKS[color][forwardSqv]) == ZERO) ||
            ((_popCount(canSupport) >= _popCount(canEnemies)) &&
            (((otherPawns & ~canEnemies) & detail::PASSED_PAWN_MASKS[color][forwardSqv]) == ZERO))){
          s += CANDIDATE_PASSED_PAWN[r] + CANDIDATE_PASSED_PAWN_FILES[edgeDistance];
          if (TRACK) ft.CandidatePasser[r][color]++;
          if (TRACK) ft.CandidatePasserFile[edgeDistance][color]++;
        }
      }

    }



    // add penalties for the doubled pawns
    if (_popCount(tmpPawns & detail::FILES[pawnCol]) > 0 &&
        !((ONE << square) & eB->EnemyPawnAttackMap[color])){
      if (TRACK) ft.PawnDoubled[color]++;
      s += DOUBLED_PAWN_PENALTY;
    }

    // score a pawn if it is isolated
    if (!(detail::NEIGHBOR_FILES[pawnCol] & pawns) &&
        !((ONE << square) & eB->EnemyPawnAttackMap[color])){
      if (TRACK) ft.PawnIsolated[color]++;
      s += ISOLATED_PAWN_PENALTY;
    } // if pawn is not isolated check if it is backwards
    else if (!((ONE << square) & eB->EnemyPawnAttackMap[color]) &&
             !(detail::PASSED_PAWN_MASKS[otherColor][square] & pawns) &&
             !(detail::CONNECTED_MASK[square] & pawns) &&
             ((ONE << forwardSqv) & eB->EnemyPawnAttackMap[color]) &&
             !((ONE << forwardSqv) & otherPawns)){

      if (TRACK){
          if ((detail::FORWARD_BITS[color][square] & otherPawns) != 0) ft.BackwardPawn[r][color]++; else ft.BackwardOpenPawn[r][color]++;
      }
      s += ((detail::FORWARD_BITS[color][square] & otherPawns) != 0) ?  BACKWARD_PAWN[r] : BACKWARD_OPEN_PAWN[r];
    }

    // test on if a pawn is connected
    if ((detail::CONNECTED_MASK[square] & pawns) != 0){
      if (TRACK) ft.PawnConnected[relSqv][color]++;
      s += PAWN_CONNECTED[relSqv];
    }

    // test if the pawn is supported by other friendly pawn
    if ((ONE << square) & eB->EnemyPawnAttackMap[otherColor]){
      if (TRACK) ft.PawnSupported[relSqv][color]++;
      s += PAWN_SUPPORTED[relSqv];
    }
  }

  return s;
}

inline int Eval::PiecePawnInteraction(const Board &board, Color color, evalBits * eB){
  int s = 0;
  Color otherColor = getOppositeColor(color);
  U64 pieces, tmpPawns = ZERO;
  U64 AllOurAttacks = eB->AttackedSquares[color] | eB->AttackedByKing[color];
  U64 AllTheirAttacks = eB->AttackedSquares[otherColor] | eB->AttackedByKing[otherColor];

  // 1. Blocked pawns - separate for passers and non-passers

  // Get major blockers and pawns
  //  a. Non-passer pawns
  pieces = board.getPieces(color, KNIGHT) | board.getPieces(color, BISHOP) |
           board.getPieces(color, ROOK)   | board.getPieces(color, QUEEN);
  tmpPawns = board.getPieces(otherColor, PAWN) ^ eB->Passers[otherColor];

  // Shift stuff, give evaluation
  // Do not forget to add pawns to BlockedBB for later use
  tmpPawns = otherColor == WHITE ? tmpPawns << 8 : tmpPawns >> 8;
  s += PAWN_BLOCKED * (_popCount(pieces & tmpPawns));
  if (TRACK) ft.PawnBlocked[color] += (_popCount(pieces & tmpPawns));

  // same stuff, but with passed pawns
  tmpPawns = otherColor == WHITE ? eB->Passers[otherColor] << 8 : eB->Passers[otherColor] >> 8;
  s += PASSER_BLOCKED * (_popCount(pieces & tmpPawns));
  if (TRACK) ft.PassersBlocked[color] += (_popCount(pieces & tmpPawns));

  // 2. Minors immediately behind our pawns - separate for passers and non-passers
  // Get minor pieces, shift them, and see if our pawn is & with them
  // Same as above, separate for passers and non-passers
  pieces = board.getPieces(color, KNIGHT) | board.getPieces(color, BISHOP);
  pieces = color == WHITE ? pieces << 8 : pieces >> 8;
  tmpPawns = board.getPieces(color, PAWN) ^ eB->Passers[color];
  s += MINOR_BEHIND_PAWN * _popCount(tmpPawns & pieces);
  if (TRACK) ft.MinorBehindPawn[color] += _popCount(tmpPawns & pieces);

  s += MINOR_BEHIND_PASSER * _popCount(eB->Passers[color] & pieces);
  if (TRACK) ft.MinorBehindPasser[color] += _popCount(eB->Passers[color] & pieces);

  // Minor in front of own pawn - passer
  pieces = board.getPieces(color, KNIGHT) | board.getPieces(color, BISHOP);
  pieces = color == WHITE ? pieces >> 8 : pieces << 8;
  tmpPawns = board.getPieces(color, PAWN) ^ eB->Passers[color];

  s += MINOR_BLOCK_OWN_PAWN * _popCount(tmpPawns & pieces);
  if (TRACK) ft.MinorBlockOwn[color] += _popCount(tmpPawns & pieces);

  s += MINOR_BLOCK_OWN_PASSER * _popCount(eB->Passers[color] & pieces);
  if (TRACK) ft.MinorBlockOwnPassed[color] += _popCount(eB->Passers[color] & pieces);

  // Evaluate pawn push threats
  U64 posAdvance = ~AllTheirAttacks | AllOurAttacks;
  U64 targets  = board.getAllPieces(otherColor) ^ board.getPieces(otherColor, PAWN);
  U64 pawnPush = color == WHITE ? board.getPieces(color, PAWN) << 8 : board.getPieces(color, PAWN) >> 8;
  pawnPush = pawnPush & ~ (board.getAllPieces(color) | board.getAllPieces(otherColor)) & (posAdvance &  ~eB->EnemyPawnAttackMap[color]);
  pawnPush = color == WHITE ? ((pawnPush << 9) & ~FILE_A) | ((pawnPush << 7) & ~FILE_H)
                            : ((pawnPush >> 9) & ~FILE_H) | ((pawnPush >> 7) & ~FILE_A);
  s += PAWN_PUSH_THREAT * _popCount(pawnPush & targets);
  if (TRACK) ft.PawnPushThreat[color] += _popCount(pawnPush & targets);

  // Passer - piece evaluation

  tmpPawns = eB->Passers[color];
  pieces   = board.getAllPieces(color) | board.getAllPieces(otherColor);
  int forward = color == WHITE ? 8 : -8;
  int ourKingSquare = _bitscanForward(board.getPieces(color, KING));
  int enemyKingSquare = _bitscanForward(board.getPieces(otherColor, KING));

  while (tmpPawns != ZERO) {

    int square = _popLsb(tmpPawns);
    int r = color == WHITE ? _row(square) : 7 - _row(square);


    // Evaluate Distance between both kings and current passer
    s += KING_PASSER_DISTANCE_FRIENDLY[Eval::detail::DISTANCE[square][ourKingSquare]];
    s += KING_PASSER_DISTANCE_ENEMY[Eval::detail::DISTANCE[square][enemyKingSquare]];
    if (TRACK){
      ft.KingFriendlyPasser[Eval::detail::DISTANCE[square][ourKingSquare]][color]++;
      ft.KingEnemyPasser[Eval::detail::DISTANCE[square][enemyKingSquare]][color]++;
    }


    // if our own king is within 1 square to passer
    // determine if King is ahead of pawn, behind or on equal rank
    if (Eval::detail::DISTANCE[square][ourKingSquare] == 1){
      int kingRank = color == WHITE ? _row(ourKingSquare) : 7 - _row(ourKingSquare);
      int pawnRank = color == WHITE ? _row(square) : 7 - _row(square);
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

    // Evaluate distance between enemy knights and our passers
    U64 enemyKnights = board.getPieces(otherColor, KNIGHT);
    while (enemyKnights){
        int knightSuqare = _popLsb(enemyKnights);
        s += KNIGHT_PASSER_DISTANCE_ENEMY[Eval::detail::DISTANCE[square][knightSuqare] / 2];
        if (TRACK){
          ft.KnightEnemyPasser[Eval::detail::DISTANCE[square][knightSuqare] / 2][color]++;
        }
    }

    // For passers on the enemy side of the board, consider their advancing ability
    if (r >= 4){
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


  }

  int unContested = _popCount(eB->AttackedSquares[color] & eB->EnemyKingZone[color] & ~eB->AttackedSquares[otherColor]);
  eB->KingAttackPower[color] += UNCONTESTED_KING_ATTACK[std::min(unContested, 5)];
  if (board.getActivePlayer() == color) eB->KingAttackPower[color] += ATTACK_TEMPO;

  return s;
}

inline int Eval::kingDanger(Color color, const evalBits * eB){
  int attackScore = eB->KingAttackPower[color] * COUNT_TO_POWER[std::min(7, eB->KingAttackers[color])] / COUNT_TO_POWER_DIVISOR;
  return gS(std::max(0, attackScore), 0);
}

inline int Eval::TaperAndScale(const Board &board, Color color, int score){

  // Calculation of the phase value
  Color otherColor  = getOppositeColor(color);
  int phase         = board.getPhase();

  // adjust EG eval based on pawns left
  int StrongPawn = egS(score) > 0 ? _popCount(board.getPieces(color, PAWN)) : _popCount(board.getPieces(otherColor, PAWN));
  int Scale = std::min(EG_SCALE_NORMAL, EG_SCALE_MINIMAL + EG_SCALE_PAWN * StrongPawn);


  // Interpolate between opening/endgame scores depending on the phase
  int final_eval = ((opS(score) * (MAX_PHASE - phase)) + (egS(score) * phase * Scale / EG_SCALE_NORMAL)) / MAX_PHASE;

  // Initiate values for tuning
  if (TRACK){ ft.FinalEval = score;  ft.Scale  = BOTH_SCALE_NORMAL;}

  // see if we are in OCB endgame
  bool isOCB =  _popCount(board.getPieces(color, BISHOP)) == 1 && _popCount(board.getPieces(otherColor, BISHOP)) == 1 &&
                _popCount((board.getPieces(color, BISHOP) | board.getPieces(otherColor, BISHOP)) & WHITE_SQUARES) == 1;

  // correct our score if there is an OCB case
  if (isOCB){
    U64 bothQueens  = board.getPieces(color, QUEEN) | board.getPieces(otherColor, QUEEN);
    U64 bothRooks   = board.getPieces(color, ROOK) | board.getPieces(otherColor, ROOK);
    U64 bothKnights = board.getPieces(color, KNIGHT) | board.getPieces(otherColor, KNIGHT);

    if (!bothQueens && !bothRooks && !bothKnights){
          final_eval = final_eval * BOTH_SCALE_OCB / BOTH_SCALE_NORMAL;
          if (TRACK) ft.Scale = BOTH_SCALE_OCB;
        } else if (
        !bothQueens && !bothKnights &&
        _popCount(board.getPieces(color, ROOK)) == 1 && _popCount(board.getPieces(otherColor, ROOK)) == 1){
          final_eval = final_eval * BOTH_SCALE_ROOK_OCB / BOTH_SCALE_NORMAL;
          if (TRACK) ft.Scale = BOTH_SCALE_ROOK_OCB;
        } else if (
        !bothQueens && !bothRooks &&
        _popCount(board.getPieces(color, KNIGHT)) == 1 && _popCount(board.getPieces(otherColor, KNIGHT)) == 1){
          final_eval = final_eval * BOTH_SCALE_KNIGHT_OCB / BOTH_SCALE_NORMAL;
          if (TRACK) ft.Scale = BOTH_SCALE_KNIGHT_OCB;
        }
  }

  return final_eval;
}

int Eval::evaluate(const Board &board, Color color) {

  int score = 0;
  Color otherColor = getOppositeColor(color);

  if (IsItDeadDraw(board, color)){
    return 0;
  }

  if (TRACK){
    ft.MaterialValue[PAWN][color]+= _popCount(board.getPieces(color, PAWN));
    ft.MaterialValue[PAWN][otherColor]+= _popCount(board.getPieces(otherColor, PAWN));

    ft.MaterialValue[KNIGHT][color]+= _popCount(board.getPieces(color, KNIGHT));
    ft.MaterialValue[KNIGHT][otherColor]+= _popCount(board.getPieces(otherColor, KNIGHT));

    ft.MaterialValue[BISHOP][color]+= _popCount(board.getPieces(color, BISHOP));
    ft.MaterialValue[BISHOP][otherColor]+= _popCount(board.getPieces(otherColor, BISHOP));

    ft.MaterialValue[ROOK][color]+= _popCount(board.getPieces(color, ROOK));
    ft.MaterialValue[ROOK][otherColor]+= _popCount(board.getPieces(otherColor, ROOK));

    ft.MaterialValue[QUEEN][color]+= _popCount(board.getPieces(color, QUEEN));
    ft.MaterialValue[QUEEN][otherColor]+= _popCount(board.getPieces(otherColor, QUEEN));
  }

  // Piece square tables
  score += board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(otherColor);

  // Get PSQT-pawns Adjustments
  // 0 - own queen
  if (board.getPieces(color, QUEEN) != 0){
    score += board.getPSquareTable().getPawnAdjustment(color, 0) - board.getPSquareTable().getPawnAdjustment(otherColor, 1);
  }

  if (board.getPieces(otherColor, QUEEN) != 0){
    score += board.getPSquareTable().getPawnAdjustment(color, 1) - board.getPSquareTable().getPawnAdjustment(otherColor, 0);
  }

  // Create evalBits stuff
  evalBits eB = Eval::Setupbits(board);

  // Probe pawnHash, if not found, do full pawn evaluation
  score += probePawnStructure(board, color, &eB);

  // Evaluate pieces
  score +=  evaluateBISHOP(board, color, &eB) - evaluateBISHOP(board, otherColor, &eB)
          + evaluateKNIGHT(board, color, &eB) - evaluateKNIGHT(board, otherColor, &eB)
          + evaluateROOK  (board, color, &eB) - evaluateROOK  (board, otherColor, &eB)
          + evaluateQUEEN (board, color, &eB) - evaluateQUEEN (board, otherColor, &eB)
          + evaluateKING  (board, color, &eB)  - evaluateKING  (board, otherColor, &eB);

  // Interactions between pieces and pawns
  score +=  PiecePawnInteraction(board, color, &eB)
          - PiecePawnInteraction(board, otherColor, &eB);

  score +=  kingShieldSafety(board, color, &eB)
          - kingShieldSafety(board, otherColor, &eB);

  // Transform obtained safety score into game score
  score +=  kingDanger(color, &eB)
          - kingDanger(otherColor, &eB);

  // Taper and Scale obtained score
  int final_eval = TaperAndScale(board, color, score);

  return final_eval + TEMPO;
}

int Eval::evalTestSuite(const Board &board, Color color)
{
  return 0;
}
