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
           gS(-16,-73), gS(-5,-31), gS(3,-19), gS(9,-8), gS(13,2), gS(15,5), gS(15,8),
           gS(16,7), gS(16,12), gS(18,11), gS(18,8), gS(27,11), gS(39,19), gS(73,4),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-71), gS(7,-29), gS(15,-9), gS(21,-2), gS(27,0),
           gS(30,7), gS(35,2), gS(40,-4), gS(37,4),
};

const int KING_MOBILITY[9] = {
           gS(26,-1), gS(16,-8), gS(10,-1), gS(6,4), gS(1,1),
           gS(-8,1), gS(5,-5), gS(-19,1), gS(-48,-2),
};

const int ROOK_MOBILITY[15] = {
           gS(-26,-85), gS(-12,-56), gS(-6,-37), gS(0,-24), gS(3,-11), gS(1,0), gS(6,3),
           gS(8,3), gS(13,7), gS(15,13), gS(19,15), gS(22,19), gS(25,22), gS(33,20),
           gS(83,-22),
};

const int QUEEN_MOBILITY[28] = {
           gS(-20,-114), gS(-15,-185), gS(-13,-127), gS(-11,-71), gS(-10,-40), gS(-8,-23), gS(-7,-7),
           gS(-5,1), gS(-3,11), gS(-2,17), gS(-1,24), gS(0,29), gS(1,32), gS(1,37),
           gS(2,40), gS(2,44), gS(2,50), gS(2,50), gS(3,50), gS(9,47), gS(13,44),
           gS(17,41), gS(12,41), gS(22,28), gS(12,27), gS(20,23), gS(26,29), gS(24,25),
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
        [PAWN] = gS(78,115),
        [ROOK] = gS(477,651),
        [KNIGHT] = gS(336,355),
        [BISHOP] = gS(356,385),
        [QUEEN] = gS(918,1314),
        [KING] = gS(0, 0)
};

const int TEMPO = 10;
const int KING_HIGH_DANGER = gS(-48,-51);

const int KING_MED_DANGER = gS(-21,-30);

const int KING_LOW_DANGER = gS(-9,-42);

const int BISHOP_PAIR_BONUS = gS(37,52);

const int PAWN_SUPPORTED = gS(14,4);

const int DOUBLED_PAWN_PENALTY = gS(-4,-29);

const int ISOLATED_PAWN_PENALTY = gS(-8,-1);

const int PAWN_BLOCKED = gS(3,16);

const int PASSER_BLOCKED = gS(17,-2);

const int BISHOP_RAMMED_PENALTY = gS(-2,-5);

const int BISHOP_CENTER_CONTROL = gS(12,4);

const int MINOR_BEHIND_PAWN = gS(9,12);

const int MINOR_BEHIND_PASSER = gS(14,2);

const int KING_AHEAD_PASSER = gS(-23,13);

const int KING_EQUAL_PASSER = gS(10,7);

const int KING_BEHIND_PASSER = gS(11,-14);

const int KING_OPEN_FILE = gS(-72,10);

const int KING_OWN_SEMI_FILE = gS(-23,10);

const int KING_ENEMY_SEMI_LINE = gS(-19,9);

const int KING_ATTACK_PAWN = gS(48,27);


const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(6,29), gS(-15,32), gS(-8,15),
           gS(0,-1), gS(3,-2), gS(22,-4), gS(13,-2),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(60,-12), gS(-27,28), gS(-7,-2),
           gS(-9,-11), gS(-2,-20), gS(-4,-20), gS(13,-23),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(42,-14), gS(29,3),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(17,-5), gS(18,10),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,21), gS(-55,-29), gS(-54,-36), gS(-55,-68), gS(-45,-17),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(16,28), gS(16,32), gS(15,30), gS(1,21),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-5,12), gS(61,23), gS(46,29), gS(-1,-7),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(2,-7), gS(9,21), gS(9,11), gS(7,23),
           gS(0,5), gS(-11,-31), gS(0,1), gS(0,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(14,7), gS(15,33), gS(24,7), gS(20,26),
           gS(11,5), gS(-9,-11), gS(-1,5), gS(10,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(5,-1), gS(7,4), gS(9,12),
           gS(19,26), gS(30,49), gS(2,17),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(0,-6), gS(-2,9), gS(-1,43),
           gS(17,51), gS(28,78), gS(56,101), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,20), gS(4,15), gS(3,6), gS(3,-7),
           gS(-5,-3), gS(-5,8), gS(-5,18), gS(11,13),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-5,8), gS(2,5), gS(22,-2),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(8,29), gS(8,67), gS(2,113),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-13,-21), gS(14,0), gS(-2,-26), gS(-13,-12),
           gS(-18,-9), gS(-19,-10), gS(-22,-10), gS(-19,-9),
};

const int KING_PSQT_BLACK[64] = {
           gS(-13,-109), gS(0,-49), gS(0,-33), gS(0,-17), gS(0,-31), gS(0,-26), gS(0,-25), gS(-12,-101),
           gS(-13,-37), gS(0,22), gS(5,21), gS(1,12), gS(1,10), gS(3,19), gS(-1,19), gS(-10,-34),
           gS(-1,-3), gS(27,26), gS(31,28), gS(12,36), gS(14,44), gS(34,26), gS(24,26), gS(-5,-3),
           gS(-9,-16), gS(20,13), gS(23,28), gS(-5,48), gS(0,47), gS(25,27), gS(24,14), gS(-39,-13),
           gS(-23,-33), gS(19,1), gS(13,21), gS(-27,42), gS(-17,37), gS(-8,23), gS(15,3), gS(-52,-19),
           gS(-43,-24), gS(3,0), gS(-13,14), gS(-40,29), gS(-32,27), gS(-31,18), gS(-8,2), gS(-39,-18),
           gS(-9,-28), gS(-8,2), gS(-2,5), gS(-24,16), gS(-24,15), gS(-6,8), gS(-7,1), gS(-14,-26),
           gS(-30,-63), gS(2,-32), gS(-18,-10), gS(-36,-10), gS(8,-38), gS(-19,-18), gS(0,-28), gS(-13,-62),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,63), gS(39,82), gS(52,80), gS(66,69), gS(72,68), gS(71,74), gS(-3,90), gS(-12,76),
           gS(10,38), gS(18,39), gS(30,27), gS(48,17), gS(53,16), gS(80,18), gS(44,30), gS(28,28),
           gS(-5,15), gS(2,17), gS(9,5), gS(24,3), gS(30,3), gS(25,2), gS(13,9), gS(0,7),
           gS(-19,3), gS(-17,8), gS(0,-3), gS(16,-3), gS(15,-3), gS(14,-3), gS(-3,0), gS(-14,-4),
           gS(-19,-4), gS(-20,2), gS(-7,-2), gS(-8,6), gS(-1,5), gS(-2,2), gS(7,-7), gS(-12,-11),
           gS(-22,6), gS(-12,8), gS(-15,14), gS(-5,17), gS(-5,18), gS(13,9), gS(11,3), gS(-11,-5),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(19,16), gS(14,17), gS(-1,20), gS(2,10), gS(7,0), gS(20,4), gS(37,8), gS(45,11),
           gS(0,20), gS(-3,19), gS(13,16), gS(27,9), gS(21,10), gS(42,1), gS(10,9), gS(17,10),
           gS(-2,20), gS(24,5), gS(16,11), gS(29,3), gS(44,-3), gS(49,-5), gS(62,-11), gS(16,4),
           gS(-4,14), gS(4,8), gS(11,11), gS(22,1), gS(20,0), gS(23,-3), gS(19,-3), gS(5,0),
           gS(-15,10), gS(-14,12), gS(-8,12), gS(-3,8), gS(0,6), gS(-4,6), gS(1,1), gS(-15,1),
           gS(-17,-1), gS(-10,-1), gS(-8,0), gS(-5,-1), gS(-1,-1), gS(0,-8), gS(5,-12), gS(-10,-14),
           gS(-24,-8), gS(-10,-10), gS(-2,-7), gS(-2,-8), gS(7,-12), gS(8,-17), gS(7,-23), gS(-39,-12),
           gS(-5,-10), gS(0,-13), gS(0,-6), gS(9,-17), gS(13,-20), gS(16,-21), gS(-6,-12), gS(0,-27),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-44,7), gS(-43,2), gS(-81,13), gS(-81,20), gS(-69,15), gS(-91,8), gS(-16,-4), gS(-44,-2),
           gS(-43,0), gS(-21,3), gS(-11,0), gS(-15,14), gS(-10,10), gS(-5,0), gS(-34,3), gS(-29,-7),
           gS(-1,0), gS(0,4), gS(5,4), gS(11,2), gS(17,2), gS(5,14), gS(13,-2), gS(14,-4),
           gS(-11,-2), gS(13,2), gS(2,2), gS(22,9), gS(19,8), gS(15,0), gS(14,-3), gS(0,-4),
           gS(5,-8), gS(0,0), gS(11,1), gS(22,4), gS(26,0), gS(6,1), gS(11,-1), gS(11,-14),
           gS(1,-16), gS(19,-8), gS(10,-5), gS(13,2), gS(6,9), gS(18,-6), gS(15,-10), gS(8,-14),
           gS(14,-30), gS(13,-25), gS(21,-19), gS(8,-4), gS(18,-5), gS(17,-10), gS(24,-24), gS(10,-41),
           gS(-1,-37), gS(14,-18), gS(0,-6), gS(4,-5), gS(8,-6), gS(-10,0), gS(1,-14), gS(2,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-142,-62), gS(-38,-22), gS(-71,6), gS(-14,-5), gS(0,-3), gS(-60,4), gS(-32,-14), gS(-97,-70),
           gS(-19,-20), gS(-8,-1), gS(14,-1), gS(38,10), gS(27,10), gS(46,-12), gS(-16,-1), gS(-10,-21),
           gS(-13,-11), gS(13,0), gS(27,21), gS(29,26), gS(53,12), gS(40,13), gS(17,-10), gS(7,-16),
           gS(8,-6), gS(11,0), gS(20,18), gS(37,27), gS(27,19), gS(39,22), gS(15,-4), gS(24,-11),
           gS(0,-3), gS(11,4), gS(17,27), gS(26,26), gS(27,27), gS(31,19), gS(25,3), gS(11,-4),
           gS(-14,-22), gS(8,-3), gS(6,11), gS(21,26), gS(23,24), gS(17,7), gS(22,-4), gS(-2,-14),
           gS(-17,-21), gS(-17,0), gS(2,-4), gS(7,9), gS(13,7), gS(10,-3), gS(-2,-1), gS(-2,-8),
           gS(-41,-47), gS(-16,-18), gS(-10,-13), gS(-3,1), gS(5,1), gS(-6,-8), gS(-9,-11), gS(-36,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-15,-5), gS(-7,12), gS(-4,30), gS(-1,44), gS(0,52), gS(23,39), gS(-3,38), gS(-2,22),
           gS(-16,-10), gS(-35,23), gS(-14,21), gS(-40,64), gS(-43,91), gS(-11,58), gS(-36,62), gS(0,30),
           gS(-9,-19), gS(-3,-15), gS(-5,15), gS(-3,41), gS(-1,57), gS(7,66), gS(15,29), gS(0,42),
           gS(-4,-29), gS(-10,-1), gS(-4,6), gS(-9,44), gS(-13,62), gS(-12,60), gS(-9,45), gS(1,18),
           gS(-1,-39), gS(-5,-12), gS(-3,-2), gS(-6,31), gS(0,28), gS(-9,21), gS(5,1), gS(-6,4),
           gS(-1,-53), gS(9,-35), gS(4,-14), gS(2,-15), gS(1,-14), gS(6,-8), gS(14,-25), gS(5,-39),
           gS(2,-63), gS(7,-56), gS(19,-61), gS(10,-40), gS(14,-40), gS(20,-74), gS(21,-77), gS(-1,-54),
           gS(-1,-64), gS(3,-74), gS(3,-75), gS(3,-60), gS(4,-64), gS(-4,-81), gS(3,-83), gS(0,-73),
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
