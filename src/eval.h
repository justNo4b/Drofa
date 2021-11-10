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

const int KING_HIGH_DANGER = gS(-50,-61);

const int KING_MED_DANGER = gS(-23,-37);

const int KING_LOW_DANGER = gS(67,-64);

const int BISHOP_PAIR_BONUS = gS(31,52);

const int PAWN_SUPPORTED = gS(13,8);

const int DOUBLED_PAWN_PENALTY = gS(-7,-23);

const int ISOLATED_PAWN_PENALTY = gS(-8,-4);

const int PAWN_BLOCKED = gS(3,14);

const int PASSER_BLOCKED = gS(7,-3);

const int BISHOP_RAMMED_PENALTY = gS(-3,-5);

const int BISHOP_CENTER_CONTROL = gS(10,9);

const int MINOR_BEHIND_PAWN = gS(4,10);

const int MINOR_BEHIND_PASSER = gS(11,0);

const int MINOR_BLOCK_OWN_PAWN = gS(-7,-1);

const int MINOR_BLOCK_OWN_PASSER = gS(0,-5);

const int KING_AHEAD_PASSER = gS(-17,15);

const int KING_EQUAL_PASSER = gS(11,7);

const int KING_BEHIND_PASSER = gS(1,-5);

const int KING_OPEN_FILE = gS(-58,8);

const int KING_OWN_SEMI_FILE = gS(-25,15);

const int KING_ENEMY_SEMI_LINE = gS(-18,8);

const int KING_ATTACK_PAWN = gS(19,50);
/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(5,0), gS(6,5), gS(8,12),
           gS(20,25), gS(40,57), gS(5,36),
};

const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-8,-7), gS(-4,8), gS(0,45),
           gS(16,55), gS(25,79), gS(55,87),
};

const int PASSED_PAWN_FILES[4] = {
           gS(3,15), gS(0,6), gS(2,-1), gS(2,-14),
};

const int PASSED_PAWN_FREE[7] = {
           gS(-3,-11), gS(-11,5), gS(-14,9), gS(-5,14),
           gS(0,9), gS(7,24), gS(57,27),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(13,27), gS(33,57), gS(38,116),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-7,-20), gS(16,4), gS(1,-19), gS(-8,-7),
           gS(-11,-8), gS(-20,-8), gS(-19,-11), gS(-17,-9),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(3,36), gS(-8,28), gS(-5,10),
           gS(-3,1), gS(0,1), gS(15,0), gS(13,-1),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-45,0), gS(10,-17), gS(0,1),
           gS(2,12), gS(-2,23), gS(1,24), gS(-8,23),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-6,-8), gS(1,3), gS(7,8), gS(13,12),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(28,-3), gS(33,0),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(14,-2), gS(24,11),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-66,-40), gS(-50,-46), gS(-53,-67), gS(-49,-26),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(16,33), gS(15,35), gS(16,31),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-3,19), gS(64,22), gS(43,42),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(63,31), gS(39,15), gS(43,26),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-5,2), gS(4,10), gS(4,10), gS(-1,13),
           gS(-8,5), gS(-19,-4), gS(-6,11), gS(0,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(9,0), gS(12,24), gS(19,5), gS(23,16),
           gS(10,0), gS(-15,-3), gS(-3,8), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-8,-66), gS(1,-26), gS(8,-13), gS(13,-2), gS(16,7), gS(17,10), gS(16,13),
           gS(16,12), gS(16,14), gS(16,12), gS(19,9), gS(34,0), gS(35,14), gS(62,-9),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-3,-91), gS(8,-34), gS(15,-7), gS(21,3), gS(25,11),
           gS(29,16), gS(33,14), gS(37,8), gS(41,-1),
};

const int KING_MOBILITY[9] = {
           gS(14,-18), gS(6,-5), gS(1,3), gS(-1,9), gS(-5,7),
           gS(-9,6), gS(0,4), gS(-2,0), gS(5,-16),
};

const int ROOK_MOBILITY[15] = {
           gS(-25,-86), gS(-12,-48), gS(-5,-30), gS(2,-17), gS(4,-5), gS(2,7), gS(7,8),
           gS(9,9), gS(13,13), gS(15,17), gS(17,21), gS(19,24), gS(24,22), gS(30,21),
           gS(53,11),
};

const int QUEEN_MOBILITY[28] = {
           gS(-12,-114), gS(-7,-166), gS(-6,-111), gS(-5,-68), gS(-5,-47), gS(-4,-30), gS(-3,-14),
           gS(-1,-2), gS(0,10), gS(0,21), gS(1,27), gS(2,32), gS(3,36), gS(3,38),
           gS(2,41), gS(3,43), gS(2,45), gS(6,41), gS(8,39), gS(11,38), gS(13,32),
           gS(16,28), gS(9,32), gS(19,21), gS(4,19), gS(12,15), gS(17,20), gS(15,17),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(-3,-96), gS(0,-38), gS(0,-21), gS(0,-8), gS(0,-22), gS(0,-22), gS(0,-14), gS(-2,-94),
           gS(-4,-22), gS(0,33), gS(5,32), gS(1,30), gS(1,14), gS(0,33), gS(0,31), gS(-4,-22),
           gS(0,-1), gS(27,24), gS(24,35), gS(5,38), gS(8,39), gS(29,34), gS(17,28), gS(0,-3),
           gS(-4,-13), gS(13,13), gS(8,24), gS(-5,35), gS(-4,32), gS(13,23), gS(15,13), gS(-38,-14),
           gS(-17,-24), gS(7,0), gS(-3,17), gS(-23,27), gS(-17,25), gS(-21,19), gS(0,2), gS(-51,-16),
           gS(-27,-23), gS(9,-4), gS(-15,10), gS(-36,22), gS(-31,21), gS(-26,11), gS(-3,-3), gS(-18,-22),
           gS(0,-27), gS(-4,-5), gS(-1,1), gS(-21,11), gS(-23,11), gS(-3,2), gS(-7,-4), gS(0,-29),
           gS(-19,-54), gS(2,-37), gS(-16,-16), gS(-34,-15), gS(11,-46), gS(-13,-25), gS(1,-35), gS(-4,-66),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(54,70), gS(40,80), gS(42,68), gS(66,57), gS(66,62), gS(56,64), gS(-1,87), gS(1,76),
           gS(3,35), gS(11,36), gS(26,21), gS(48,8), gS(49,10), gS(74,14), gS(32,29), gS(28,21),
           gS(-7,15), gS(1,15), gS(12,3), gS(20,6), gS(27,5), gS(27,2), gS(24,4), gS(0,7),
           gS(-20,1), gS(-17,8), gS(-6,0), gS(12,0), gS(12,0), gS(12,-2), gS(0,-1), gS(-14,-6),
           gS(-23,-4), gS(-19,-1), gS(-4,-5), gS(-3,1), gS(2,1), gS(3,-3), gS(0,-9), gS(-17,-11),
           gS(-27,5), gS(-13,4), gS(-13,9), gS(0,9), gS(-1,12), gS(5,6), gS(0,0), gS(-17,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-1,7), gS(-1,5), gS(0,2), gS(0,2), gS(0,0), gS(-1,0), gS(0,0), gS(0,2),
           gS(-1,7), gS(-2,8), gS(-3,5), gS(0,0), gS(0,1), gS(-1,2), gS(-1,12), gS(-1,9),
           gS(-3,-3), gS(-3,0), gS(-6,-6), gS(0,-17), gS(-1,-12), gS(-4,-5), gS(-8,0), gS(-2,0),
           gS(-1,-10), gS(-3,-7), gS(0,-11), gS(0,-22), gS(0,-17), gS(-1,-6), gS(-4,-1), gS(0,0),
           gS(0,-20), gS(-4,-5), gS(-4,-9), gS(-4,0), gS(-6,-6), gS(-6,1), gS(1,-5), gS(0,-19),
           gS(0,-29), gS(-2,-15), gS(-4,-3), gS(-7,0), gS(-5,1), gS(0,-2), gS(1,-6), gS(0,-36),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-3,2), gS(-2,2), gS(0,0), gS(0,0), gS(0,0), gS(-1,0), gS(0,0), gS(0,0),
           gS(-1,6), gS(-3,7), gS(-3,6), gS(0,0), gS(0,1), gS(-2,0), gS(0,10), gS(-2,5),
           gS(0,-3), gS(0,0), gS(-1,-3), gS(1,-16), gS(0,-13), gS(0,-7), gS(-8,-4), gS(-1,0),
           gS(0,4), gS(0,-1), gS(3,-6), gS(1,-12), gS(0,-10), gS(0,-8), gS(0,-1), gS(0,-5),
           gS(3,-1), gS(0,4), gS(0,1), gS(-3,3), gS(-2,3), gS(-2,6), gS(3,-5), gS(5,-2),
           gS(4,13), gS(0,8), gS(0,17), gS(-2,13), gS(-3,12), gS(4,17), gS(5,10), gS(6,4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(26,13), gS(15,22), gS(0,28), gS(0,21), gS(7,20), gS(16,20), gS(29,16), gS(39,14),
           gS(6,16), gS(0,16), gS(18,12), gS(33,4), gS(26,6), gS(32,2), gS(-2,13), gS(11,10),
           gS(4,18), gS(20,9), gS(18,11), gS(28,1), gS(43,-4), gS(40,-3), gS(32,0), gS(-1,12),
           gS(1,13), gS(0,13), gS(17,9), gS(21,2), gS(20,0), gS(20,-1), gS(7,2), gS(0,4),
           gS(-13,9), gS(-11,14), gS(-1,10), gS(-1,6), gS(0,3), gS(-2,1), gS(-5,6), gS(-19,5),
           gS(-16,0), gS(-8,0), gS(-4,0), gS(-4,-2), gS(-3,-2), gS(0,-9), gS(1,-8), gS(-12,-10),
           gS(-20,-8), gS(-4,-10), gS(0,-7), gS(0,-10), gS(11,-15), gS(8,-22), gS(6,-23), gS(-39,-10),
           gS(-4,-9), gS(1,-12), gS(2,-8), gS(11,-19), gS(14,-23), gS(14,-21), gS(-7,-10), gS(-3,-23),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-33,0), gS(-43,-1), gS(-74,16), gS(-81,18), gS(-60,18), gS(-91,13), gS(-21,-6), gS(-27,-12),
           gS(-38,0), gS(-17,0), gS(-10,0), gS(-12,12), gS(-10,8), gS(-8,2), gS(-26,-4), gS(-22,-6),
           gS(-4,0), gS(0,3), gS(1,4), gS(19,-1), gS(19,0), gS(5,0), gS(5,0), gS(12,-5),
           gS(-9,-1), gS(11,5), gS(1,4), gS(22,7), gS(15,9), gS(13,2), gS(8,0), gS(0,-10),
           gS(8,-6), gS(-1,3), gS(9,3), gS(20,7), gS(22,3), gS(6,3), gS(9,0), gS(8,-17),
           gS(4,-16), gS(21,-7), gS(11,-4), gS(12,5), gS(7,10), gS(22,-8), gS(19,-10), gS(9,-12),
           gS(18,-22), gS(13,-25), gS(17,-11), gS(6,0), gS(15,-1), gS(15,-6), gS(22,-22), gS(9,-31),
           gS(-1,-33), gS(9,-10), gS(0,-2), gS(4,-1), gS(8,-4), gS(-10,4), gS(0,-9), gS(0,-25),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-151,-63), gS(-31,-9), gS(-76,8), gS(-13,-2), gS(0,0), gS(-70,2), gS(-29,-5), gS(-89,-64),
           gS(-27,-9), gS(-12,2), gS(3,0), gS(29,8), gS(16,5), gS(34,-9), gS(-10,0), gS(-11,-17),
           gS(-10,-1), gS(15,1), gS(17,21), gS(29,10), gS(50,2), gS(38,6), gS(13,-3), gS(12,-13),
           gS(8,-1), gS(7,1), gS(15,15), gS(30,17), gS(19,16), gS(34,9), gS(9,-6), gS(23,-11),
           gS(1,2), gS(13,5), gS(17,21), gS(24,17), gS(25,22), gS(29,13), gS(25,1), gS(12,0),
           gS(-7,-18), gS(11,-5), gS(9,8), gS(22,20), gS(24,16), gS(22,4), gS(26,-7), gS(3,-8),
           gS(-6,-24), gS(-10,4), gS(3,-5), gS(6,9), gS(12,7), gS(11,-3), gS(1,2), gS(1,-14),
           gS(-40,-36), gS(-13,-15), gS(-2,-6), gS(1,1), gS(8,3), gS(-4,-4), gS(-7,-6), gS(-23,-33),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-13,0), gS(0,11), gS(-2,19), gS(-4,29), gS(-7,35), gS(6,20), gS(-8,26), gS(-10,18),
           gS(-13,0), gS(-28,25), gS(-12,24), gS(-30,55), gS(-44,76), gS(-22,43), gS(-17,49), gS(1,19),
           gS(-9,-15), gS(0,-1), gS(-3,22), gS(0,39), gS(-2,49), gS(0,47), gS(5,20), gS(-9,24),
           gS(-3,-20), gS(-7,6), gS(-4,16), gS(-5,40), gS(-10,56), gS(-8,45), gS(-7,34), gS(3,0),
           gS(1,-29), gS(-6,0), gS(-1,7), gS(-3,38), gS(3,32), gS(-6,13), gS(8,4), gS(-5,-1),
           gS(-2,-40), gS(11,-27), gS(6,-7), gS(5,-2), gS(4,-1), gS(8,-7), gS(14,-11), gS(4,-29),
           gS(5,-57), gS(11,-56), gS(21,-61), gS(13,-36), gS(16,-33), gS(22,-67), gS(25,-80), gS(1,-48),
           gS(4,-62), gS(8,-72), gS(6,-75), gS(6,-60), gS(8,-64), gS(-4,-77), gS(0,-74), gS(9,-69),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(83,114), gS(498,687), gS(350,381), gS(372,402), gS(994,1285),
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
