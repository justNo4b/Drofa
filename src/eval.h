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

  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int COUNT_TO_POWER_DIVISOR = 128;
  /**@}*/

const int TEMPO = 10;

const int KING_HIGH_DANGER = gS(-52,-63);

const int KING_MED_DANGER = gS(-23,-36);

const int KING_LOW_DANGER = gS(65,-64);

const int BISHOP_PAIR_BONUS = gS(30,55);

const int PAWN_SUPPORTED = gS(13,8);

const int DOUBLED_PAWN_PENALTY = gS(-7,-21);

const int ISOLATED_PAWN_PENALTY = gS(-8,-5);

const int PAWN_BLOCKED = gS(3,15);

const int PASSER_BLOCKED = gS(6,-3);

const int BISHOP_RAMMED_PENALTY = gS(-3,-5);

const int BISHOP_CENTER_CONTROL = gS(9,9);

const int MINOR_BEHIND_PAWN = gS(4,10);

const int MINOR_BEHIND_PASSER = gS(10,0);

const int MINOR_BLOCK_OWN_PAWN = gS(-7,-1);

const int MINOR_BLOCK_OWN_PASSER = gS(0,-4);

const int KING_AHEAD_PASSER = gS(-17,15);

const int KING_EQUAL_PASSER = gS(10,7);

const int KING_BEHIND_PASSER = gS(3,-6);

const int KING_OPEN_FILE = gS(-65,8);

const int KING_OWN_SEMI_FILE = gS(-25,15);

const int KING_ENEMY_SEMI_LINE = gS(-18,8);

const int KING_ATTACK_PAWN = gS(17,50);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,0), gS(6,5), gS(8,12),
           gS(19,25), gS(39,57), gS(5,35),
};

const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-7,-10), gS(-4,7), gS(0,45),
           gS(17,56), gS(25,80), gS(52,87),
};

const int PASSED_PAWN_FILES[8] = {
           gS(0,20), gS(-1,12), gS(2,6), gS(3,-4),
           gS(-2,-2), gS(-2,5), gS(-4,13), gS(0,14),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-1,9), gS(5,24), gS(55,27),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(12,27), gS(31,57), gS(36,116),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-9,-24), gS(15,0), gS(0,-21), gS(-9,-8),
           gS(-9,-7), gS(-18,-5), gS(-17,-7), gS(-16,-6),
};

const int PASSED_ENEMY_PAWN_DISTANCE[8] = {
           gS(0,0), gS(0,-5), gS(-2,-6), gS(1,-1),
           gS(1,0), gS(1,2), gS(0,5), gS(1,4),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(3,36), gS(-8,28), gS(-6,11),
           gS(-3,2), gS(0,1), gS(15,-2), gS(13,-2),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-43,0), gS(12,-16), gS(2,1),
           gS(3,12), gS(-3,22), gS(-1,23), gS(-10,21),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-6,-7), gS(0,3), gS(7,8), gS(13,11),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(31,-4), gS(33,0),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(15,-3), gS(24,11),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-65,-40), gS(-49,-45), gS(-53,-67), gS(-49,-26),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(15,33), gS(14,35), gS(15,30),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-3,19), gS(63,22), gS(42,41),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(60,30), gS(38,15), gS(42,25),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-5,2), gS(3,10), gS(4,10), gS(0,14),
           gS(-7,5), gS(-18,-4), gS(-6,10), gS(0,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(10,0), gS(12,25), gS(19,6), gS(23,17),
           gS(10,0), gS(-14,-3), gS(-2,8), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-11,-68), gS(0,-27), gS(6,-14), gS(12,-3), gS(15,6), gS(17,10), gS(17,14),
           gS(18,13), gS(19,15), gS(21,13), gS(24,11), gS(37,2), gS(36,16), gS(63,-8),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-4,-91), gS(8,-34), gS(15,-8), gS(20,3), gS(25,10),
           gS(29,16), gS(33,14), gS(37,8), gS(41,0),
};

const int KING_MOBILITY[9] = {
           gS(19,-17), gS(10,-3), gS(4,3), gS(0,8), gS(-4,6),
           gS(-11,5), gS(-5,4), gS(-10,0), gS(0,-18),
};

const int ROOK_MOBILITY[15] = {
           gS(-26,-87), gS(-13,-50), gS(-6,-31), gS(1,-18), gS(3,-5), gS(1,7), gS(6,8),
           gS(9,9), gS(12,14), gS(15,18), gS(18,22), gS(21,25), gS(26,23), gS(32,22),
           gS(55,13),
};

const int QUEEN_MOBILITY[28] = {
           gS(-15,-114), gS(-10,-167), gS(-10,-112), gS(-8,-69), gS(-7,-47), gS(-6,-31), gS(-4,-15),
           gS(-2,-3), gS(-1,9), gS(0,20), gS(1,26), gS(2,32), gS(4,35), gS(5,38),
           gS(5,41), gS(6,44), gS(6,46), gS(9,42), gS(11,41), gS(12,39), gS(14,32),
           gS(16,28), gS(10,32), gS(20,21), gS(5,19), gS(12,15), gS(17,20), gS(15,17),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(-3,-96), gS(0,-38), gS(0,-21), gS(0,-8), gS(0,-22), gS(0,-22), gS(0,-14), gS(-2,-94),
           gS(-4,-22), gS(0,32), gS(5,31), gS(1,29), gS(1,14), gS(0,32), gS(0,31), gS(-4,-22),
           gS(0,-2), gS(27,24), gS(24,35), gS(5,38), gS(8,39), gS(29,34), gS(17,28), gS(0,-3),
           gS(-4,-13), gS(12,13), gS(9,24), gS(-5,35), gS(-4,32), gS(13,23), gS(15,13), gS(-39,-14),
           gS(-18,-24), gS(7,0), gS(-3,16), gS(-24,27), gS(-18,25), gS(-21,18), gS(0,2), gS(-51,-16),
           gS(-28,-22), gS(8,-4), gS(-17,10), gS(-39,22), gS(-35,21), gS(-28,12), gS(-5,-2), gS(-19,-21),
           gS(0,-26), gS(-3,-4), gS(0,1), gS(-22,12), gS(-22,12), gS(-2,2), gS(-6,-3), gS(-1,-29),
           gS(-19,-53), gS(3,-37), gS(-16,-16), gS(-31,-16), gS(14,-47), gS(-11,-26), gS(2,-34), gS(-2,-66),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(53,69), gS(40,79), gS(42,69), gS(66,59), gS(66,63), gS(56,64), gS(-2,86), gS(0,76),
           gS(2,33), gS(10,34), gS(24,22), gS(47,11), gS(48,11), gS(72,14), gS(31,28), gS(27,20),
           gS(-8,14), gS(0,14), gS(11,3), gS(19,7), gS(25,6), gS(25,2), gS(23,4), gS(0,7),
           gS(-21,0), gS(-18,7), gS(-7,0), gS(11,1), gS(11,0), gS(11,-2), gS(-1,-1), gS(-14,-5),
           gS(-23,-5), gS(-20,-2), gS(-4,-4), gS(-4,3), gS(2,2), gS(3,-2), gS(0,-9), gS(-18,-11),
           gS(-27,5), gS(-13,4), gS(-12,10), gS(0,11), gS(-1,14), gS(6,7), gS(0,0), gS(-17,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-1,6), gS(-1,5), gS(0,1), gS(0,1), gS(0,0), gS(-1,0), gS(0,0), gS(0,1),
           gS(-1,7), gS(-2,7), gS(-4,4), gS(-1,0), gS(0,1), gS(-2,1), gS(-2,10), gS(-1,8),
           gS(-3,-3), gS(-3,0), gS(-6,-7), gS(0,-18), gS(-1,-12), gS(-4,-5), gS(-8,0), gS(-2,0),
           gS(-1,-9), gS(-3,-7), gS(0,-12), gS(0,-22), gS(-1,-17), gS(-1,-6), gS(-4,0), gS(-1,0),
           gS(0,-19), gS(-4,-4), gS(-5,-9), gS(-4,0), gS(-6,-5), gS(-6,1), gS(0,-5), gS(0,-18),
           gS(0,-27), gS(-2,-15), gS(-4,-2), gS(-7,0), gS(-6,3), gS(0,0), gS(1,-4), gS(0,-34),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-3,1), gS(-2,1), gS(0,0), gS(0,0), gS(0,0), gS(-1,0), gS(0,0), gS(0,0),
           gS(-1,5), gS(-3,6), gS(-3,5), gS(-1,0), gS(0,0), gS(-3,0), gS(-1,8), gS(-2,4),
           gS(0,-3), gS(-1,0), gS(-2,-4), gS(0,-17), gS(0,-13), gS(-1,-8), gS(-8,-4), gS(-2,0),
           gS(0,3), gS(0,-2), gS(2,-7), gS(0,-13), gS(0,-11), gS(0,-9), gS(-1,-1), gS(-1,-5),
           gS(2,-1), gS(0,3), gS(0,0), gS(-3,3), gS(-2,3), gS(-2,5), gS(2,-4), gS(5,-2),
           gS(3,12), gS(0,7), gS(0,17), gS(-2,12), gS(-3,13), gS(3,17), gS(4,10), gS(6,3),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(25,13), gS(15,22), gS(0,28), gS(1,21), gS(8,21), gS(17,20), gS(30,16), gS(40,14),
           gS(6,16), gS(0,17), gS(18,12), gS(33,5), gS(26,7), gS(32,2), gS(0,14), gS(11,11),
           gS(3,18), gS(20,9), gS(18,11), gS(29,1), gS(44,-3), gS(41,-3), gS(32,0), gS(0,12),
           gS(0,13), gS(0,13), gS(16,9), gS(21,2), gS(21,1), gS(21,0), gS(8,3), gS(0,5),
           gS(-14,9), gS(-11,14), gS(-2,9), gS(-1,6), gS(0,4), gS(-1,2), gS(-4,6), gS(-19,5),
           gS(-17,0), gS(-9,0), gS(-4,0), gS(-4,-2), gS(-2,-1), gS(0,-8), gS(2,-8), gS(-13,-10),
           gS(-22,-9), gS(-5,-10), gS(0,-7), gS(0,-10), gS(11,-15), gS(9,-22), gS(5,-23), gS(-40,-11),
           gS(-6,-11), gS(0,-13), gS(2,-9), gS(10,-20), gS(14,-23), gS(13,-21), gS(-8,-11), gS(-3,-25),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-33,0), gS(-43,-1), gS(-73,16), gS(-81,18), gS(-60,18), gS(-91,13), gS(-20,-6), gS(-27,-12),
           gS(-38,0), gS(-17,0), gS(-10,0), gS(-12,12), gS(-9,9), gS(-7,2), gS(-26,-3), gS(-22,-5),
           gS(-4,0), gS(0,3), gS(1,4), gS(19,-1), gS(22,0), gS(5,0), gS(8,0), gS(12,-5),
           gS(-9,-1), gS(10,5), gS(1,3), gS(24,7), gS(14,10), gS(16,2), gS(8,0), gS(2,-9),
           gS(7,-6), gS(-1,3), gS(11,3), gS(19,6), gS(23,3), gS(5,4), gS(10,0), gS(9,-16),
           gS(3,-15), gS(22,-7), gS(11,-5), gS(12,5), gS(6,10), gS(22,-8), gS(18,-10), gS(10,-11),
           gS(19,-22), gS(12,-25), gS(17,-11), gS(6,0), gS(15,0), gS(15,-6), gS(21,-23), gS(9,-31),
           gS(-1,-33), gS(10,-9), gS(0,-2), gS(4,-1), gS(8,-4), gS(-11,4), gS(0,-9), gS(0,-26),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-151,-62), gS(-31,-9), gS(-75,8), gS(-13,-2), gS(0,0), gS(-70,2), gS(-29,-4), gS(-89,-63),
           gS(-27,-9), gS(-11,2), gS(4,0), gS(29,8), gS(16,6), gS(34,-9), gS(-10,0), gS(-9,-16),
           gS(-10,-2), gS(15,1), gS(17,21), gS(30,11), gS(52,2), gS(38,6), gS(14,-2), gS(12,-13),
           gS(8,-2), gS(7,1), gS(16,15), gS(32,18), gS(23,15), gS(36,9), gS(13,-6), gS(24,-10),
           gS(0,1), gS(13,4), gS(16,21), gS(24,17), gS(25,23), gS(29,14), gS(26,1), gS(11,0),
           gS(-8,-18), gS(9,-5), gS(9,6), gS(21,19), gS(24,16), gS(20,4), gS(25,-7), gS(1,-8),
           gS(-7,-24), gS(-10,4), gS(1,-5), gS(6,8), gS(11,7), gS(10,-3), gS(0,2), gS(0,-14),
           gS(-40,-36), gS(-14,-16), gS(-2,-7), gS(0,0), gS(7,2), gS(-6,-5), gS(-9,-7), gS(-23,-33),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-13,0), gS(0,12), gS(-2,20), gS(-3,30), gS(-6,36), gS(7,21), gS(-7,26), gS(-9,19),
           gS(-13,0), gS(-28,25), gS(-10,25), gS(-29,56), gS(-42,78), gS(-19,44), gS(-15,50), gS(3,20),
           gS(-9,-16), gS(0,-1), gS(-3,22), gS(3,40), gS(0,51), gS(3,49), gS(9,22), gS(-5,26),
           gS(-3,-20), gS(-8,5), gS(-3,15), gS(-2,41), gS(-8,57), gS(-5,47), gS(-5,35), gS(5,1),
           gS(0,-29), gS(-6,0), gS(-2,6), gS(-4,37), gS(2,31), gS(-6,14), gS(8,4), gS(-5,0),
           gS(-3,-40), gS(9,-28), gS(4,-8), gS(3,-3), gS(2,-2), gS(7,-8), gS(12,-12), gS(4,-29),
           gS(3,-57), gS(8,-57), gS(18,-63), gS(11,-38), gS(13,-35), gS(20,-68), gS(22,-81), gS(0,-49),
           gS(1,-63), gS(5,-73), gS(4,-76), gS(3,-62), gS(6,-65), gS(-6,-78), gS(0,-74), gS(8,-70),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(83,112), gS(491,688), gS(347,382), gS(371,403), gS(991,1285),
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
void SetupTuning(int phase, PieceType piece, int value);

};

#endif
