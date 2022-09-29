#ifndef EVAL_H
#define EVAL_H

#include "defs.h"
#include "movegen.h"
#include "bitutils.h"
#include "endgame.h"

#define gS(opS, egS) (int)((unsigned int)(opS) << 16) + (egS)
#define opS(gS) (int16_t)((uint16_t)((unsigned)((gS) + 0x8000) >> 16))
#define egS(gS) (int16_t)((uint16_t)((unsigned)((gS))))

struct evalBits{
    U64 EnemyPawnAttackMap[2];
    U64 PossibleProtOutposts[2];
    U64 PossibleGenOutposts[2];
    U64 EnemyKingZone[2];
    U64 Passers[2];
    U64 AttackedSquares[2];
    U64 AttackedByKing[2];
    int RammedCount;
    int KingAttackers[2];
    int KingAttackPower[2];
};

enum CastleSide {
    KingSide,
    QueenSide,
    NoCastle
};

/**
 * @brief Namespace containing board evaluation functions
 */
namespace Eval {
namespace detail {
/**
 * @brief Array of files A-H as bitboards
 */
extern U64 FILES[8];

/**
 * @brief Array of the files neighboring each file as bitboards
 */
extern U64 CONNECTED_MASK[64];
extern U64 NEIGHBOR_FILES[8];
extern U64 TWO_PLUS_FILES[8];
extern U64 OUTPOST_MASK[2][64];
extern U64 OUTPOST_PROTECTION[2][64];
extern U64 KINGZONE[2][64];
extern U64 PAWN_DUOS [64];
extern U64 DISTANCE[64][64];
extern U64 FORWARD_BITS[2][64];

/**
 * @brief Array of masks indexed by [Color][square] containing all squares that
 * must be free of enemy pawns for a pawn of the given color on the given
 * square to be considered passed
 */
extern U64 PASSED_PAWN_MASKS[2][64];

/**
 * @brief Array of masks indexed by [Color][sideTo_OO][mask_NUM]
 * 0 - kindgside castle masks
 * 1 - queenside castle masks
 *
 * maskNUMs - just all masks
 */
extern U64 KING_PAWN_MASKS[2][2][8];
};

/**
 * @brief Various constants used for Scaling evaluation
 * The basic idea is that in some endgames it is hard to convert even a big advantage
 * We want our engine to avoid such endgames, so we scale down the evaluation if the endgame
 * is hard to win (OCB and its derivates, no pany pawns, etc)
 *
  * @{
  */
  const int EG_SCALE_NORMAL = 64;
  const int EG_SCALE_MAXIMUM = 128;
  const int EG_SCALE_MINIMAL = 32;
  const int EG_SCALE_PAWN    = 8;

  const int BOTH_SCALE_NORMAL = 4;
  const int BOTH_SCALE_OCB = 2;
  const int BOTH_SCALE_ROOK_OCB = 3;
  const int BOTH_SCALE_KNIGHT_OCB = 3;
  const int BOTH_SCALE_ZERO = 0;
  /**@}*/

/**
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int START_ATTACK_VALUE = -50;
  const int ATTACK_TEMPO = 35;
  const int QUEEN_FACE_CHECK = 80;

  const int UNCONTESTED_KING_ATTACK [6] = {
      -70, -20, 0, 100, 150, 200
  };

  const int PIECE_ATTACK_POWER[6] = {
        0, 24, 50, 26, 47, 0
  };

  const int PIECE_CHECK_POWER[6] = {
        0, 22, 16, 92, 26, 0
  };

  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int COUNT_TO_POWER_DIVISOR = 128;
  /**@}*/

const int TEMPO = 10;

const int KING_HIGH_DANGER = gS(-42,-54);

const int KING_MED_DANGER = gS(-29,-23);

const int KING_LOW_DANGER = gS(52,-76);

const int BISHOP_PAIR_BONUS = gS(22,62);

const int DOUBLED_PAWN_PENALTY = gS(-4,-23);

const int ISOLATED_PAWN_PENALTY = gS(-3,-7);

const int PAWN_BLOCKED = gS(3,16);

const int PASSER_BLOCKED = gS(6,-1);

const int PAWN_PUSH_THREAT = gS(17,15);

const int BISHOP_RAMMED_PENALTY = gS(-4,-3);

const int BISHOP_CENTER_CONTROL = gS(9,7);

const int BISHOP_POS_PROUTPOST_JUMP = gS(8,0);

const int BISHOP_POS_GENOUTPOST_JUMP = gS(3,5);

const int KNIGHT_POS_PROUTPOST_JUMP = gS(10,8);

const int KNIGHT_POS_GENOUTPOST_JUMP = gS(8,12);

const int MINOR_BEHIND_PAWN = gS(6,10);

const int MINOR_BEHIND_PASSER = gS(8,-3);

const int MINOR_BLOCK_OWN_PAWN = gS(-4,1);

const int MINOR_BLOCK_OWN_PASSER = gS(-3,3);

const int ROOK_LINE_TENSION = gS(15,-5);

const int ROOK_RAMMED_LINE = gS(-8,-12);

const int KING_AHEAD_PASSER = gS(-12,17);

const int KING_EQUAL_PASSER = gS(14,10);

const int KING_BEHIND_PASSER = gS(0,-6);

const int KING_OPEN_FILE = gS(-78,14);

const int KING_OWN_SEMI_FILE = gS(-28,17);

const int KING_ENEMY_SEMI_LINE = gS(-20,9);

const int KING_ATTACK_PAWN = gS(8,38);

const int KING_PAWNLESS_FLANG = gS(-10,-65);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-9,-9), gS(-6,4), gS(-2,39),
           gS(26,50), gS(21,72), gS(35,95),
};

const int PASSED_PAWN_FILES[4] = {
           gS(-2,20), gS(-3,16), gS(0,5), gS(2,-4),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-3,12), gS(-5,36), gS(54,27),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(0,34), gS(11,87), gS(58,115),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-3,-25), gS(14,-1), gS(-2,-17), gS(-6,-9),
           gS(-9,-6), gS(-19,-10), gS(-18,-15), gS(-17,-18),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(4,34), gS(-9,27), gS(-7,11),
           gS(-3,1), gS(2,-2), gS(14,-3), gS(9,-2),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-49,1), gS(20,-20), gS(0,1),
           gS(0,13), gS(-3,22), gS(-4,25), gS(-14,23),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-7,-13), gS(2,1), gS(5,11), gS(10,22),
};

const int CANDIDATE_PASSED_PAWN[7] = {
           gS(0,0), gS(-26,-5), gS(-17,6), gS(-4,18),
           gS(18,40), gS(27,63), gS(0,0),
};

const int CANDIDATE_PASSED_PAWN_FILES[4] = {
           gS(7,23), gS(3,15), gS(0,7), gS(4,0),
};

const int BACKWARD_PAWN[7] = {
           gS(0,0), gS(-7,-1), gS(3,-4), gS(-1,-8),
           gS(1,-2), gS(0,0), gS(0,0),
};

const int BACKWARD_OPEN_PAWN[7] = {
           gS(0,0), gS(-19,-17), gS(-12,-14), gS(-14,-5),
           gS(2,0), gS(0,0), gS(0,0),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(22,4), gS(21,-3),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(9,0), gS(14,0),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-3,11), gS(-63,-26), gS(-52,-26), gS(-54,-53), gS(-48,-26),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(15,26), gS(16,29), gS(14,23),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-2,19), gS(55,5), gS(37,21),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(52,25), gS(33,12), gS(48,45),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-8,-5), gS(5,6), gS(0,15), gS(-6,0),
           gS(-6,1), gS(-16,-8), gS(-3,15), gS(-6,-2),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(16,-1), gS(20,21), gS(17,0), gS(24,0),
           gS(9,0), gS(-6,-3), gS(-1,15), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-13,-56), gS(-6,-28), gS(1,-13), gS(6,-3), gS(10,2), gS(12,8), gS(13,8),
           gS(13,8), gS(11,11), gS(13,6), gS(19,4), gS(25,-1), gS(29,10), gS(43,-11),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-14,-89), gS(-1,-26), gS(7,-5), gS(12,0), gS(16,4),
           gS(20,9), gS(26,5), gS(30,0), gS(33,-8),
};

const int KING_MOBILITY[9] = {
           gS(16,-24), gS(15,-1), gS(7,1), gS(0,6), gS(-6,5),
           gS(-13,4), gS(-7,3), gS(-11,0), gS(13,-16),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-71), gS(-11,-37), gS(-4,-27), gS(1,-18), gS(0,-10), gS(-5,1), gS(0,3),
           gS(3,3), gS(5,7), gS(9,10), gS(9,14), gS(10,18), gS(15,19), gS(19,18),
           gS(34,10),
};

const int QUEEN_MOBILITY[28] = {
           gS(-11,-114), gS(-5,-150), gS(-11,-109), gS(-10,-71), gS(-9,-52), gS(-7,-36), gS(-6,-19),
           gS(-5,-6), gS(-3,2), gS(-2,8), gS(-2,16), gS(0,22), gS(0,24), gS(-1,30),
           gS(0,33), gS(0,37), gS(0,44), gS(1,42), gS(7,43), gS(13,41), gS(17,37),
           gS(25,34), gS(14,32), gS(22,20), gS(3,13), gS(6,6), gS(8,5), gS(0,2),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(0,-84), gS(0,-25), gS(0,-8), gS(0,-5), gS(0,-17), gS(0,-19), gS(0,-9), gS(0,-92),
           gS(0,-14), gS(0,30), gS(3,30), gS(0,31), gS(1,18), gS(0,36), gS(0,32), gS(-1,-13),
           gS(0,-3), gS(27,25), gS(7,35), gS(0,39), gS(0,42), gS(17,38), gS(11,31), gS(0,0),
           gS(-1,-17), gS(8,11), gS(-2,25), gS(-9,33), gS(-11,31), gS(-2,26), gS(4,14), gS(-30,-11),
           gS(-11,-28), gS(-2,0), gS(-6,15), gS(-18,28), gS(-19,25), gS(-25,14), gS(-7,-1), gS(-41,-18),
           gS(-5,-30), gS(11,-6), gS(-13,6), gS(-30,19), gS(-23,16), gS(-24,6), gS(0,-7), gS(-9,-24),
           gS(9,-31), gS(0,-6), gS(8,0), gS(-13,6), gS(-14,8), gS(4,-1), gS(7,-11), gS(1,-31),
           gS(-13,-59), gS(15,-43), gS(-11,-16), gS(-36,-15), gS(14,-35), gS(-14,-20), gS(8,-40), gS(0,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(46,69), gS(36,73), gS(35,71), gS(65,50), gS(55,53), gS(39,58), gS(-1,76), gS(-3,68),
           gS(2,33), gS(8,34), gS(27,13), gS(32,-2), gS(32,-5), gS(73,0), gS(30,24), gS(26,19),
           gS(-16,21), gS(0,9), gS(1,1), gS(2,-8), gS(8,-8), gS(15,-4), gS(14,0), gS(-2,0),
           gS(-28,8), gS(-19,10), gS(-13,0), gS(0,-6), gS(3,-6), gS(0,-5), gS(-8,0), gS(-18,-7),
           gS(-31,2), gS(-19,2), gS(-11,-2), gS(-7,0), gS(1,-2), gS(-4,-5), gS(-5,-7), gS(-23,-11),
           gS(-36,10), gS(-16,10), gS(-11,8), gS(1,3), gS(0,11), gS(4,2), gS(-6,0), gS(-35,-5),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-6,12), gS(-5,5), gS(-1,7), gS(0,4), gS(-1,0), gS(0,0), gS(0,0), gS(-3,4),
           gS(-2,11), gS(-5,18), gS(-7,8), gS(0,3), gS(0,3), gS(1,6), gS(2,19), gS(-2,15),
           gS(-2,-9), gS(-1,-12), gS(-9,-13), gS(-1,-22), gS(5,-22), gS(0,-13), gS(0,-1), gS(0,-6),
           gS(3,-24), gS(0,-13), gS(4,-16), gS(6,-20), gS(3,-26), gS(3,-12), gS(0,-5), gS(7,-6),
           gS(1,-26), gS(-1,-8), gS(-3,-3), gS(-4,-1), gS(-8,-7), gS(-5,6), gS(1,4), gS(3,-25),
           gS(10,-44), gS(1,-22), gS(-2,-9), gS(-13,-3), gS(-11,3), gS(0,9), gS(4,0), gS(17,-48),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-9,4), gS(-8,4), gS(-2,1), gS(-1,0), gS(-2,-1), gS(0,0), gS(-1,0), gS(-5,0),
           gS(0,13), gS(-6,12), gS(-5,10), gS(-1,2), gS(0,2), gS(1,1), gS(5,20), gS(0,13),
           gS(1,0), gS(0,-7), gS(-1,-5), gS(-1,-17), gS(4,-20), gS(4,-15), gS(-6,-4), gS(5,-1),
           gS(3,12), gS(1,-5), gS(0,-7), gS(-4,-7), gS(0,-9), gS(2,-8), gS(4,-3), gS(5,-5),
           gS(2,11), gS(-2,0), gS(-5,0), gS(-8,3), gS(-6,6), gS(-6,10), gS(0,1), gS(5,2),
           gS(3,18), gS(-2,9), gS(-6,14), gS(-3,14), gS(-7,18), gS(-9,27), gS(3,12), gS(7,5),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(21,10), gS(10,15), gS(-1,25), gS(-3,18), gS(1,14), gS(7,16), gS(21,15), gS(33,13),
           gS(-2,9), gS(-4,16), gS(11,16), gS(29,3), gS(19,2), gS(22,3), gS(-1,13), gS(11,6),
           gS(-6,9), gS(14,5), gS(10,6), gS(14,0), gS(33,-5), gS(27,-7), gS(29,0), gS(0,2),
           gS(-4,11), gS(2,9), gS(12,8), gS(13,2), gS(14,-1), gS(20,-7), gS(13,2), gS(2,-1),
           gS(-14,7), gS(-12,9), gS(-2,6), gS(1,6), gS(2,2), gS(-5,0), gS(1,2), gS(-14,0),
           gS(-15,1), gS(-9,0), gS(-3,-1), gS(-1,0), gS(3,-2), gS(5,-11), gS(19,-12), gS(-2,-13),
           gS(-17,-6), gS(-7,-6), gS(2,-2), gS(4,-4), gS(11,-12), gS(6,-17), gS(17,-20), gS(-24,-9),
           gS(-5,-4), gS(-2,-7), gS(-1,0), gS(5,-6), gS(12,-13), gS(0,-7), gS(4,-14), gS(-6,-19),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-36,-1), gS(-43,-1), gS(-73,13), gS(-82,12), gS(-56,10), gS(-91,9), gS(-26,-5), gS(-31,-18),
           gS(-38,-5), gS(-23,-5), gS(-15,-2), gS(-13,6), gS(-13,1), gS(-18,-1), gS(-31,-7), gS(-30,-12),
           gS(-4,3), gS(-1,0), gS(-6,0), gS(11,-3), gS(6,-1), gS(20,0), gS(3,-2), gS(11,0),
           gS(-10,0), gS(15,6), gS(4,4), gS(16,15), gS(19,7), gS(17,4), gS(21,0), gS(-7,-4),
           gS(-3,-4), gS(-7,5), gS(3,7), gS(20,7), gS(20,3), gS(7,3), gS(3,1), gS(11,-15),
           gS(-2,-12), gS(15,-1), gS(8,0), gS(8,7), gS(10,11), gS(15,0), gS(19,-5), gS(13,-11),
           gS(6,-21), gS(6,-24), gS(14,-13), gS(1,2), gS(10,1), gS(13,-3), gS(21,-17), gS(7,-28),
           gS(-3,-30), gS(14,-11), gS(0,0), gS(0,0), gS(10,-1), gS(-11,8), gS(0,-9), gS(0,-27),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-162,-52), gS(-33,-6), gS(-80,6), gS(-14,-6), gS(-1,-3), gS(-82,0), gS(-30,-6), gS(-92,-65),
           gS(-36,-11), gS(-23,-1), gS(-6,0), gS(13,0), gS(3,-1), gS(26,-13), gS(-12,-3), gS(-21,-18),
           gS(-11,-3), gS(0,0), gS(12,20), gS(21,7), gS(37,0), gS(46,0), gS(8,-4), gS(8,-16),
           gS(1,-1), gS(15,3), gS(28,15), gS(43,18), gS(31,17), gS(57,13), gS(22,4), gS(24,-10),
           gS(-6,0), gS(9,3), gS(20,20), gS(26,18), gS(30,23), gS(32,11), gS(27,2), gS(8,0),
           gS(-17,-17), gS(1,-3), gS(9,7), gS(11,15), gS(23,14), gS(22,2), gS(23,-7), gS(6,-7),
           gS(-19,-25), gS(-16,-2), gS(-3,-1), gS(8,4), gS(10,2), gS(7,-1), gS(-2,-3), gS(-3,-7),
           gS(-41,-28), gS(-18,-7), gS(-11,-6), gS(-2,1), gS(6,4), gS(-3,-2), gS(-14,-3), gS(-20,-27),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-22,-5), gS(-6,3), gS(-1,7), gS(0,18), gS(-9,25), gS(-1,8), gS(-11,13), gS(-16,10),
           gS(-17,-4), gS(-30,15), gS(-18,21), gS(-29,42), gS(-43,63), gS(-24,27), gS(-15,24), gS(8,11),
           gS(-8,-13), gS(-6,-4), gS(-6,22), gS(-5,34), gS(-6,40), gS(4,30), gS(8,9), gS(3,6),
           gS(-7,-15), gS(0,6), gS(-4,17), gS(-10,36), gS(-8,49), gS(2,31), gS(9,24), gS(8,-2),
           gS(1,-27), gS(-6,0), gS(-3,7), gS(-2,35), gS(0,31), gS(-1,11), gS(12,2), gS(1,0),
           gS(-3,-33), gS(8,-24), gS(2,-4), gS(0,2), gS(4,3), gS(9,-4), gS(22,-12), gS(7,-25),
           gS(2,-52), gS(8,-54), gS(13,-45), gS(13,-28), gS(14,-26), gS(16,-57), gS(27,-75), gS(1,-45),
           gS(0,-61), gS(0,-64), gS(4,-65), gS(5,-48), gS(7,-56), gS(0,-69), gS(1,-60), gS(11,-69),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(81,110), gS(480,686), gS(336,374), gS(362,385), gS(977,1255),
};

/**
 * @brief Initializes all inner constants used by functions in the Eval namespace
 */
void init();

/**
 * @brief Returns the evaluated advantage of the given color in centipawns
 *
 * @param board Board to evaluate
 * @param color Color to evaluate advantage of
 * @return Advantage of the given color in centipawns
 */
inline int evaluateMain(const Board &, Color);

int evaluate(const Board &, Color);


void initEG();

inline void egHashAdd(std::string , egEvalFunction, egEntryType);

int evaluateDraw(const Board &, Color);
int evaluateRookMinor_Rook(const Board &, Color);
int evaluateQueen_vs_X(const Board &, Color);
int evaluateQueen_vs_Pawn(const Board &, Color);
int evaluateRook_vs_Bishop(const Board &, Color);
int evaluateRook_vs_Knight(const Board &, Color);
int evaluateRook_vs_Pawn(const Board &, Color);
int evaluateBishopPawn_vs_Bishop(const Board &, Color);
int evaluateBishopPawn_vs_Knight(const Board &, Color);
int evaluateQueen_vs_RookPawn(const Board &, Color);
int evaluateRookPawn_vs_Bishop(const Board &, Color);
int evaluateHugeAdvantage(const Board &, Color);
int evaluateBN_Mating(const Board &, Color);
int evaluateKnights_vs_Pawn(const Board &, Color);
int evaluateRookPawn_vs_Rook(const Board &, Color);
int evaluateKingPawn_vs_King(const Board &, Color);
int evaluateBishopPawn_vs_KP(const Board &, Color);

/**
 * @brief Returns the value of the given PieceType used for evaluation
 * purposes in centipawns
 *
 * This method returns the material value assuming the game is in the opening
 * and can be used to quickly determine relative capture values for move
 * ordering purposes.
 *
 * @param pieceType PieceType to get value of
 * @return The value of the given PieceType used for evaluation purposes
 */
int getMaterialValue(int, PieceType);

  /**
    * @name Functions used for the evaluating positions of the Major Pieces
    * @brief Returns structure that contain opening and enggame scores
    * @{
    */
   inline int evaluateQUEEN(const Board &, Color, evalBits *);
   inline int evaluateROOK(const Board &, Color, evalBits *);
   inline int evaluateBISHOP(const Board &, Color, evalBits *);
   inline int evaluateKNIGHT(const Board &, Color, evalBits *);
   inline int evaluatePAWNS(const Board &, Color, evalBits *);
   inline int evaluateKING(const Board &, Color, evalBits *);
   inline int probePawnStructure(const Board &, Color, evalBits *);

  /**@}*/

evalBits Setupbits(const Board &);

/**
 * @brief This function analyses king shield safety.
 * it returns simple overall score gS() and
 * adjust base safety value for some types of shields
 */
inline int kingShieldSafety(const Board &, Color, evalBits *);

/**
 * @brief Function evaluate piece-pawns interactions for given color
 * Includes:
 * 1. Blocked Pawns
 * 2. Minors shielded by pawns
 * 3. Threats by pawn push
 */
inline int PiecePawnInteraction(const Board &, Color, evalBits *);

/**
 * @brief Taper evaluation between Opening and Endgame and scale it
 * if there is some specific endgame position
 */
inline int TaperAndScale(const Board &, Color, int);

/**
 * @brief Transform danger score accumulated in other functions in
 *        a score used for an evaluation
 */
inline int kingDanger(Color, const evalBits *);

/**
 * @brief Set value for a MATERIAL_VALUES_TUNABLE array
 * which is used for optuna tuning
 */
void SetupTuning(PieceType piece, int value);

};

#endif
