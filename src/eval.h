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

/**
 * @brief Weights for each piece used to calculate the game phase based off
 * remaining material
 */
const int PHASE_WEIGHTS[6] = {
    [PAWN] = 0,
    [ROOK] = 2,
    [KNIGHT] = 1,
    [BISHOP] = 1,
    [QUEEN] = 4,
    [KING] = 0
};

/**
 * @brief Weighted sum of the values in PHASE_WEIGHTS used for calculating
 * a tapered evaluation score
 *
 * This is the sum of each item in detail::PHASE_WEIGHTS with each value multiplied
 * by how many of those pieces are initially on the board (Multiply the
 * value for pawns by 16, knights by 4, etc.).
 */
extern int PHASE_WEIGHT_SUM;
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-24,-102), gS(-13,-42), gS(-6,-16), gS(-3,-1), gS(0,12), gS(1,23), gS(1,30),
           gS(0,33), gS(1,36), gS(4,34), gS(10,32), gS(27,21), gS(44,33), gS(75,-2),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-71), gS(1,-24), gS(5,0), gS(7,14), gS(10,22),
           gS(12,28), gS(17,26), gS(23,18), gS(32,3),
};

const int KING_MOBILITY[9] = {
           gS(10,-2), gS(14,-3), gS(9,0), gS(5,4), gS(0,3),
           gS(-6,1), gS(-1,0), gS(-15,0), gS(-20,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-22,-105), gS(-13,-69), gS(-9,-35), gS(-7,-14), gS(-8,4), gS(-9,18), gS(-4,21),
           gS(0,26), gS(4,31), gS(9,35), gS(12,40), gS(17,44), gS(21,45), gS(36,33),
           gS(89,3),
};

const int QUEEN_MOBILITY[28] = {
           gS(-44,-115), gS(-22,-187), gS(-10,-133), gS(-7,-76), gS(-5,-43), gS(-3,-21), gS(-1,-3),
           gS(0,8), gS(3,17), gS(5,23), gS(6,30), gS(7,34), gS(8,38), gS(7,43),
           gS(6,49), gS(4,53), gS(2,56), gS(2,56), gS(2,57), gS(7,52), gS(12,47),
           gS(10,42), gS(9,40), gS(21,28), gS(11,24), gS(20,23), gS(26,29), gS(24,26),
};


/**
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int SAFE_SHIELD_SAFETY[2][9] = {
      {-50, -50, -50, 0, 0, 0, 0, 0, 0},
      {-50, -50, -50, 0, 0, 0, 0, 0, 0},
  };
  /**@}*/


/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(70,126),
        [ROOK] = gS(410,767),
        [KNIGHT] = gS(274,424),
        [BISHOP] = gS(302,456),
        [QUEEN] = gS(940,1368),
        [KING] = gS(0, 0)
};

const int TEMPO = 10;
const int KING_HIGH_DANGER = gS(-21,-69);

const int KING_MED_DANGER = gS(-6,-32);

const int KING_LOW_DANGER = gS(5,8);

const int BISHOP_PAIR_BONUS = gS(15,73);

const int PAWN_SUPPORTED = gS(15,5);

const int DOUBLED_PAWN_PENALTY = gS(-7,-20);

const int ISOLATED_PAWN_PENALTY = gS(-4,-8);

const int PAWN_BLOCKED = gS(0,25);

const int PASSER_BLOCKED = gS(6,18);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(10,5);

const int MINOR_BEHIND_PAWN = gS(6,13);

const int MINOR_BEHIND_PASSER = gS(7,15);

const int KING_AHEAD_PASSER = gS(-19,13);

const int KING_EQUAL_PASSER = gS(9,5);

const int KING_BEHIND_PASSER = gS(9,-6);

const int KING_OPEN_FILE = gS(-49,0);

const int KING_OWN_SEMI_FILE = gS(-30,16);

const int KING_ENEMY_SEMI_LINE = gS(-17,9);

const int KING_ATTACK_PAWN = gS(28,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(7,33), gS(1,24), gS(0,9),
           gS(0,-1), gS(0,-5), gS(15,-10), gS(4,-21),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(29,34), gS(-18,25), gS(-3,3),
           gS(-3,-9), gS(-2,-19), gS(4,-30), gS(14,-37),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(24,4), gS(22,10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(7,0), gS(13,10),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-2,12), gS(-29,-17), gS(-31,-51), gS(-35,-78), gS(-22,-14),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(0,19), gS(8,36), gS(10,31), gS(0,4),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-5,0), gS(30,20), gS(25,17), gS(-5,7),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-9), gS(1,14), gS(9,10), gS(-7,20),
           gS(-2,6), gS(-1,-30), gS(4,-9), gS(4,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(9,10), gS(-7,27), gS(18,7), gS(-1,21),
           gS(8,5), gS(-3,-6), gS(-13,3), gS(8,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,-1), gS(8,2), gS(7,9),
           gS(16,33), gS(17,36), gS(1,7),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-9), gS(-14,4), gS(-7,39),
           gS(23,50), gS(39,76), gS(78,94), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(14,14), gS(8,11), gS(3,1), gS(2,-2),
           gS(-1,-4), gS(-11,0), gS(-6,12), gS(11,11),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-8,10), gS(-12,30), gS(18,31),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(0,27), gS(0,69), gS(0,92),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-8,-12), gS(1,12), gS(-8,-5), gS(-8,0),
           gS(-8,0), gS(-6,-1), gS(-8,-3), gS(-8,-2),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-104), gS(0,-47), gS(0,-33), gS(0,-15), gS(0,-32), gS(0,-27), gS(0,-25), gS(-12,-94),
           gS(-13,-37), gS(0,25), gS(5,19), gS(0,3), gS(0,0), gS(3,16), gS(0,33), gS(-12,-38),
           gS(-2,-6), gS(28,28), gS(34,29), gS(12,18), gS(12,15), gS(36,28), gS(26,30), gS(-7,-12),
           gS(-9,-9), gS(22,14), gS(30,30), gS(0,34), gS(7,29), gS(32,26), gS(28,13), gS(-38,-12),
           gS(-22,-31), gS(24,2), gS(26,22), gS(-14,39), gS(0,35), gS(6,23), gS(28,3), gS(-47,-22),
           gS(-45,-14), gS(-1,0), gS(-2,13), gS(-27,31), gS(-14,27), gS(-13,18), gS(0,1), gS(-31,-15),
           gS(-12,-11), gS(-22,8), gS(-15,13), gS(-54,24), gS(-36,18), gS(-35,20), gS(-21,6), gS(-19,-19),
           gS(-29,-58), gS(-1,-27), gS(-32,-3), gS(-24,-26), gS(-15,-37), gS(-43,-5), gS(-4,-26), gS(-16,-73),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(63,66), gS(42,84), gS(60,81), gS(69,69), gS(77,68), gS(80,72), gS(-3,89), gS(-14,78),
           gS(0,45), gS(26,42), gS(40,35), gS(44,17), gS(53,18), gS(91,26), gS(56,33), gS(13,34),
           gS(-2,19), gS(3,16), gS(4,12), gS(19,-2), gS(28,0), gS(27,4), gS(8,13), gS(0,9),
           gS(-11,4), gS(-8,7), gS(-4,-1), gS(2,-3), gS(8,-2), gS(3,0), gS(0,0), gS(-9,-5),
           gS(-18,-1), gS(-19,0), gS(-16,3), gS(-14,4), gS(-7,7), gS(-8,7), gS(-6,-4), gS(-14,-11),
           gS(-13,3), gS(-10,5), gS(-15,16), gS(-6,16), gS(-12,23), gS(7,18), gS(7,2), gS(-10,-13),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(25,28), gS(20,33), gS(0,41), gS(8,36), gS(13,36), gS(25,36), gS(41,31), gS(52,27),
           gS(-4,24), gS(-5,26), gS(15,21), gS(29,21), gS(27,21), gS(45,6), gS(11,19), gS(16,17),
           gS(-8,19), gS(30,4), gS(21,14), gS(43,3), gS(53,-2), gS(55,3), gS(71,-9), gS(18,10),
           gS(-8,12), gS(9,9), gS(17,12), gS(38,4), gS(32,4), gS(29,3), gS(28,1), gS(14,5),
           gS(-20,2), gS(-12,13), gS(-13,15), gS(-3,7), gS(-4,6), gS(-6,9), gS(5,6), gS(-9,-1),
           gS(-24,-15), gS(-7,-3), gS(-15,-3), gS(-7,-8), gS(-7,-7), gS(-8,-7), gS(9,-7), gS(-14,-17),
           gS(-55,-14), gS(-18,-18), gS(-10,-13), gS(-4,-17), gS(-4,-17), gS(-2,-21), gS(-2,-22), gS(-54,-13),
           gS(-17,-17), gS(-10,-14), gS(-6,-11), gS(0,-18), gS(-1,-17), gS(-3,-7), gS(-2,-16), gS(-11,-25),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,6), gS(-43,8), gS(-83,17), gS(-81,23), gS(-70,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-51,3), gS(-23,8), gS(-11,7), gS(-17,14), gS(-12,12), gS(0,6), gS(-32,7), gS(-26,-1),
           gS(0,4), gS(0,9), gS(16,6), gS(4,9), gS(22,8), gS(4,12), gS(21,3), gS(13,0),
           gS(-11,0), gS(15,5), gS(2,6), gS(23,13), gS(19,13), gS(14,4), gS(17,2), gS(-4,1),
           gS(-2,-10), gS(3,-1), gS(11,7), gS(17,8), gS(24,7), gS(2,6), gS(11,0), gS(0,-11),
           gS(-3,-17), gS(10,-8), gS(6,-3), gS(13,3), gS(8,4), gS(10,-4), gS(11,-9), gS(12,-11),
           gS(9,-35), gS(8,-32), gS(10,-16), gS(3,-2), gS(6,-3), gS(8,-14), gS(17,-27), gS(5,-46),
           gS(6,-38), gS(2,-16), gS(0,-2), gS(0,-6), gS(0,-2), gS(-1,-6), gS(-1,-11), gS(2,-39),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-63), gS(-37,-20), gS(-69,11), gS(-14,0), gS(0,0), gS(-56,13), gS(-30,-12), gS(-99,-70),
           gS(-17,-21), gS(-13,-6), gS(20,-1), gS(45,18), gS(38,17), gS(48,-10), gS(-21,-3), gS(-11,-20),
           gS(-17,-16), gS(5,0), gS(32,31), gS(32,30), gS(54,22), gS(44,27), gS(24,-6), gS(5,-15),
           gS(3,-10), gS(12,3), gS(30,27), gS(31,40), gS(23,33), gS(43,20), gS(22,3), gS(13,-8),
           gS(-3,-8), gS(12,8), gS(17,31), gS(24,34), gS(28,36), gS(22,32), gS(28,11), gS(7,0),
           gS(-19,-33), gS(1,-5), gS(6,10), gS(15,26), gS(18,24), gS(15,8), gS(8,0), gS(-4,-25),
           gS(-21,-19), gS(-27,-1), gS(-5,-10), gS(7,8), gS(2,10), gS(0,-7), gS(-15,-2), gS(-11,-6),
           gS(-40,-49), gS(-13,-29), gS(-11,-17), gS(-8,4), gS(0,3), gS(-8,-10), gS(-11,-20), gS(-41,-47),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-2,3), gS(0,21), gS(0,38), gS(4,52), gS(10,64), gS(31,49), gS(4,48), gS(13,34),
           gS(-17,-7), gS(-53,28), gS(-15,20), gS(-40,68), gS(-37,101), gS(0,70), gS(-52,66), gS(-17,31),
           gS(-14,-19), gS(-7,-19), gS(-9,12), gS(-9,41), gS(2,60), gS(15,78), gS(34,42), gS(6,55),
           gS(-5,-33), gS(-4,0), gS(-7,1), gS(-14,46), gS(-14,66), gS(-7,71), gS(5,57), gS(9,28),
           gS(-2,-41), gS(2,-13), gS(-4,-7), gS(-11,30), gS(-7,29), gS(-7,26), gS(9,3), gS(2,9),
           gS(-3,-58), gS(6,-40), gS(1,-19), gS(0,-25), gS(0,-24), gS(-1,-9), gS(13,-30), gS(2,-43),
           gS(-3,-66), gS(2,-59), gS(12,-75), gS(6,-45), gS(9,-49), gS(13,-85), gS(10,-81), gS(-12,-57),
           gS(-2,-66), gS(1,-78), gS(3,-82), gS(6,-65), gS(7,-71), gS(2,-85), gS(5,-84), gS(-4,-75),
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
 * @brief Returns a numeric representation of the given board's phase based
 * off remaining material
 *
 * The returned score will range from 0 - Eval::MAX_PHASE.
 *
 * @return A numeric representation of the game phase
 */
int getPhase(const Board &);

/**
 * @brief Use this number as an upper bound on the numerical representation of
 * the game phase when performing a tapered evaluation
 */
const int MAX_PHASE = 256;

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

  /**@}*/

evalBits Setupbits(const Board &);

/**
 * @brief This function analyses king shield safety.
 * it returns simple overall score gS() and
 * adjust base safety value for some types of shields
 */
inline int kingShieldSafety(const Board &, Color, int, evalBits *);

/**
 * @brief This function takes number of each pieceType count for each
 * side and (assuming best play) returns if the position is deadDraw
 *
 * Returns true is position is drawn, returns false if there is some play left.
 * Based on Vice function.
 *
 */
inline bool IsItDeadDraw (int w_N, int w_B, int w_R, int w_Q,
                          int b_N, int b_B, int b_R, int b_Q);


/**
 * @brief Function evaluate piece-pawns interactions for given color
 * Includes:
 * 1. Blocked Pawns
 * 2. Minors shielded by pawns
 * 3. Threats by pawn push
 */
inline int PiecePawnInteraction(const Board &, Color, evalBits &);

/**
 * @brief Set value for a MATERIAL_VALUES_TUNABLE array
 * which is used for optuna tuning
 */
void SetupTuning(int phase, PieceType piece, int value);

};

#endif
