#ifndef EVAL_H
#define EVAL_H

#include "defs.h"
#include "movegen.h"
#include "bitutils.h"

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
const int KING_HIGH_DANGER = gS(-44,-64);

const int KING_MED_DANGER = gS(-24,-39);

const int KING_LOW_DANGER = gS(75,-78);

const int KING_PAWN_TENSION = gS(-8,-1);

const int KING_BLOCKED_TENSION = gS(-22,-10);

const int BISHOP_PAIR_BONUS = gS(31,52);

const int DOUBLED_PAWN_PENALTY = gS(-9,-23);

const int ISOLATED_PAWN_PENALTY = gS(-5,-5);

const int PAWN_BLOCKED = gS(2,14);

const int PASSER_BLOCKED = gS(5,-2);

const int PAWN_PUSH_THREAT = gS(12,13);

const int BISHOP_RAMMED_PENALTY = gS(-2,-4);

const int BISHOP_CENTER_CONTROL = gS(9,9);

const int BISHOP_POS_PROUTPOST_JUMP = gS(10,0);

const int BISHOP_POS_GENOUTPOST_JUMP = gS(0,3);

const int KNIGHT_POS_PROUTPOST_JUMP = gS(8,7);

const int KNIGHT_POS_GENOUTPOST_JUMP = gS(7,8);

const int MINOR_BEHIND_PAWN = gS(4,7);

const int MINOR_BEHIND_PASSER = gS(8,-2);

const int MINOR_BLOCK_OWN_PAWN = gS(-7,0);

const int MINOR_BLOCK_OWN_PASSER = gS(0,-6);

const int ROOK_LINE_TENSION = gS(20,-4);

const int ROOK_RAMMED_LINE = gS(-8,-9);

const int KING_AHEAD_PASSER = gS(-11,14);

const int KING_EQUAL_PASSER = gS(11,7);

const int KING_BEHIND_PASSER = gS(0,-3);

const int KING_OPEN_FILE = gS(-59,10);

const int KING_OWN_SEMI_FILE = gS(-24,14);

const int KING_ENEMY_SEMI_LINE = gS(-18,8);

const int KING_ATTACK_PAWN = gS(16,45);

const int KING_PAWNLESS_FLANG = gS(-10,-60);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-12,-9), gS(-9,4), gS(-3,40),
           gS(16,52), gS(29,78), gS(49,87),
};

const int PASSED_PAWN_FILES[4] = {
           gS(1,19), gS(-3,15), gS(0,6), gS(-1,-3),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(0,9), gS(7,24), gS(66,22),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(9,27), gS(31,57), gS(68,106),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-2,-22), gS(17,1), gS(0,-18), gS(-7,-7),
           gS(-10,-8), gS(-19,-9), gS(-17,-13), gS(-15,-11),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(2,32), gS(-9,25), gS(-7,9),
           gS(-3,0), gS(0,0), gS(13,0), gS(11,0),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-49,-1), gS(8,-16), gS(0,2),
           gS(1,13), gS(-4,23), gS(0,24), gS(-8,21),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-4,-8), gS(1,2), gS(7,8), gS(13,12),
};

const int CANDIDATE_PASSED_PAWN[7] = {
           gS(0,0), gS(-18,0), gS(-11,7), gS(0,11),
           gS(3,30), gS(8,54), gS(0,0),
};

const int CANDIDATE_PASSED_PAWN_FILES[4] = {
           gS(9,16), gS(0,7), gS(0,0), gS(-6,-2),
};

const int BACKWARD_PAWN[7] = {
           gS(0,0), gS(0,-5), gS(1,-4), gS(-1,-7),
           gS(0,-4), gS(0,0), gS(0,0),
};

const int BACKWARD_OPEN_PAWN[7] = {
           gS(0,0), gS(-13,-11), gS(-6,-10), gS(-5,-9),
           gS(0,-6), gS(0,0), gS(0,0),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(28,-4), gS(34,-1),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(14,-4), gS(22,5),
};


/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-68,-40), gS(-54,-46), gS(-55,-66), gS(-50,-32),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(17,34), gS(15,35), gS(15,31),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-3,19), gS(65,20), gS(43,41),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(61,36), gS(39,21), gS(41,46),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-6,2), gS(6,9), gS(0,11), gS(0,4),
           gS(-8,5), gS(-21,0), gS(-5,17), gS(0,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(8,0), gS(14,20), gS(14,0), gS(24,0),
           gS(9,0), gS(-18,-1), gS(-4,16), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-5,-65), gS(3,-25), gS(10,-13), gS(15,-2), gS(17,6), gS(18,10), gS(17,13),
           gS(16,12), gS(15,13), gS(15,12), gS(17,9), gS(28,0), gS(30,10), gS(44,-9),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-2,-94), gS(9,-35), gS(16,-8), gS(20,1), gS(24,9),
           gS(27,13), gS(30,9), gS(33,3), gS(36,-5),
};

const int KING_MOBILITY[9] = {
           gS(14,-30), gS(5,-2), gS(0,5), gS(-2,9), gS(-5,6),
           gS(-8,4), gS(0,1), gS(0,-2), gS(13,-16),
};

const int ROOK_MOBILITY[15] = {
           gS(-28,-90), gS(-15,-52), gS(-8,-32), gS(0,-19), gS(2,-7), gS(0,5), gS(4,7),
           gS(6,7), gS(9,12), gS(11,16), gS(13,21), gS(15,23), gS(19,21), gS(25,21),
           gS(48,10),
};

const int QUEEN_MOBILITY[28] = {
           gS(-11,-114), gS(-6,-148), gS(-6,-111), gS(-5,-74), gS(-4,-54), gS(-4,-36), gS(-3,-18),
           gS(-1,-6), gS(0,6), gS(0,17), gS(0,24), gS(1,29), gS(1,34), gS(2,37),
           gS(2,39), gS(2,43), gS(1,46), gS(5,41), gS(6,39), gS(9,37), gS(13,30),
           gS(16,23), gS(9,25), gS(19,16), gS(0,8), gS(5,4), gS(8,3), gS(0,0),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(0,-85), gS(0,-21), gS(0,-5), gS(0,-5), gS(0,-17), gS(0,-21), gS(0,-11), gS(0,-94),
           gS(0,-12), gS(0,35), gS(5,34), gS(0,33), gS(1,16), gS(0,35), gS(0,31), gS(-4,-20),
           gS(0,0), gS(27,26), gS(7,38), gS(0,39), gS(0,40), gS(16,36), gS(11,29), gS(0,-1),
           gS(0,-12), gS(11,15), gS(-1,26), gS(-7,34), gS(-8,32), gS(0,25), gS(7,16), gS(-30,-15),
           gS(-10,-24), gS(0,2), gS(-4,15), gS(-14,22), gS(-14,21), gS(-21,16), gS(-1,1), gS(-38,-21),
           gS(-5,-30), gS(13,-6), gS(-11,6), gS(-30,16), gS(-25,15), gS(-21,7), gS(0,-6), gS(-6,-28),
           gS(6,-31), gS(0,-8), gS(-2,0), gS(-21,7), gS(-23,8), gS(-2,0), gS(-2,-8), gS(5,-34),
           gS(-12,-57), gS(8,-40), gS(-11,-20), gS(-35,-16), gS(10,-46), gS(-13,-26), gS(7,-37), gS(2,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(48,67), gS(40,73), gS(34,63), gS(65,48), gS(58,55), gS(38,60), gS(0,79), gS(0,67),
           gS(-1,30), gS(5,26), gS(23,8), gS(34,-2), gS(32,-1), gS(62,0), gS(19,19), gS(18,11),
           gS(-18,16), gS(-1,9), gS(0,0), gS(8,-2), gS(13,-3), gS(11,-1), gS(17,0), gS(-9,4),
           gS(-28,5), gS(-24,12), gS(-10,0), gS(5,-3), gS(4,-4), gS(5,-3), gS(-8,1), gS(-22,-4),
           gS(-28,-1), gS(-21,1), gS(-8,-3), gS(-5,0), gS(0,0), gS(0,-2), gS(-4,-6), gS(-23,-10),
           gS(-38,7), gS(-18,7), gS(-14,7), gS(0,7), gS(0,10), gS(0,6), gS(-5,1), gS(-29,-5),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-3,16), gS(-3,11), gS(0,8), gS(0,3), gS(0,1), gS(-1,1), gS(0,1), gS(0,7),
           gS(-7,12), gS(-8,19), gS(-12,8), gS(-2,0), gS(-2,2), gS(-10,6), gS(-6,21), gS(-8,14),
           gS(-3,-5), gS(-3,-3), gS(-11,-8), gS(0,-19), gS(0,-15), gS(-6,-4), gS(-5,-1), gS(-4,0),
           gS(0,-18), gS(-3,-9), gS(2,-14), gS(0,-25), gS(0,-24), gS(0,-6), gS(-5,0), gS(0,0),
           gS(0,-27), gS(-3,-3), gS(-7,-6), gS(-4,0), gS(-7,-9), gS(-8,0), gS(2,-6), gS(0,-28),
           gS(0,-45), gS(-1,-20), gS(-6,-6), gS(-14,0), gS(-9,0), gS(0,-2), gS(0,-9), gS(0,-50),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-6,8), gS(-6,7), gS(-1,2), gS(0,0), gS(0,0), gS(-2,0), gS(-1,0), gS(-3,0),
           gS(-5,11), gS(-9,13), gS(-9,13), gS(-3,0), gS(-2,1), gS(-11,1), gS(-2,21), gS(-7,8),
           gS(0,0), gS(0,0), gS(0,0), gS(1,-16), gS(0,-14), gS(0,-8), gS(-9,-4), gS(0,1),
           gS(0,15), gS(0,2), gS(0,-3), gS(0,-7), gS(0,-5), gS(0,-8), gS(0,0), gS(0,0),
           gS(2,4), gS(-1,6), gS(-1,3), gS(-5,5), gS(-2,7), gS(-2,10), gS(3,-2), gS(6,3),
           gS(3,33), gS(-1,17), gS(-3,25), gS(-2,20), gS(-4,16), gS(2,20), gS(5,14), gS(8,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,11), gS(16,20), gS(0,26), gS(0,19), gS(5,19), gS(10,20), gS(23,17), gS(36,14),
           gS(6,15), gS(0,15), gS(16,11), gS(35,3), gS(27,5), gS(25,4), gS(-8,15), gS(7,11),
           gS(1,17), gS(17,8), gS(18,9), gS(25,1), gS(40,-3), gS(32,-1), gS(24,1), gS(-6,13),
           gS(0,12), gS(2,13), gS(20,7), gS(21,1), gS(21,1), gS(22,0), gS(9,4), gS(0,3),
           gS(-14,7), gS(-9,12), gS(0,7), gS(0,6), gS(1,3), gS(-1,2), gS(-4,6), gS(-19,4),
           gS(-16,-1), gS(-9,0), gS(-4,0), gS(-3,-3), gS(-2,-2), gS(0,-9), gS(1,-8), gS(-12,-11),
           gS(-21,-9), gS(-5,-11), gS(0,-8), gS(0,-11), gS(11,-16), gS(7,-22), gS(4,-23), gS(-39,-11),
           gS(-5,-13), gS(0,-14), gS(1,-10), gS(10,-21), gS(13,-24), gS(12,-23), gS(-9,-11), gS(-3,-26),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-36,-1), gS(-43,-2), gS(-74,16), gS(-81,16), gS(-53,15), gS(-91,13), gS(-27,-5), gS(-28,-17),
           gS(-37,-2), gS(-19,-1), gS(-14,0), gS(-12,9), gS(-13,6), gS(-17,3), gS(-26,-6), gS(-25,-7),
           gS(-5,0), gS(0,0), gS(-2,3), gS(20,-2), gS(12,1), gS(18,-1), gS(4,0), gS(5,-3),
           gS(-7,-1), gS(15,5), gS(4,3), gS(22,12), gS(18,12), gS(13,5), gS(13,2), gS(-1,-7),
           gS(4,-5), gS(-4,4), gS(7,7), gS(20,8), gS(23,6), gS(6,5), gS(7,1), gS(5,-14),
           gS(2,-14), gS(19,-4), gS(10,-2), gS(10,6), gS(6,11), gS(21,-7), gS(17,-10), gS(7,-11),
           gS(14,-19), gS(11,-24), gS(15,-10), gS(3,0), gS(13,0), gS(15,-6), gS(20,-22), gS(7,-29),
           gS(-3,-32), gS(6,-5), gS(-2,-3), gS(1,0), gS(6,-4), gS(-12,4), gS(0,-8), gS(-1,-24),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-165,-55), gS(-31,-3), gS(-82,8), gS(-13,-3), gS(0,-1), gS(-83,2), gS(-29,-4), gS(-91,-64),
           gS(-35,-10), gS(-21,0), gS(-4,0), gS(19,6), gS(8,3), gS(25,-12), gS(-13,-2), gS(-22,-16),
           gS(-11,-1), gS(3,3), gS(10,20), gS(27,6), gS(41,1), gS(45,5), gS(9,-1), gS(13,-13),
           gS(6,-1), gS(13,1), gS(20,15), gS(36,19), gS(24,22), gS(40,14), gS(16,3), gS(23,-10),
           gS(0,0), gS(14,5), gS(20,19), gS(25,17), gS(25,21), gS(31,13), gS(25,1), gS(11,-1),
           gS(-8,-22), gS(9,-8), gS(9,4), gS(20,15), gS(23,13), gS(21,1), gS(25,-10), gS(2,-12),
           gS(-7,-27), gS(-12,1), gS(2,-6), gS(5,6), gS(11,4), gS(12,-5), gS(0,0), gS(1,-15),
           gS(-40,-30), gS(-15,-17), gS(-1,-5), gS(0,1), gS(7,3), gS(-3,-5), gS(-9,-8), gS(-18,-29),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-14,0), gS(0,10), gS(0,7), gS(0,16), gS(-8,24), gS(0,10), gS(-10,17), gS(-13,15),
           gS(-14,2), gS(-28,22), gS(-13,25), gS(-26,44), gS(-40,62), gS(-21,30), gS(-13,27), gS(1,10),
           gS(-10,-15), gS(-1,0), gS(-4,23), gS(0,35), gS(0,43), gS(4,31), gS(7,12), gS(-5,7),
           gS(-3,-16), gS(-4,9), gS(-3,19), gS(-1,40), gS(-5,53), gS(0,33), gS(0,25), gS(5,-4),
           gS(0,-26), gS(-6,0), gS(-1,11), gS(-2,38), gS(3,34), gS(-3,10), gS(8,4), gS(-5,-3),
           gS(-4,-31), gS(9,-23), gS(5,-5), gS(3,0), gS(3,0), gS(7,-4), gS(13,-11), gS(3,-26),
           gS(3,-52), gS(10,-57), gS(18,-57), gS(10,-33), gS(13,-29), gS(20,-64), gS(24,-79), gS(0,-44),
           gS(4,-62), gS(6,-68), gS(4,-75), gS(3,-58), gS(6,-63), gS(-4,-75), gS(0,-62), gS(13,-69),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(90,112), gS(515,682), gS(359,377), gS(380,395), gS(1004,1254),
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
int evaluate(const Board &, Color);

/**
 * @brief Basically template function for testing various eval features.
 *  As I use it, it calls once before search.
 *
 */
int evalTestSuite(const Board &, Color);

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
 * @brief This function takes number of each pieceType count for each
 * side and (assuming best play) returns if the position is deadDraw
 *
 * Returns true is position is drawn, returns false if there is some play left.
 * Based on Vice function.
 *
 */
inline bool IsItDeadDraw (const Board &, Color);


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
