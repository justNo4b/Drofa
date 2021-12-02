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
    U64 AttackedByQueen[2];
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
  const int QUEEN_SAFE_FACE_CHECK[2] = {35, 100};

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

const int KING_HIGH_DANGER = gS(-50,-63);

const int KING_MED_DANGER = gS(-24,-38);

const int KING_LOW_DANGER = gS(71,-75);

const int BISHOP_PAIR_BONUS = gS(31,52);

const int DOUBLED_PAWN_PENALTY = gS(-7,-23);

const int ISOLATED_PAWN_PENALTY = gS(-8,-4);

const int PAWN_BLOCKED = gS(3,13);

const int PASSER_BLOCKED = gS(6,-3);

const int BISHOP_RAMMED_PENALTY = gS(-2,-4);

const int BISHOP_CENTER_CONTROL = gS(9,9);

const int MINOR_BEHIND_PAWN = gS(4,10);

const int MINOR_BEHIND_PASSER = gS(10,0);

const int MINOR_BLOCK_OWN_PAWN = gS(-8,0);

const int MINOR_BLOCK_OWN_PASSER = gS(0,-5);

const int KING_AHEAD_PASSER = gS(-13,14);

const int KING_EQUAL_PASSER = gS(12,7);

const int KING_BEHIND_PASSER = gS(0,-5);

const int KING_OPEN_FILE = gS(-58,8);

const int KING_OWN_SEMI_FILE = gS(-24,15);

const int KING_ENEMY_SEMI_LINE = gS(-17,8);

const int KING_ATTACK_PAWN = gS(19,48);
/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-9,-8), gS(-6,6), gS(0,43),
           gS(16,54), gS(26,77), gS(54,86),
};

const int PASSED_PAWN_FILES[8] = {
           gS(0,21), gS(-1,12), gS(1,4), gS(1,-7),
           gS(-4,-4), gS(-5,4), gS(-4,13), gS(0,15),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(0,8), gS(7,23), gS(64,24),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(12,28), gS(36,60), gS(50,112),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-5,-22), gS(17,4), gS(1,-19), gS(-7,-7),
           gS(-10,-8), gS(-19,-9), gS(-17,-13), gS(-15,-11),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(4,35), gS(-9,27), gS(-6,9),
           gS(-3,0), gS(0,0), gS(14,-1), gS(13,-2),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-46,-2), gS(9,-17), gS(0,0),
           gS(2,11), gS(-2,22), gS(1,22), gS(-8,21),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-6,-8), gS(1,3), gS(7,8), gS(13,11),
};

const int CANDIDATE_PASSED_PAWN[7] = {
           gS(0,0), gS(-12,-1), gS(-7,4), gS(0,12),
           gS(5,21), gS(12,16), gS(0,0),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(29,-3), gS(32,0),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(14,-2), gS(23,9),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-66,-41), gS(-50,-47), gS(-54,-66), gS(-49,-30),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(16,33), gS(15,35), gS(15,31),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-3,19), gS(64,22), gS(43,42),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(62,35), gS(39,19), gS(42,34),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-6,2), gS(6,10), gS(0,12), gS(0,10),
           gS(-8,5), gS(-21,0), gS(-4,16), gS(0,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(8,0), gS(14,23), gS(15,4), gS(25,11),
           gS(10,0), gS(-18,-1), gS(-2,13), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-7,-65), gS(1,-25), gS(9,-13), gS(14,-2), gS(17,6), gS(18,9), gS(17,12),
           gS(17,11), gS(17,12), gS(17,11), gS(18,9), gS(31,0), gS(32,11), gS(58,-11),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-2,-93), gS(9,-35), gS(16,-7), gS(21,3), gS(26,10),
           gS(30,16), gS(34,13), gS(38,7), gS(42,-1),
};

const int KING_MOBILITY[9] = {
           gS(14,-25), gS(5,-4), gS(0,4), gS(-2,10), gS(-5,8),
           gS(-9,6), gS(0,4), gS(0,0), gS(14,-18),
};

const int ROOK_MOBILITY[15] = {
           gS(-26,-86), gS(-12,-48), gS(-6,-30), gS(1,-17), gS(4,-5), gS(1,6), gS(7,8),
           gS(9,8), gS(12,12), gS(15,16), gS(17,20), gS(19,23), gS(23,21), gS(28,20),
           gS(52,10),
};

const int QUEEN_MOBILITY[28] = {
           gS(-11,-114), gS(-6,-162), gS(-6,-111), gS(-5,-72), gS(-4,-51), gS(-3,-33), gS(-2,-16),
           gS(-1,-3), gS(0,9), gS(0,20), gS(1,26), gS(2,32), gS(3,36), gS(3,38),
           gS(2,41), gS(3,43), gS(2,46), gS(5,41), gS(7,39), gS(10,37), gS(13,30),
           gS(16,24), gS(9,28), gS(19,18), gS(1,16), gS(9,12), gS(14,17), gS(12,14),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(0,-93), gS(0,-35), gS(0,-18), gS(0,-7), gS(0,-19), gS(0,-22), gS(0,-13), gS(0,-94),
           gS(-1,-19), gS(0,33), gS(5,33), gS(0,32), gS(1,15), gS(0,34), gS(0,31), gS(-4,-22),
           gS(0,-2), gS(27,24), gS(21,35), gS(2,38), gS(5,39), gS(26,35), gS(14,28), gS(0,-3),
           gS(-2,-13), gS(11,13), gS(3,25), gS(-6,35), gS(-7,33), gS(9,23), gS(12,13), gS(-37,-15),
           gS(-14,-24), gS(4,0), gS(-3,16), gS(-19,26), gS(-14,24), gS(-21,18), gS(-1,1), gS(-48,-18),
           gS(-20,-26), gS(11,-6), gS(-13,8), gS(-31,20), gS(-26,19), gS(-23,10), gS(-2,-4), gS(-12,-25),
           gS(1,-29), gS(-4,-6), gS(-4,1), gS(-22,11), gS(-24,11), gS(-3,2), gS(-6,-5), gS(1,-31),
           gS(-17,-55), gS(3,-39), gS(-15,-17), gS(-35,-15), gS(10,-46), gS(-14,-25), gS(3,-36), gS(-2,-67),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(54,69), gS(40,79), gS(39,67), gS(66,56), gS(65,61), gS(52,64), gS(0,85), gS(1,74),
           gS(3,32), gS(10,33), gS(26,16), gS(45,5), gS(47,6), gS(74,10), gS(30,27), gS(28,18),
           gS(-8,15), gS(2,12), gS(10,1), gS(16,0), gS(24,0), gS(24,0), gS(25,2), gS(0,8),
           gS(-20,3), gS(-18,12), gS(-6,0), gS(12,-3), gS(12,-4), gS(12,-3), gS(-1,2), gS(-14,-3),
           gS(-22,-3), gS(-17,0), gS(-4,-5), gS(-3,0), gS(2,-1), gS(4,-3), gS(0,-6), gS(-16,-9),
           gS(-32,5), gS(-15,5), gS(-13,7), gS(1,7), gS(0,10), gS(5,5), gS(-2,1), gS(-22,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-2,11), gS(-2,9), gS(0,6), gS(0,2), gS(0,1), gS(-1,1), gS(0,1), gS(0,6),
           gS(-3,10), gS(-5,14), gS(-6,7), gS(-2,0), gS(-1,2), gS(-4,4), gS(-3,18), gS(-2,13),
           gS(-2,-4), gS(-3,-1), gS(-9,-8), gS(0,-19), gS(-1,-13), gS(-6,-4), gS(-7,0), gS(-1,0),
           gS(0,-14), gS(-3,-9), gS(2,-13), gS(0,-25), gS(0,-20), gS(0,-6), gS(-5,-1), gS(0,0),
           gS(1,-26), gS(-3,-5), gS(-5,-10), gS(-4,0), gS(-6,-8), gS(-6,1), gS(2,-5), gS(0,-26),
           gS(0,-38), gS(-1,-19), gS(-4,-7), gS(-9,-1), gS(-6,0), gS(1,-3), gS(1,-9), gS(0,-44),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-5,6), gS(-4,6), gS(-1,1), gS(0,0), gS(0,0), gS(-2,0), gS(0,0), gS(-1,0),
           gS(-2,9), gS(-6,11), gS(-6,10), gS(-2,0), gS(-1,1), gS(-4,1), gS(-2,16), gS(-3,7),
           gS(0,-3), gS(0,0), gS(-2,-1), gS(1,-17), gS(0,-14), gS(-2,-8), gS(-8,-4), gS(0,0),
           gS(1,8), gS(0,0), gS(3,-7), gS(0,-10), gS(0,-9), gS(0,-8), gS(0,0), gS(0,-3),
           gS(5,0), gS(0,5), gS(0,2), gS(-3,3), gS(-2,5), gS(-2,7), gS(4,-5), gS(9,0),
           gS(6,22), gS(0,13), gS(0,20), gS(-1,17), gS(-3,14), gS(5,17), gS(6,13), gS(10,12),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(28,12), gS(15,21), gS(0,28), gS(0,21), gS(6,20), gS(13,20), gS(27,16), gS(38,14),
           gS(7,15), gS(0,16), gS(17,12), gS(34,4), gS(26,6), gS(28,3), gS(-5,15), gS(11,10),
           gS(4,17), gS(20,9), gS(19,10), gS(27,1), gS(43,-4), gS(38,-2), gS(30,0), gS(-3,12),
           gS(0,13), gS(0,13), gS(18,8), gS(21,1), gS(20,0), gS(20,-1), gS(7,2), gS(0,3),
           gS(-13,9), gS(-11,14), gS(0,9), gS(-2,6), gS(1,3), gS(-2,1), gS(-5,6), gS(-19,4),
           gS(-15,0), gS(-8,0), gS(-4,0), gS(-3,-2), gS(-3,-2), gS(0,-9), gS(1,-9), gS(-12,-10),
           gS(-20,-8), gS(-4,-10), gS(1,-7), gS(1,-10), gS(12,-16), gS(9,-22), gS(6,-23), gS(-39,-11),
           gS(-4,-9), gS(2,-12), gS(3,-8), gS(11,-19), gS(14,-22), gS(14,-20), gS(-7,-10), gS(-3,-24),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-33,-1), gS(-43,-2), gS(-74,16), gS(-81,17), gS(-57,17), gS(-91,13), gS(-25,-7), gS(-28,-15),
           gS(-37,-1), gS(-17,0), gS(-10,0), gS(-12,11), gS(-11,8), gS(-12,3), gS(-25,-4), gS(-23,-7),
           gS(-3,0), gS(2,1), gS(1,3), gS(27,-3), gS(18,0), gS(22,0), gS(8,0), gS(9,-5),
           gS(-7,-2), gS(9,5), gS(5,2), gS(23,8), gS(17,9), gS(13,4), gS(8,0), gS(-1,-10),
           gS(7,-7), gS(0,4), gS(8,4), gS(20,6), gS(23,5), gS(6,3), gS(9,0), gS(8,-17),
           gS(4,-16), gS(21,-5), gS(10,-3), gS(10,6), gS(6,11), gS(21,-8), gS(18,-11), gS(9,-13),
           gS(17,-21), gS(12,-25), gS(16,-10), gS(5,0), gS(14,0), gS(15,-6), gS(21,-21), gS(8,-30),
           gS(-2,-32), gS(8,-7), gS(0,-3), gS(2,0), gS(7,-4), gS(-11,4), gS(0,-9), gS(-1,-25),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-156,-60), gS(-31,-6), gS(-80,8), gS(-13,-3), gS(0,-1), gS(-75,2), gS(-29,-5), gS(-89,-64),
           gS(-28,-8), gS(-14,2), gS(1,0), gS(25,7), gS(12,5), gS(33,-10), gS(-11,-1), gS(-14,-16),
           gS(-10,-1), gS(12,2), gS(15,20), gS(33,8), gS(47,3), gS(47,6), gS(18,-1), gS(13,-13),
           gS(7,-1), gS(5,-1), gS(14,13), gS(30,15), gS(18,17), gS(34,10), gS(11,-3), gS(24,-10),
           gS(1,2), gS(14,5), gS(17,20), gS(23,17), gS(25,20), gS(29,11), gS(26,1), gS(11,0),
           gS(-7,-19), gS(11,-5), gS(10,7), gS(22,19), gS(24,16), gS(21,3), gS(26,-7), gS(3,-8),
           gS(-7,-26), gS(-10,4), gS(3,-4), gS(6,9), gS(12,7), gS(11,-3), gS(2,2), gS(2,-15),
           gS(-40,-33), gS(-13,-16), gS(-2,-6), gS(0,1), gS(8,3), gS(-3,-4), gS(-7,-6), gS(-20,-30),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-13,0), gS(0,11), gS(-3,16), gS(-4,25), gS(-9,30), gS(0,15), gS(-9,22), gS(-11,17),
           gS(-13,1), gS(-28,25), gS(-13,25), gS(-30,52), gS(-47,71), gS(-25,38), gS(-18,42), gS(1,16),
           gS(-9,-15), gS(0,0), gS(-4,23), gS(0,36), gS(-3,46), gS(0,41), gS(5,17), gS(-10,19),
           gS(-3,-17), gS(-7,8), gS(-5,19), gS(-5,40), gS(-9,55), gS(-7,41), gS(-5,31), gS(3,-3),
           gS(2,-27), gS(-5,0), gS(-1,11), gS(-3,38), gS(3,34), gS(-4,11), gS(9,4), gS(-4,-2),
           gS(-1,-36), gS(11,-24), gS(7,-6), gS(5,0), gS(4,0), gS(9,-6), gS(15,-11), gS(5,-27),
           gS(6,-54), gS(12,-57), gS(21,-58), gS(13,-34), gS(16,-30), gS(22,-65), gS(26,-80), gS(1,-45),
           gS(6,-62), gS(9,-70), gS(7,-76), gS(6,-58), gS(9,-64), gS(-3,-75), gS(0,-70), gS(12,-69),
};

/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(83,114), gS(503,683), gS(353,378), gS(376,399), gS(998,1271),
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
