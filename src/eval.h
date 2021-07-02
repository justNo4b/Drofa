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
           gS(-21,-69), gS(-15,-26), gS(-5,-11), gS(1,0), gS(6,6), gS(9,13), gS(12,15),
           gS(13,16), gS(12,21), gS(15,17), gS(18,17), gS(26,12), gS(40,30), gS(72,-3),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-21,-67), gS(-4,-18), gS(5,0), gS(12,5), gS(17,9),
           gS(22,16), gS(29,14), gS(34,10), gS(38,3),
};

const int KING_MOBILITY[9] = {
           gS(20,3), gS(25,-4), gS(15,-2), gS(7,2), gS(-1,2),
           gS(-14,1), gS(-15,4), gS(-31,2), gS(-17,-17),
};

const int ROOK_MOBILITY[15] = {
           gS(-18,-71), gS(-7,-35), gS(-2,-21), gS(1,-12), gS(1,-3), gS(-4,7), gS(0,10),
           gS(3,10), gS(6,15), gS(10,19), gS(11,23), gS(13,29), gS(18,30), gS(25,27),
           gS(56,15),
};

const int QUEEN_MOBILITY[28] = {
           gS(-39,-114), gS(-9,-186), gS(-13,-129), gS(-12,-70), gS(-12,-39), gS(-10,-21), gS(-8,-5),
           gS(-7,4), gS(-4,11), gS(-2,14), gS(-2,21), gS(0,27), gS(1,28), gS(0,35),
           gS(0,39), gS(2,44), gS(1,53), gS(1,54), gS(6,57), gS(14,54), gS(17,53),
           gS(23,52), gS(16,48), gS(25,33), gS(14,30), gS(21,24), gS(26,29), gS(24,26),
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
        [PAWN] = gS(63,116),
        [ROOK] = gS(418,742),
        [KNIGHT] = gS(292,405),
        [BISHOP] = gS(323,428),
        [QUEEN] = gS(919,1359),
        [KING] = gS(0, 0)
};

const int TEMPO = 10;
const int KING_HIGH_DANGER = gS(-41,-69);

const int KING_MED_DANGER = gS(-20,-38);

const int KING_LOW_DANGER = gS(-7,2);

const int BISHOP_PAIR_BONUS = gS(22,68);

const int PAWN_SUPPORTED = gS(16,11);

const int DOUBLED_PAWN_PENALTY = gS(-2,-21);

const int ISOLATED_PAWN_PENALTY = gS(-5,-6);

const int PAWN_BLOCKED = gS(3,16);

const int PASSER_BLOCKED = gS(14,3);

const int BISHOP_RAMMED_PENALTY = gS(-4,-5);

const int BISHOP_CENTER_CONTROL = gS(12,5);

const int MINOR_BEHIND_PAWN = gS(11,16);

const int MINOR_BEHIND_PASSER = gS(11,7);

const int KING_AHEAD_PASSER = gS(-24,21);

const int KING_EQUAL_PASSER = gS(10,11);

const int KING_BEHIND_PASSER = gS(8,-11);

const int KING_OPEN_FILE = gS(-84,10);

const int KING_OWN_SEMI_FILE = gS(-28,16);

const int KING_ENEMY_SEMI_LINE = gS(-19,9);

const int KING_ATTACK_PAWN = gS(16,45);


const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(1,40), gS(-5,30), gS(-1,8),
           gS(0,-2), gS(5,-6), gS(21,-8), gS(10,-10),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(30,3), gS(-30,25), gS(-6,1),
           gS(-5,-11), gS(0,-21), gS(1,-25), gS(19,-25),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(27,5), gS(17,2),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(11,0), gS(11,4),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-3,12), gS(-47,-19), gS(-45,-25), gS(-49,-61), gS(-42,-15),
};

const int KNIGHT_PAWN_ADJUSTMENT[9] = {
           gS(0,0), gS(-2,-15), gS(-5,-11), gS(-5,-6),
           gS(3,-1), gS(6,4), gS(5,8), gS(0,9),
           gS(-1,7),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(11,27), gS(15,30), gS(12,23), gS(-1,14),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-7,7), gS(50,11), gS(39,12), gS(-2,-2),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-10), gS(3,22), gS(8,15), gS(-10,18),
           gS(-1,4), gS(-1,-39), gS(5,0), gS(-2,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(21,5), gS(19,34), gS(23,6), gS(3,22),
           gS(8,5), gS(2,-13), gS(0,5), gS(10,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(5,0), gS(8,4), gS(11,11),
           gS(24,27), gS(29,52), gS(1,14),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(2,-8), gS(2,6), gS(-1,45),
           gS(25,53), gS(29,70), gS(50,100), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(3,26), gS(5,16), gS(4,6), gS(6,-8),
           gS(-3,-8), gS(1,-1), gS(0,13), gS(10,16),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-9,12), gS(-12,38), gS(11,41),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(0,38), gS(-5,102), gS(-4,132),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-11,-24), gS(9,3), gS(-6,-16), gS(-12,-6),
           gS(-16,-2), gS(-17,-9), gS(-23,-13), gS(-20,-14),
};

const int KING_PSQT_BLACK[64] = {
           gS(-11,-103), gS(0,-47), gS(0,-31), gS(0,-13), gS(0,-29), gS(0,-24), gS(0,-21), gS(-10,-93),
           gS(-13,-34), gS(0,22), gS(5,20), gS(1,12), gS(1,11), gS(4,27), gS(0,36), gS(-9,-25),
           gS(-3,-9), gS(27,24), gS(32,30), gS(13,33), gS(14,37), gS(37,38), gS(26,34), gS(-5,-4),
           gS(-10,-18), gS(18,6), gS(24,25), gS(-3,37), gS(1,35), gS(26,27), gS(22,14), gS(-40,-8),
           gS(-24,-35), gS(17,-5), gS(15,15), gS(-24,36), gS(-14,32), gS(-7,18), gS(12,-2), gS(-54,-17),
           gS(-45,-23), gS(0,-3), gS(-11,9), gS(-35,26), gS(-27,24), gS(-31,13), gS(-9,0), gS(-43,-15),
           gS(-12,-25), gS(-13,1), gS(2,7), gS(-20,16), gS(-18,17), gS(-2,7), gS(1,-3), gS(-19,-21),
           gS(-33,-63), gS(2,-37), gS(-25,-8), gS(-39,-10), gS(8,-26), gS(-25,-10), gS(-1,-32), gS(-15,-64),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(60,71), gS(37,78), gS(54,84), gS(67,68), gS(71,65), gS(73,68), gS(-4,87), gS(-17,83),
           gS(11,41), gS(19,47), gS(37,28), gS(41,17), gS(50,15), gS(91,17), gS(62,38), gS(26,37),
           gS(-10,22), gS(-1,16), gS(2,9), gS(6,0), gS(29,-1), gS(28,2), gS(12,12), gS(6,5),
           gS(-17,3), gS(-12,6), gS(-7,-1), gS(8,-3), gS(14,-4), gS(9,-3), gS(3,-2), gS(-8,-9),
           gS(-24,-1), gS(-20,0), gS(-12,-1), gS(-12,6), gS(-1,4), gS(-3,0), gS(0,-8), gS(-18,-14),
           gS(-17,6), gS(-13,10), gS(-11,12), gS(-5,17), gS(-4,26), gS(11,10), gS(10,0), gS(-15,-7),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(13,18), gS(11,22), gS(-4,32), gS(0,25), gS(5,21), gS(21,24), gS(37,20), gS(45,18),
           gS(-9,15), gS(-8,22), gS(8,22), gS(22,11), gS(15,9), gS(41,3), gS(15,13), gS(21,10),
           gS(-12,14), gS(22,5), gS(10,9), gS(21,0), gS(39,-5), gS(45,-9), gS(70,-10), gS(20,-1),
           gS(-10,14), gS(2,8), gS(5,12), gS(16,2), gS(13,-5), gS(19,-8), gS(25,-3), gS(9,-4),
           gS(-18,8), gS(-16,11), gS(-12,11), gS(-3,8), gS(0,5), gS(-11,3), gS(6,0), gS(-7,-2),
           gS(-19,0), gS(-11,0), gS(-9,0), gS(-5,0), gS(1,-2), gS(3,-8), gS(22,-13), gS(0,-15),
           gS(-23,-2), gS(-12,-4), gS(0,-1), gS(1,-3), gS(7,-9), gS(6,-12), gS(15,-17), gS(-26,-7),
           gS(-7,-1), gS(-5,-4), gS(-4,1), gS(2,-5), gS(10,-11), gS(0,-3), gS(4,-12), gS(-4,-17),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-48,4), gS(-43,4), gS(-81,14), gS(-82,18), gS(-71,14), gS(-90,9), gS(-15,-3), gS(-51,-4),
           gS(-42,0), gS(-24,0), gS(-12,0), gS(-17,11), gS(-11,5), gS(-5,-2), gS(-36,1), gS(-31,-7),
           gS(-1,4), gS(-1,3), gS(2,0), gS(5,1), gS(11,0), gS(7,6), gS(17,-2), gS(20,0),
           gS(-13,0), gS(9,4), gS(0,4), gS(16,13), gS(19,4), gS(15,1), gS(12,-4), gS(-9,-2),
           gS(-2,-6), gS(-3,1), gS(5,5), gS(22,7), gS(24,0), gS(5,1), gS(7,1), gS(13,-13),
           gS(-3,-12), gS(15,-3), gS(8,-1), gS(11,4), gS(13,9), gS(13,0), gS(16,-5), gS(17,-13),
           gS(10,-28), gS(8,-25), gS(18,-19), gS(5,1), gS(15,0), gS(17,-8), gS(25,-19), gS(10,-38),
           gS(0,-36), gS(19,-15), gS(3,2), gS(2,-1), gS(10,0), gS(-7,9), gS(2,-11), gS(4,-39),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-142,-62), gS(-39,-22), gS(-70,8), gS(-15,-4), gS(0,-1), gS(-58,5), gS(-32,-13), gS(-100,-71),
           gS(-18,-17), gS(-6,0), gS(13,0), gS(35,8), gS(25,4), gS(46,-11), gS(-15,0), gS(-9,-21),
           gS(-13,-9), gS(7,1), gS(29,26), gS(25,23), gS(49,13), gS(42,11), gS(19,-9), gS(7,-14),
           gS(3,-2), gS(8,5), gS(22,20), gS(42,28), gS(29,15), gS(48,14), gS(13,0), gS(20,-8),
           gS(-5,0), gS(7,6), gS(16,28), gS(24,26), gS(29,30), gS(28,20), gS(26,6), gS(8,0),
           gS(-20,-18), gS(-1,0), gS(8,9), gS(13,26), gS(27,23), gS(19,4), gS(19,-1), gS(4,-13),
           gS(-25,-19), gS(-20,-1), gS(-4,0), gS(10,7), gS(12,4), gS(5,0), gS(-6,-2), gS(-6,-3),
           gS(-41,-47), gS(-20,-9), gS(-15,-12), gS(-4,2), gS(5,5), gS(-6,-5), gS(-14,-8), gS(-37,-44),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-18,-7), gS(-11,9), gS(-6,31), gS(-1,46), gS(0,54), gS(24,41), gS(-1,39), gS(0,21),
           gS(-18,-13), gS(-37,21), gS(-18,20), gS(-42,65), gS(-44,94), gS(-8,60), gS(-37,62), gS(3,33),
           gS(-10,-19), gS(-7,-17), gS(-5,15), gS(-8,41), gS(-3,57), gS(9,68), gS(23,30), gS(8,43),
           gS(-8,-29), gS(-8,-2), gS(-6,4), gS(-17,42), gS(-19,60), gS(-13,60), gS(-3,46), gS(0,19),
           gS(0,-39), gS(-7,-11), gS(-6,-4), gS(-7,28), gS(-3,27), gS(-10,22), gS(10,2), gS(0,6),
           gS(0,-54), gS(6,-34), gS(0,-14), gS(0,-14), gS(5,-12), gS(7,-7), gS(23,-25), gS(11,-38),
           gS(2,-63), gS(5,-55), gS(12,-54), gS(14,-38), gS(15,-36), gS(19,-71), gS(23,-75), gS(-1,-54),
           gS(-3,-64), gS(-1,-72), gS(3,-70), gS(6,-53), gS(7,-61), gS(1,-77), gS(5,-82), gS(-1,-73),
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
