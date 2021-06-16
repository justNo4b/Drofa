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
           gS(-24,-103), gS(-12,-44), gS(-6,-17), gS(-2,-2), gS(0,11), gS(2,22), gS(2,29),
           gS(0,31), gS(1,34), gS(4,32), gS(10,29), gS(26,19), gS(45,28), gS(75,-4),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-71), gS(2,-25), gS(5,0), gS(8,14), gS(10,22),
           gS(13,28), gS(18,27), gS(24,19), gS(34,4),
};

const int KING_MOBILITY[9] = {
           gS(10,-2), gS(14,-6), gS(9,0), gS(5,5), gS(-1,5),
           gS(-7,2), gS(-2,2), gS(-17,0), gS(-16,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-103), gS(-14,-67), gS(-9,-33), gS(-7,-12), gS(-9,6), gS(-10,19), gS(-4,23),
           gS(0,26), gS(4,31), gS(9,36), gS(11,40), gS(17,42), gS(22,42), gS(36,29),
           gS(88,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-43,-108), gS(-21,-175), gS(-8,-132), gS(-5,-76), gS(-3,-43), gS(-1,-21), gS(0,-3),
           gS(1,8), gS(4,17), gS(6,23), gS(7,30), gS(8,34), gS(9,38), gS(8,43),
           gS(7,49), gS(5,53), gS(3,56), gS(3,57), gS(3,57), gS(9,53), gS(12,48),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
};


/**
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
  const int COUNT_TO_POWER[8] = {0, 0, 40, 65, 80, 87, 95, 100};
  const int SAFE_SHIELD_SAFETY[2][9] = {
      {-50, -50, -50, 0, 0, 0, 0, 0, 0},
      {-50, -50, -50, 0, 0, 0, 0, 0, 0},
  };
  /**@}*/


/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(68,125),
        [ROOK] = gS(411,754),
        [KNIGHT] = gS(275,415),
        [BISHOP] = gS(302,448),
        [QUEEN] = gS(946,1366),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int KING_HIGH_DANGER = gS(-14,-58);

const int KING_MED_DANGER = gS(-6,-23);

const int KING_LOW_DANGER = gS(0,3);

const int BISHOP_PAIR_BONUS = gS(15,76);

const int PAWN_SUPPORTED = gS(14,5);

const int DOUBLED_PAWN_PENALTY = gS(-6,-22);

const int ISOLATED_PAWN_PENALTY = gS(-4,-10);

const int PAWN_BLOCKED = gS(1,26);

const int PASSER_BLOCKED = gS(1,46);

const int PAWN_CONNECTED = gS(6,4);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(10,7);

const int MINOR_BEHIND_PAWN = gS(6,13);

const int MINOR_BEHIND_PASSER = gS(7,14);

const int KING_AHEAD_PASSER = gS(-18,11);

const int KING_EQUAL_PASSER = gS(6,8);

const int KING_BEHIND_PASSER = gS(8,-5);

const int KING_OPEN_FILE = gS(-49,-1);

const int KING_OWN_SEMI_FILE = gS(-30,18);

const int KING_ENEMY_SEMI_LINE = gS(-16,8);

const int KING_ATTACK_PAWN = gS(25,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,35), gS(-2,26), gS(-5,11),
           gS(-4,0), gS(-3,-4), gS(10,-9), gS(0,-18),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(41,43), gS(-14,18), gS(-2,-1),
           gS(-2,-13), gS(0,-24), gS(5,-35), gS(14,-42),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(22,10), gS(22,11),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(6,3), gS(13,9),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-3,11), gS(-26,-14), gS(-26,-49), gS(-31,-76), gS(-18,-13),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-5), gS(0,17), gS(10,17), gS(-4,22),
           gS(0,9), gS(-4,-21), gS(5,4), gS(4,2),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(8,4), gS(-5,11), gS(19,5), gS(2,8),
           gS(9,3), gS(-4,-1), gS(-9,3), gS(3,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-27), gS(-15,-14), gS(-8,21),
           gS(16,55), gS(26,119), gS(71,130), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,17), gS(6,16), gS(0,3), gS(-1,-1),
           gS(-4,-2), gS(-14,0), gS(-7,16), gS(8,12),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-99), gS(0,-47), gS(0,-33), gS(0,-16), gS(0,-32), gS(0,-27), gS(0,-25), gS(-12,-91),
           gS(-13,-37), gS(0,28), gS(5,21), gS(0,5), gS(0,1), gS(3,17), gS(0,36), gS(-12,-37),
           gS(-2,-6), gS(27,29), gS(33,29), gS(12,18), gS(12,15), gS(35,26), gS(26,29), gS(-7,-13),
           gS(-9,-10), gS(22,12), gS(30,27), gS(0,32), gS(7,26), gS(32,23), gS(28,11), gS(-38,-13),
           gS(-22,-33), gS(22,0), gS(26,19), gS(-14,37), gS(0,32), gS(7,21), gS(25,2), gS(-52,-22),
           gS(-51,-12), gS(-7,3), gS(-6,15), gS(-28,31), gS(-16,27), gS(-16,19), gS(-6,4), gS(-43,-9),
           gS(-6,-10), gS(-15,7), gS(-18,16), gS(-58,26), gS(-40,20), gS(-38,22), gS(-17,7), gS(-14,-17),
           gS(-25,-53), gS(3,-26), gS(-27,-4), gS(-27,-22), gS(-16,-35), gS(-47,-3), gS(0,-25), gS(-11,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(62,75), gS(44,91), gS(63,87), gS(70,74), gS(79,73), gS(82,79), gS(-7,97), gS(-17,87),
           gS(-2,47), gS(22,44), gS(36,37), gS(40,17), gS(50,17), gS(87,26), gS(53,33), gS(11,35),
           gS(-2,20), gS(3,18), gS(4,16), gS(19,1), gS(28,4), gS(28,9), gS(8,16), gS(1,10),
           gS(-11,5), gS(-8,8), gS(-4,1), gS(3,0), gS(9,1), gS(4,3), gS(1,2), gS(-9,-3),
           gS(-16,0), gS(-17,0), gS(-14,4), gS(-12,5), gS(-5,8), gS(-5,8), gS(-4,-4), gS(-13,-10),
           gS(-15,3), gS(-12,3), gS(-16,15), gS(-7,15), gS(-14,21), gS(5,17), gS(4,0), gS(-10,-14),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,26), gS(23,32), gS(0,42), gS(8,37), gS(13,37), gS(25,37), gS(41,31), gS(52,27),
           gS(-1,23), gS(-4,27), gS(18,22), gS(30,22), gS(27,23), gS(45,8), gS(11,21), gS(17,18),
           gS(-8,16), gS(31,2), gS(21,13), gS(43,2), gS(54,-3), gS(53,3), gS(70,-10), gS(17,8),
           gS(-8,9), gS(8,7), gS(16,9), gS(37,2), gS(30,3), gS(28,1), gS(28,0), gS(13,2),
           gS(-20,0), gS(-12,11), gS(-14,12), gS(-4,5), gS(-5,5), gS(-7,8), gS(5,4), gS(-9,-3),
           gS(-25,-14), gS(-8,-3), gS(-16,-3), gS(-8,-9), gS(-8,-8), gS(-9,-6), gS(9,-6), gS(-15,-17),
           gS(-55,-9), gS(-18,-15), gS(-12,-10), gS(-5,-14), gS(-4,-15), gS(-2,-18), gS(-3,-20), gS(-53,-9),
           gS(-17,-12), gS(-11,-10), gS(-7,-8), gS(0,-15), gS(-1,-15), gS(-3,-4), gS(-1,-13), gS(-11,-21),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,5), gS(-43,8), gS(-81,17), gS(-81,23), gS(-69,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-49,1), gS(-13,4), gS(-8,7), gS(-14,15), gS(-4,13), gS(0,6), gS(-27,5), gS(-26,-4),
           gS(1,1), gS(0,8), gS(20,3), gS(7,8), gS(23,7), gS(7,9), gS(21,2), gS(14,0),
           gS(-11,0), gS(17,6), gS(3,6), gS(22,12), gS(19,13), gS(15,4), gS(18,3), gS(-5,0),
           gS(-1,-12), gS(4,0), gS(10,7), gS(17,7), gS(24,6), gS(2,6), gS(12,0), gS(1,-12),
           gS(-2,-16), gS(10,-7), gS(5,-4), gS(11,2), gS(7,4), gS(10,-5), gS(10,-7), gS(13,-10),
           gS(8,-30), gS(7,-32), gS(9,-15), gS(2,-2), gS(6,-4), gS(7,-15), gS(16,-26), gS(4,-42),
           gS(5,-37), gS(1,-15), gS(0,-2), gS(0,-6), gS(0,-2), gS(0,-7), gS(-2,-10), gS(1,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-65), gS(-35,-20), gS(-67,11), gS(-14,0), gS(0,1), gS(-56,13), gS(-28,-12), gS(-93,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-3), gS(50,17), gS(38,17), gS(52,-11), gS(-21,-3), gS(-7,-21),
           gS(-17,-18), gS(11,-1), gS(32,30), gS(35,28), gS(61,19), gS(45,24), gS(27,-8), gS(3,-17),
           gS(2,-11), gS(10,4), gS(28,26), gS(29,38), gS(22,31), gS(42,20), gS(20,4), gS(12,-8),
           gS(-4,-9), gS(10,8), gS(15,30), gS(21,32), gS(24,35), gS(20,32), gS(26,12), gS(7,0),
           gS(-20,-33), gS(0,-5), gS(5,10), gS(12,25), gS(17,23), gS(13,8), gS(7,0), gS(-5,-25),
           gS(-22,-16), gS(-28,0), gS(-7,-10), gS(5,7), gS(1,9), gS(-1,-9), gS(-16,-3), gS(-12,-3),
           gS(-40,-46), gS(-13,-27), gS(-11,-17), gS(-9,4), gS(0,2), gS(-8,-11), gS(-11,-20), gS(-41,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-2,3), gS(0,21), gS(0,36), gS(4,49), gS(10,58), gS(31,49), gS(7,50), gS(15,38),
           gS(-18,-5), gS(-56,34), gS(-17,25), gS(-41,71), gS(-41,106), gS(0,70), gS(-54,71), gS(-19,38),
           gS(-15,-18), gS(-8,-18), gS(-12,13), gS(-11,41), gS(1,60), gS(12,78), gS(32,44), gS(4,59),
           gS(-5,-33), gS(-3,0), gS(-8,1), gS(-15,44), gS(-14,64), gS(-7,70), gS(4,58), gS(8,30),
           gS(-1,-41), gS(2,-13), gS(-3,-9), gS(-10,27), gS(-6,26), gS(-6,24), gS(10,3), gS(3,8),
           gS(-1,-58), gS(8,-42), gS(3,-20), gS(1,-27), gS(2,-26), gS(0,-9), gS(14,-31), gS(4,-46),
           gS(-2,-66), gS(4,-60), gS(13,-75), gS(7,-45), gS(10,-49), gS(14,-87), gS(11,-82), gS(-11,-57),
           gS(0,-66), gS(2,-78), gS(4,-83), gS(8,-65), gS(8,-71), gS(3,-85), gS(7,-84), gS(-2,-75),
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
   inline int evaluateKING(const Board &, Color, const evalBits &);

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
