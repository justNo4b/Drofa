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
    U64 OutPostedLines[2];
    U64 PossibleProtOutposts[2];
    U64 PossibleGenOutposts[2];
    U64 EnemyKingZone[2];
    U64 Passers[2];
    U64 AttackedSquares[2];
    U64 AttackedByPieces[2][6];
    int RammedCount;
    int KingAttackers[2];
    int KingAttackPower[2];
    int EnemyKingSquare[2];
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

  const int UNCONTESTED_KING_ATTACK [6] = {
      -70, -20, 0, 100, 150, 200
  };

  const int PIECE_ATTACK_POWER[6] = {
        0, 24, 50, 26, 62, 0
  };

  const int PIECE_CHECK_POWER[6] = {
        0, 22, 16, 92, 34, 0
  };

  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int COUNT_TO_POWER_DIVISOR = 128;
  /**@}*/

const int TEMPO = 10;

const int KING_HIGH_DANGER = gS(-50,-65);

const int KING_MED_DANGER = gS(-24,-40);

const int KING_LOW_DANGER = gS(74,-78);

const int BISHOP_PAIR_BONUS = gS(31,52);

const int DOUBLED_PAWN_PENALTY = gS(-8,-23);

const int ISOLATED_PAWN_PENALTY = gS(-6,-3);

const int PAWN_BLOCKED = gS(3,14);

const int PASSER_BLOCKED = gS(7,-3);

const int BISHOP_RAMMED_PENALTY = gS(-2,-4);

const int BISHOP_CENTER_CONTROL = gS(9,8);

const int BISHOP_POS_PROUTPOST_JUMP = gS(10,0);

const int BISHOP_POS_GENOUTPOST_JUMP = gS(0,3);

const int KNIGHT_POS_PROUTPOST_JUMP = gS(8,7);

const int KNIGHT_POS_GENOUTPOST_JUMP = gS(6,9);

const int MINOR_BEHIND_PAWN = gS(4,8);

const int MINOR_BEHIND_PASSER = gS(8,-2);

const int MINOR_BLOCK_OWN_PAWN = gS(-8,-1);

const int MINOR_BLOCK_OWN_PASSER = gS(0,-5);

const int KING_AHEAD_PASSER = gS(-11,14);

const int KING_EQUAL_PASSER = gS(12,7);

const int KING_BEHIND_PASSER = gS(0,-4);

const int KING_OPEN_FILE = gS(-58,8);

const int KING_OWN_SEMI_FILE = gS(-24,15);

const int KING_ENEMY_SEMI_LINE = gS(-17,8);

const int KING_ATTACK_PAWN = gS(19,47);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-9,-9), gS(-7,5), gS(-1,41),
           gS(15,53), gS(27,78), gS(53,87),
};

const int PASSED_PAWN_FILES[4] = {
           gS(0,18), gS(-2,13), gS(0,5), gS(0,-4),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(0,8), gS(7,23), gS(65,23),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(13,28), gS(36,60), gS(58,109),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-4,-21), gS(17,4), gS(0,-19), gS(-7,-7),
           gS(-10,-8), gS(-19,-9), gS(-17,-13), gS(-15,-11),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(3,34), gS(-9,27), gS(-6,9),
           gS(-3,0), gS(0,0), gS(14,-1), gS(13,-2),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-47,-3), gS(8,-17), gS(0,0),
           gS(1,12), gS(-3,22), gS(0,23), gS(-8,21),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-5,-9), gS(1,2), gS(7,8), gS(13,12),
};

const int CANDIDATE_PASSED_PAWN[7] = {
           gS(0,0), gS(-16,2), gS(-9,8), gS(1,14),
           gS(5,31), gS(16,39), gS(0,0),
};

const int CANDIDATE_PASSED_PAWN_FILES[4] = {
           gS(14,9), gS(-3,6), gS(0,0), gS(-7,-3),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(29,-3), gS(34,0),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(14,-1), gS(23,9),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,18), gS(-66,-41), gS(-50,-47), gS(-54,-66), gS(-49,-30),
};

const int KNIGHT_KING_FORK_POS[5] = {
          gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0)
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(17,33), gS(15,35), gS(15,31),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-2,19), gS(64,21), gS(43,42),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(62,35), gS(39,19), gS(42,38),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-7,2), gS(6,10), gS(0,12), gS(0,6),
           gS(-8,5), gS(-21,0), gS(-5,16), gS(0,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(7,0), gS(14,23), gS(15,2), gS(25,7),
           gS(10,0), gS(-18,-1), gS(-3,14), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-6,-62), gS(2,-23), gS(9,-11), gS(14,-1), gS(17,7), gS(18,9), gS(17,11),
           gS(16,11), gS(15,12), gS(15,10), gS(16,8), gS(28,0), gS(30,10), gS(54,-11),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-1,-93), gS(10,-33), gS(17,-6), gS(22,3), gS(26,10),
           gS(29,14), gS(32,10), gS(36,4), gS(39,-5),
};

const int KING_MOBILITY[9] = {
           gS(14,-29), gS(5,-5), gS(0,4), gS(-2,10), gS(-5,8),
           gS(-9,6), gS(0,5), gS(0,0), gS(15,-18),
};

const int ROOK_MOBILITY[15] = {
           gS(-27,-85), gS(-13,-48), gS(-6,-30), gS(1,-18), gS(3,-6), gS(1,6), gS(6,7),
           gS(8,7), gS(11,12), gS(14,15), gS(15,20), gS(18,22), gS(22,20), gS(28,19),
           gS(50,9),
};

const int QUEEN_MOBILITY[28] = {
           gS(-10,-114), gS(-6,-158), gS(-6,-111), gS(-5,-72), gS(-4,-51), gS(-3,-33), gS(-2,-16),
           gS(-1,-3), gS(0,8), gS(0,19), gS(1,26), gS(2,31), gS(2,36), gS(2,38),
           gS(2,41), gS(2,43), gS(1,46), gS(5,41), gS(6,39), gS(10,37), gS(13,30),
           gS(16,23), gS(9,25), gS(19,16), gS(0,12), gS(5,8), gS(10,13), gS(8,10),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(0,-93), gS(0,-31), gS(0,-14), gS(0,-7), gS(0,-19), gS(0,-22), gS(0,-13), gS(0,-94),
           gS(0,-16), gS(0,33), gS(5,33), gS(0,32), gS(1,15), gS(0,34), gS(0,31), gS(-4,-22),
           gS(0,-2), gS(27,24), gS(17,35), gS(0,38), gS(1,39), gS(22,35), gS(11,28), gS(0,-3),
           gS(-1,-13), gS(11,13), gS(0,25), gS(-6,35), gS(-7,33), gS(5,24), gS(10,13), gS(-35,-15),
           gS(-13,-24), gS(2,0), gS(-3,16), gS(-15,25), gS(-14,24), gS(-21,18), gS(-1,1), gS(-44,-18),
           gS(-16,-27), gS(11,-6), gS(-12,8), gS(-30,19), gS(-25,18), gS(-22,9), gS(0,-5), gS(-8,-26),
           gS(2,-29), gS(-3,-7), gS(-3,1), gS(-22,10), gS(-24,10), gS(-3,1), gS(-6,-6), gS(2,-31),
           gS(-16,-55), gS(4,-39), gS(-15,-18), gS(-34,-15), gS(11,-47), gS(-13,-26), gS(3,-36), gS(-1,-67),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(53,69), gS(40,78), gS(37,66), gS(65,53), gS(63,58), gS(48,63), gS(0,84), gS(0,72),
           gS(1,31), gS(9,30), gS(25,12), gS(42,0), gS(43,0), gS(70,5), gS(26,23), gS(26,13),
           gS(-10,16), gS(2,11), gS(7,1), gS(15,0), gS(21,-1), gS(20,-1), gS(25,1), gS(-2,5),
           gS(-22,4), gS(-20,12), gS(-7,0), gS(11,-3), gS(10,-4), gS(10,-3), gS(-3,2), gS(-16,-5),
           gS(-23,-2), gS(-17,0), gS(-6,-4), gS(-4,0), gS(1,-1), gS(1,-2), gS(-1,-7), gS(-18,-10),
           gS(-34,6), gS(-15,6), gS(-14,7), gS(0,6), gS(0,9), gS(1,5), gS(-3,0), gS(-25,-6),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-2,14), gS(-2,9), gS(0,6), gS(0,2), gS(0,1), gS(-1,1), gS(0,1), gS(0,6),
           gS(-4,10), gS(-6,17), gS(-8,7), gS(-2,0), gS(-2,2), gS(-6,4), gS(-4,20), gS(-4,13),
           gS(-2,-4), gS(-3,-1), gS(-10,-8), gS(0,-19), gS(-1,-13), gS(-6,-4), gS(-7,0), gS(-3,0),
           gS(0,-16), gS(-3,-9), gS(1,-13), gS(0,-25), gS(0,-22), gS(0,-6), gS(-6,-1), gS(0,0),
           gS(0,-27), gS(-3,-5), gS(-6,-9), gS(-5,0), gS(-6,-8), gS(-7,1), gS(2,-5), gS(0,-27),
           gS(0,-41), gS(-1,-20), gS(-5,-7), gS(-11,0), gS(-7,0), gS(1,-2), gS(1,-9), gS(0,-47),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-5,6), gS(-4,6), gS(-1,1), gS(0,0), gS(0,0), gS(-2,0), gS(0,0), gS(-1,0),
           gS(-3,9), gS(-7,11), gS(-8,11), gS(-2,0), gS(-2,1), gS(-6,1), gS(-2,19), gS(-4,7),
           gS(0,-3), gS(0,0), gS(-2,0), gS(1,-17), gS(0,-14), gS(-2,-8), gS(-9,-3), gS(0,0),
           gS(0,11), gS(0,0), gS(2,-6), gS(0,-9), gS(0,-7), gS(0,-8), gS(0,0), gS(0,-1),
           gS(4,0), gS(0,5), gS(0,3), gS(-4,3), gS(-2,5), gS(-2,9), gS(4,-4), gS(8,0),
           gS(5,26), gS(0,15), gS(-1,23), gS(-1,18), gS(-4,15), gS(4,19), gS(6,14), gS(10,15),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(28,12), gS(15,21), gS(0,27), gS(0,21), gS(5,20), gS(12,20), gS(27,16), gS(38,14),
           gS(7,15), gS(0,16), gS(17,12), gS(34,3), gS(26,5), gS(28,3), gS(-5,15), gS(10,10),
           gS(4,17), gS(20,9), gS(19,10), gS(27,1), gS(43,-4), gS(37,-2), gS(29,0), gS(-3,12),
           gS(0,13), gS(0,13), gS(18,8), gS(21,1), gS(20,0), gS(20,-1), gS(6,2), gS(0,3),
           gS(-13,8), gS(-11,13), gS(0,8), gS(-1,6), gS(1,3), gS(-2,1), gS(-5,5), gS(-19,4),
           gS(-16,0), gS(-9,0), gS(-4,0), gS(-3,-3), gS(-3,-2), gS(0,-10), gS(1,-9), gS(-13,-10),
           gS(-21,-8), gS(-4,-10), gS(0,-7), gS(0,-10), gS(12,-16), gS(8,-22), gS(5,-23), gS(-39,-11),
           gS(-5,-9), gS(1,-12), gS(2,-8), gS(11,-19), gS(13,-22), gS(13,-20), gS(-8,-10), gS(-3,-24),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-33,-1), gS(-43,-2), gS(-74,16), gS(-81,16), gS(-56,16), gS(-91,13), gS(-25,-7), gS(-28,-15),
           gS(-37,-1), gS(-17,0), gS(-10,0), gS(-12,9), gS(-11,6), gS(-13,3), gS(-25,-4), gS(-23,-7),
           gS(-3,0), gS(0,1), gS(0,3), gS(24,-3), gS(16,0), gS(21,0), gS(7,0), gS(8,-5),
           gS(-7,-2), gS(9,5), gS(4,2), gS(22,9), gS(16,9), gS(12,4), gS(7,0), gS(-1,-10),
           gS(6,-6), gS(-2,3), gS(8,4), gS(19,7), gS(22,5), gS(6,3), gS(8,0), gS(6,-17),
           gS(4,-16), gS(20,-5), gS(11,-2), gS(11,6), gS(6,11), gS(21,-7), gS(18,-10), gS(9,-13),
           gS(16,-21), gS(12,-24), gS(16,-9), gS(5,0), gS(14,0), gS(15,-5), gS(21,-20), gS(8,-30),
           gS(-2,-32), gS(8,-7), gS(-1,-3), gS(2,0), gS(7,-4), gS(-11,4), gS(0,-8), gS(-1,-24),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-159,-58), gS(-31,-4), gS(-80,8), gS(-13,-3), gS(0,-1), gS(-79,2), gS(-29,-5), gS(-89,-64),
           gS(-32,-9), gS(-17,0), gS(-1,0), gS(21,5), gS(9,3), gS(29,-12), gS(-11,-1), gS(-16,-16),
           gS(-10,-1), gS(9,2), gS(13,19), gS(30,5), gS(44,0), gS(47,5), gS(15,-1), gS(13,-13),
           gS(5,-1), gS(7,0), gS(15,14), gS(32,15), gS(20,17), gS(35,11), gS(12,-2), gS(23,-11),
           gS(0,1), gS(13,5), gS(18,20), gS(23,16), gS(24,19), gS(29,12), gS(25,0), gS(11,0),
           gS(-7,-19), gS(10,-5), gS(10,7), gS(20,17), gS(23,14), gS(21,3), gS(25,-8), gS(3,-9),
           gS(-6,-26), gS(-10,3), gS(4,-3), gS(6,9), gS(12,7), gS(13,-2), gS(1,1), gS(3,-15),
           gS(-40,-30), gS(-13,-15), gS(-1,-5), gS(1,2), gS(9,4), gS(-2,-3), gS(-7,-5), gS(-19,-29),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-13,0), gS(0,11), gS(-3,12), gS(-4,21), gS(-9,26), gS(0,11), gS(-9,18), gS(-11,16),
           gS(-13,1), gS(-27,25), gS(-13,25), gS(-30,48), gS(-47,67), gS(-24,34), gS(-16,38), gS(1,12),
           gS(-9,-15), gS(0,0), gS(-3,23), gS(0,36), gS(-3,45), gS(0,37), gS(6,14), gS(-9,15),
           gS(-3,-16), gS(-6,8), gS(-5,19), gS(-4,40), gS(-8,53), gS(-6,37), gS(-4,27), gS(3,-3),
           gS(1,-26), gS(-5,0), gS(0,11), gS(-2,38), gS(3,34), gS(-4,11), gS(8,4), gS(-4,-2),
           gS(-2,-32), gS(10,-24), gS(7,-6), gS(4,0), gS(4,0), gS(8,-6), gS(14,-11), gS(5,-27),
           gS(5,-52), gS(12,-57), gS(20,-58), gS(12,-33), gS(15,-30), gS(22,-64), gS(26,-80), gS(1,-44),
           gS(6,-62), gS(8,-69), gS(6,-76), gS(5,-57), gS(8,-64), gS(-3,-75), gS(0,-66), gS(12,-69),
};

/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(86,113), gS(508,683), gS(353,376), gS(377,397), gS(1002,1264),
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
inline int PiecePieceInteraction(const Board &, Color, evalBits *);

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
