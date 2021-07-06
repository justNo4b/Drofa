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
           gS(-20,-76), gS(-15,-27), gS(-5,-10), gS(0,0), gS(5,7), gS(8,14), gS(10,16),
           gS(12,18), gS(10,23), gS(13,19), gS(15,20), gS(25,14), gS(41,31), gS(73,-3),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-21,-68), gS(-4,-16), gS(5,3), gS(12,7), gS(17,11),
           gS(22,17), gS(29,14), gS(34,9), gS(36,1),
};

const int KING_MOBILITY[9] = {
           gS(18,2), gS(24,-3), gS(15,-1), gS(7,3), gS(-1,2),
           gS(-13,1), gS(-14,3), gS(-29,1), gS(-18,-17),
};

const int ROOK_MOBILITY[15] = {
           gS(-17,-78), gS(-7,-38), gS(-3,-20), gS(0,-10), gS(0,-1), gS(-5,9), gS(0,12),
           gS(1,12), gS(4,17), gS(9,21), gS(10,24), gS(12,30), gS(17,32), gS(25,28),
           gS(64,13),
};

const int QUEEN_MOBILITY[28] = {
           gS(-41,-115), gS(-10,-187), gS(-11,-130), gS(-10,-72), gS(-10,-40), gS(-8,-21), gS(-7,-4),
           gS(-5,5), gS(-3,13), gS(-1,16), gS(-1,23), gS(0,28), gS(2,30), gS(0,36),
           gS(1,41), gS(2,46), gS(2,54), gS(2,55), gS(6,58), gS(13,55), gS(16,52),
           gS(20,50), gS(14,46), gS(24,32), gS(13,28), gS(21,24), gS(26,29), gS(24,26),
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
        [PAWN] = gS(64,115),
        [ROOK] = gS(412,749),
        [KNIGHT] = gS(291,409),
        [BISHOP] = gS(317,433),
        [QUEEN] = gS(922,1361),
        [KING] = gS(0, 0)
};

const int TEMPO = 10;
const int KING_HIGH_DANGER = gS(-38,-73);

const int KING_MED_DANGER = gS(-19,-38);

const int KING_LOW_DANGER = gS(-6,3);

const int BISHOP_PAIR_BONUS = gS(23,67);

const int PAWN_SUPPORTED = gS(17,11);

const int DOUBLED_PAWN_PENALTY = gS(0,-22);

const int ISOLATED_PAWN_PENALTY = gS(-5,-5);

const int PAWN_BLOCKED = gS(3,16);

const int PASSER_BLOCKED = gS(13,6);

const int BISHOP_RAMMED_PENALTY = gS(-3,-6);

const int BISHOP_CENTER_CONTROL = gS(12,4);

const int MINOR_BEHIND_PAWN = gS(7,7);

const int MINOR_BEHIND_PASSER = gS(10,9);

const int KING_AHEAD_PASSER = gS(-23,21);

const int KING_EQUAL_PASSER = gS(10,11);

const int KING_BEHIND_PASSER = gS(7,-11);

const int KING_OPEN_FILE = gS(-83,10);

const int KING_OWN_SEMI_FILE = gS(-28,15);

const int KING_ENEMY_SEMI_LINE = gS(-18,9);

const int KING_ATTACK_PAWN = gS(20,44);


const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(2,40), gS(-4,30), gS(0,8),
           gS(2,-3), gS(5,-7), gS(19,-9), gS(9,-11),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(28,5), gS(-28,25), gS(-5,1),
           gS(-5,-11), gS(0,-21), gS(1,-24), gS(19,-26),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(26,6), gS(17,3),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(11,0), gS(11,6),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-3,12), gS(-43,-20), gS(-43,-32), gS(-47,-67), gS(-39,-16),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(10,27), gS(15,31), gS(12,24), gS(-1,12),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-7,5), gS(47,15), gS(38,14), gS(-2,0),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-10), gS(4,21), gS(9,14), gS(-9,18),
           gS(-1,5), gS(-1,-37), gS(6,-1), gS(-1,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(20,8), gS(15,34), gS(22,7), gS(2,22),
           gS(8,5), gS(0,-11), gS(-2,5), gS(10,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(5,-1), gS(8,4), gS(11,11),
           gS(24,28), gS(27,49), gS(1,13),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(0,-10), gS(0,5), gS(-2,44),
           gS(25,54), gS(30,71), gS(57,100), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(5,25), gS(6,16), gS(4,5), gS(5,-8),
           gS(-3,-8), gS(0,-1), gS(0,13), gS(11,16),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-9,13), gS(-12,39), gS(13,41),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(0,37), gS(-3,99), gS(-4,126),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-13,-21), gS(8,5), gS(-8,-15), gS(-13,-5),
           gS(-16,-2), gS(-16,-8), gS(-21,-11), gS(-19,-11),
};

const int KING_PSQT_BLACK[64] = {
           gS(-12,-104), gS(0,-47), gS(0,-32), gS(0,-14), gS(0,-30), gS(0,-25), gS(0,-22), gS(-11,-94),
           gS(-13,-35), gS(0,23), gS(5,20), gS(1,10), gS(1,8), gS(4,24), gS(0,35), gS(-10,-29),
           gS(-3,-8), gS(27,25), gS(33,30), gS(13,30), gS(14,32), gS(37,36), gS(26,34), gS(-6,-6),
           gS(-10,-16), gS(19,8), gS(26,26), gS(-2,37), gS(3,35), gS(28,27), gS(24,14), gS(-39,-9),
           gS(-24,-34), gS(19,-4), gS(18,16), gS(-22,37), gS(-10,32), gS(-4,18), gS(17,-2), gS(-52,-18),
           gS(-45,-21), gS(0,-3), gS(-9,9), gS(-34,26), gS(-25,24), gS(-28,13), gS(-8,0), gS(-40,-16),
           gS(-13,-22), gS(-17,3), gS(-3,9), gS(-28,19), gS(-24,19), gS(-8,10), gS(-1,-1), gS(-21,-20),
           gS(-32,-62), gS(1,-36), gS(-25,-8), gS(-38,-13), gS(3,-24), gS(-29,-9), gS(-3,-32), gS(-17,-64),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,70), gS(38,80), gS(56,83), gS(67,68), gS(73,66), gS(75,69), gS(-4,88), gS(-16,82),
           gS(9,43), gS(20,46), gS(36,29), gS(41,16), gS(50,15), gS(90,19), gS(61,38), gS(24,38),
           gS(-10,22), gS(0,15), gS(2,9), gS(7,0), gS(29,-1), gS(27,2), gS(11,12), gS(5,5),
           gS(-16,3), gS(-12,6), gS(-7,-1), gS(8,-4), gS(14,-4), gS(9,-3), gS(2,-1), gS(-8,-10),
           gS(-23,-2), gS(-20,0), gS(-11,-1), gS(-12,6), gS(-1,4), gS(-4,0), gS(0,-8), gS(-18,-14),
           gS(-17,6), gS(-12,10), gS(-10,11), gS(-4,18), gS(-4,26), gS(11,10), gS(9,1), gS(-16,-8),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(16,20), gS(13,24), gS(-3,34), gS(1,27), gS(7,24), gS(22,27), gS(38,22), gS(47,20),
           gS(-9,16), gS(-8,23), gS(10,22), gS(23,12), gS(17,10), gS(42,4), gS(14,15), gS(19,12),
           gS(-12,15), gS(24,5), gS(12,9), gS(25,-2), gS(42,-6), gS(47,-8), gS(70,-10), gS(19,0),
           gS(-10,14), gS(3,7), gS(7,11), gS(20,0), gS(15,-6), gS(20,-7), gS(25,-3), gS(9,-3),
           gS(-19,7), gS(-16,11), gS(-13,11), gS(-4,7), gS(-2,5), gS(-12,3), gS(5,1), gS(-8,-2),
           gS(-19,-2), gS(-11,-2), gS(-11,0), gS(-5,-1), gS(0,-2), gS(1,-7), gS(19,-11), gS(-2,-15),
           gS(-26,-2), gS(-12,-6), gS(-1,-2), gS(1,-4), gS(6,-9), gS(5,-13), gS(12,-17), gS(-31,-7),
           gS(-8,-1), gS(-6,-5), gS(-4,1), gS(2,-6), gS(9,-12), gS(0,-4), gS(3,-13), gS(-5,-17),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-49,5), gS(-43,5), gS(-82,15), gS(-82,20), gS(-71,15), gS(-91,11), gS(-15,-2), gS(-50,-3),
           gS(-45,1), gS(-24,1), gS(-12,2), gS(-17,12), gS(-12,7), gS(-4,0), gS(-35,3), gS(-30,-6),
           gS(-1,4), gS(-1,4), gS(5,0), gS(4,2), gS(13,0), gS(6,8), gS(18,-1), gS(20,0),
           gS(-13,0), gS(9,3), gS(0,4), gS(17,13), gS(18,5), gS(15,1), gS(12,-4), gS(-8,-2),
           gS(-2,-7), gS(-2,0), gS(5,4), gS(21,7), gS(24,1), gS(5,1), gS(8,0), gS(12,-12),
           gS(-3,-14), gS(15,-4), gS(8,-2), gS(11,3), gS(13,8), gS(13,0), gS(16,-6), gS(16,-13),
           gS(10,-30), gS(8,-26), gS(18,-19), gS(5,0), gS(15,-1), gS(17,-9), gS(24,-21), gS(9,-40),
           gS(0,-37), gS(17,-15), gS(2,4), gS(2,-2), gS(9,0), gS(-7,8), gS(1,-11), gS(3,-39),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-143,-62), gS(-39,-22), gS(-70,9), gS(-15,-3), gS(0,-1), gS(-58,8), gS(-32,-13), gS(-100,-71),
           gS(-18,-19), gS(-8,-1), gS(15,0), gS(38,11), gS(28,7), gS(47,-11), gS(-17,-1), gS(-10,-21),
           gS(-14,-11), gS(7,1), gS(30,27), gS(27,25), gS(50,15), gS(42,15), gS(20,-9), gS(6,-15),
           gS(3,-5), gS(9,5), gS(22,21), gS(41,30), gS(29,18), gS(47,15), gS(14,0), gS(19,-8),
           gS(-4,-1), gS(8,6), gS(16,29), gS(24,27), gS(29,31), gS(27,23), gS(26,7), gS(8,0),
           gS(-19,-22), gS(-1,0), gS(9,10), gS(13,26), gS(26,24), gS(19,6), gS(19,-1), gS(4,-16),
           gS(-24,-20), gS(-22,-1), gS(-4,-2), gS(9,8), gS(12,5), gS(5,-2), gS(-8,-2), gS(-6,-4),
           gS(-41,-48), gS(-19,-11), gS(-14,-14), gS(-5,3), gS(4,5), gS(-6,-6), gS(-14,-10), gS(-38,-46),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-14,-5), gS(-8,13), gS(-4,33), gS(0,48), gS(2,57), gS(26,44), gS(0,42), gS(3,25),
           gS(-19,-12), gS(-39,24), gS(-17,20), gS(-41,66), gS(-42,96), gS(-6,63), gS(-41,64), gS(-1,33),
           gS(-11,-19), gS(-7,-18), gS(-6,14), gS(-8,41), gS(-2,58), gS(10,71), gS(25,33), gS(7,47),
           gS(-8,-31), gS(-8,-2), gS(-6,4), gS(-17,43), gS(-19,62), gS(-13,63), gS(-4,49), gS(-1,21),
           gS(0,-40), gS(-6,-13), gS(-6,-5), gS(-8,29), gS(-4,28), gS(-11,23), gS(9,2), gS(0,7),
           gS(-1,-55), gS(6,-36), gS(0,-16), gS(0,-17), gS(5,-15), gS(6,-8), gS(22,-26), gS(10,-39),
           gS(1,-64), gS(5,-56), gS(13,-59), gS(14,-40), gS(15,-39), gS(19,-75), gS(21,-76), gS(-4,-55),
           gS(-3,-65), gS(-1,-74), gS(3,-74), gS(6,-57), gS(7,-64), gS(1,-80), gS(5,-83), gS(-2,-74),
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
