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
           gS(-12,-66), gS(-1,-27), gS(5,-14), gS(10,-4), gS(11,5), gS(13,9), gS(13,13),
           gS(14,14), gS(14,16), gS(16,15), gS(18,13), gS(30,8), gS(38,24), gS(71,-2),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-6,-76), gS(7,-33), gS(14,-7), gS(19,2), gS(24,9),
           gS(28,15), gS(32,13), gS(37,8), gS(40,0),
};

const int KING_MOBILITY[9] = {
           gS(20,-4), gS(11,-2), gS(6,3), gS(2,6), gS(-2,4),
           gS(-9,2), gS(-5,2), gS(-13,-1), gS(-10,-18),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-87), gS(-10,-52), gS(-5,-32), gS(1,-17), gS(3,-4), gS(1,7), gS(6,9),
           gS(8,10), gS(12,16), gS(14,20), gS(17,25), gS(20,28), gS(24,27), gS(32,26),
           gS(58,15),
};

const int QUEEN_MOBILITY[28] = {
           gS(-17,-114), gS(-12,-181), gS(-11,-123), gS(-9,-68), gS(-8,-40), gS(-6,-23), gS(-5,-9),
           gS(-3,0), gS(-2,11), gS(-1,19), gS(0,25), gS(0,30), gS(1,34), gS(1,38),
           gS(1,42), gS(2,45), gS(1,50), gS(3,49), gS(4,50), gS(9,48), gS(12,43),
           gS(17,42), gS(12,42), gS(23,30), gS(11,26), gS(19,22), gS(24,27), gS(22,24),
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
        [PAWN] = gS(76,109),
        [ROOK] = gS(467,699),
        [KNIGHT] = gS(329,386),
        [BISHOP] = gS(351,418),
        [QUEEN] = gS(944,1336),
        [KING] = gS(0, 0)
};

const int TEMPO = 10;
const int KING_HIGH_DANGER = gS(-47,-66);

const int KING_MED_DANGER = gS(-19,-42);

const int KING_LOW_DANGER = gS(17,-20);

const int BISHOP_PAIR_BONUS = gS(33,51);

const int PAWN_SUPPORTED = gS(13,7);

const int DOUBLED_PAWN_PENALTY = gS(-7,-19);

const int ISOLATED_PAWN_PENALTY = gS(-6,-5);

const int PAWN_BLOCKED = gS(3,15);

const int PASSER_BLOCKED = gS(11,1);

const int BISHOP_RAMMED_PENALTY = gS(-2,-7);

const int BISHOP_CENTER_CONTROL = gS(10,8);

const int BISHOP_GLORIFIED_PAWN = gS(-4,-6);

const int MINOR_BEHIND_PAWN = gS(8,1);

const int MINOR_BEHIND_PASSER = gS(13,3);

const int KING_AHEAD_PASSER = gS(-23,16);

const int KING_EQUAL_PASSER = gS(9,7);

const int KING_BEHIND_PASSER = gS(8,-9);

const int KING_OPEN_FILE = gS(-61,7);

const int KING_OWN_SEMI_FILE = gS(-24,13);

const int KING_ENEMY_SEMI_LINE = gS(-18,9);

const int KING_ATTACK_PAWN = gS(18,47);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(1,36), gS(-5,25), gS(-2,6),
           gS(0,-2), gS(0,-1), gS(17,-4), gS(11,-5),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(29,15), gS(-15,19), gS(-3,0),
           gS(-5,-11), gS(1,-22), gS(0,-24), gS(12,-23),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(32,-5), gS(31,0),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(15,-5), gS(21,11),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-59,-34), gS(-49,-40), gS(-51,-67), gS(-44,-19),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(13,33), gS(14,35), gS(14,30), gS(0,26),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-6,14), gS(61,21), gS(42,34), gS(-1,-4),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-4), gS(6,20), gS(6,9), gS(3,23),
           gS(-2,5), gS(-12,-25), gS(-1,1), gS(0,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(12,5), gS(12,31), gS(20,7), gS(25,26),
           gS(12,5), gS(-11,-10), gS(-3,3), gS(10,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,0), gS(6,5), gS(9,11),
           gS(18,26), gS(33,54), gS(2,20),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(0,-7), gS(-3,8), gS(-2,47),
           gS(15,55), gS(29,72), gS(63,80), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(8,21), gS(5,11), gS(2,3), gS(0,-8),
           gS(-4,-6), gS(-3,3), gS(0,13), gS(8,15),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-4,9), gS(3,26), gS(32,30),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(10,30), gS(16,67), gS(10,128),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-8,-21), gS(13,7), gS(-3,-14), gS(-13,-2),
           gS(-17,0), gS(-20,-3), gS(-22,-5), gS(-19,-5),
};

const int KING_PSQT_BLACK[64] = {
           gS(-10,-103), gS(0,-45), gS(0,-30), gS(0,-15), gS(0,-30), gS(0,-25), gS(0,-21), gS(-9,-94),
           gS(-11,-32), gS(0,24), gS(5,23), gS(1,15), gS(1,9), gS(4,25), gS(0,29), gS(-8,-27),
           gS(-2,-8), gS(27,22), gS(31,30), gS(11,31), gS(14,32), gS(35,31), gS(24,26), gS(-5,-6),
           gS(-10,-15), gS(18,11), gS(21,23), gS(-3,35), gS(0,32), gS(24,22), gS(22,13), gS(-40,-13),
           gS(-23,-27), gS(17,2), gS(10,16), gS(-24,31), gS(-15,27), gS(-11,18), gS(11,2), gS(-54,-18),
           gS(-43,-20), gS(3,0), gS(-14,12), gS(-40,25), gS(-36,25), gS(-31,16), gS(-9,2), gS(-41,-16),
           gS(-11,-20), gS(-6,2), gS(-2,6), gS(-22,16), gS(-24,16), gS(-3,8), gS(-9,2), gS(-15,-23),
           gS(-29,-56), gS(1,-31), gS(-18,-11), gS(-33,-10), gS(10,-38), gS(-15,-19), gS(0,-28), gS(-12,-61),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,68), gS(41,81), gS(51,72), gS(67,65), gS(73,63), gS(69,64), gS(-2,87), gS(-7,79),
           gS(8,34), gS(14,35), gS(26,24), gS(47,15), gS(50,15), gS(75,16), gS(38,29), gS(29,23),
           gS(-7,15), gS(1,16), gS(8,6), gS(21,5), gS(26,6), gS(22,4), gS(12,10), gS(-1,9),
           gS(-19,3), gS(-17,8), gS(0,0), gS(14,-1), gS(13,0), gS(13,-1), gS(-2,0), gS(-14,-2),
           gS(-19,-5), gS(-19,0), gS(-7,-1), gS(-7,7), gS(-1,6), gS(-1,2), gS(7,-10), gS(-12,-11),
           gS(-22,5), gS(-10,4), gS(-15,14), gS(-4,17), gS(-5,19), gS(13,7), gS(11,-1), gS(-11,-6),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(20,16), gS(14,22), gS(-1,29), gS(1,22), gS(8,21), gS(21,21), gS(34,15), gS(42,13),
           gS(1,19), gS(-2,18), gS(13,15), gS(27,8), gS(22,10), gS(40,2), gS(6,12), gS(14,10),
           gS(0,19), gS(22,7), gS(16,12), gS(28,3), gS(44,-2), gS(47,-3), gS(51,-7), gS(9,8),
           gS(-2,15), gS(1,13), gS(13,11), gS(21,3), gS(21,2), gS(23,0), gS(13,1), gS(1,4),
           gS(-15,10), gS(-12,14), gS(-6,13), gS(-3,8), gS(0,6), gS(-2,6), gS(-1,5), gS(-17,4),
           gS(-17,0), gS(-9,0), gS(-5,1), gS(-5,0), gS(-3,0), gS(0,-6), gS(3,-8), gS(-12,-10),
           gS(-22,-9), gS(-7,-9), gS(-1,-6), gS(-1,-9), gS(8,-13), gS(8,-19), gS(5,-22), gS(-38,-11),
           gS(-5,-12), gS(0,-14), gS(2,-10), gS(10,-21), gS(14,-24), gS(14,-22), gS(-6,-12), gS(-2,-26),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-40,4), gS(-43,1), gS(-80,15), gS(-81,18), gS(-68,17), gS(-91,12), gS(-16,-3), gS(-41,-4),
           gS(-41,1), gS(-19,0), gS(-10,0), gS(-14,14), gS(-9,10), gS(-7,0), gS(-32,-1), gS(-26,-5),
           gS(-1,0), gS(0,2), gS(3,2), gS(15,2), gS(19,3), gS(4,2), gS(10,-3), gS(13,-4),
           gS(-10,-1), gS(12,3), gS(1,4), gS(23,8), gS(17,8), gS(14,1), gS(12,-2), gS(0,-6),
           gS(7,-5), gS(0,2), gS(10,1), gS(21,4), gS(25,1), gS(6,2), gS(10,0), gS(9,-14),
           gS(0,-15), gS(18,-7), gS(10,-6), gS(12,2), gS(6,8), gS(19,-8), gS(14,-9), gS(7,-11),
           gS(17,-26), gS(13,-25), gS(20,-14), gS(9,-1), gS(17,-3), gS(17,-8), gS(23,-23), gS(9,-37),
           gS(0,-34), gS(12,-13), gS(1,-1), gS(4,-1), gS(9,-4), gS(-7,3), gS(1,-11), gS(3,-33),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-143,-63), gS(-37,-19), gS(-71,8), gS(-13,-2), gS(0,0), gS(-61,5), gS(-30,-11), gS(-96,-69),
           gS(-20,-16), gS(-8,0), gS(9,0), gS(36,11), gS(25,7), gS(43,-11), gS(-14,0), gS(-10,-19),
           gS(-11,-5), gS(14,2), gS(24,21), gS(30,18), gS(54,8), gS(38,9), gS(14,-7), gS(10,-12),
           gS(9,-2), gS(10,0), gS(18,17), gS(34,22), gS(25,19), gS(35,13), gS(14,-4), gS(24,-10),
           gS(1,0), gS(12,7), gS(16,24), gS(25,21), gS(25,27), gS(29,18), gS(25,4), gS(11,-2),
           gS(-12,-18), gS(8,-3), gS(7,8), gS(20,23), gS(22,20), gS(17,5), gS(22,-4), gS(-2,-9),
           gS(-10,-21), gS(-13,3), gS(3,-5), gS(8,8), gS(13,7), gS(11,-2), gS(0,0), gS(0,-10),
           gS(-40,-45), gS(-13,-17), gS(-5,-9), gS(0,3), gS(6,4), gS(-5,-5), gS(-8,-9), gS(-33,-43),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-14,-3), gS(-5,13), gS(-5,29), gS(-2,42), gS(-2,49), gS(20,37), gS(-5,36), gS(-6,19),
           gS(-13,-7), gS(-32,25), gS(-13,22), gS(-38,63), gS(-43,90), gS(-13,56), gS(-29,63), gS(0,28),
           gS(-9,-18), gS(0,-12), gS(-6,16), gS(0,42), gS(0,56), gS(5,62), gS(10,28), gS(-3,39),
           gS(-3,-27), gS(-11,0), gS(-4,8), gS(-7,42), gS(-13,61), gS(-10,58), gS(-9,44), gS(2,15),
           gS(0,-37), gS(-5,-9), gS(-3,0), gS(-6,33), gS(0,27), gS(-8,21), gS(5,2), gS(-6,3),
           gS(-1,-50), gS(9,-33), gS(4,-13), gS(3,-13), gS(2,-11), gS(7,-9), gS(13,-21), gS(5,-37),
           gS(3,-61), gS(9,-56), gS(19,-63), gS(11,-38), gS(15,-39), gS(21,-73), gS(22,-77), gS(0,-52),
           gS(2,-63), gS(6,-73), gS(5,-75), gS(5,-59), gS(6,-64), gS(-4,-80), gS(2,-81), gS(4,-71),
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
