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
           gS(-24,-103), gS(-12,-44), gS(-6,-17), gS(-3,-2), gS(0,11), gS(1,22), gS(1,29),
           gS(0,32), gS(1,35), gS(4,34), gS(11,31), gS(27,21), gS(45,30), gS(75,-2),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-71), gS(1,-24), gS(5,0), gS(7,14), gS(10,22),
           gS(12,28), gS(17,27), gS(23,18), gS(32,3),
};

const int KING_MOBILITY[9] = {
           gS(10,-2), gS(14,-5), gS(9,0), gS(5,5), gS(-1,5),
           gS(-6,2), gS(-2,1), gS(-16,0), gS(-17,-11),
};

const int ROOK_MOBILITY[15] = {
           gS(-22,-104), gS(-14,-68), gS(-9,-33), gS(-7,-12), gS(-9,6), gS(-10,19), gS(-4,22),
           gS(0,26), gS(4,30), gS(9,36), gS(11,40), gS(17,42), gS(22,42), gS(36,30),
           gS(89,2),
};

const int QUEEN_MOBILITY[28] = {
           gS(-44,-111), gS(-21,-179), gS(-8,-133), gS(-5,-76), gS(-3,-43), gS(-2,-21), gS(0,-3),
           gS(1,8), gS(3,17), gS(5,23), gS(6,30), gS(8,34), gS(8,38), gS(8,43),
           gS(6,49), gS(4,53), gS(3,56), gS(2,56), gS(3,57), gS(8,52), gS(12,47),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
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
        [PAWN] = gS(68,126),
        [ROOK] = gS(410,756),
        [KNIGHT] = gS(274,418),
        [BISHOP] = gS(302,452),
        [QUEEN] = gS(943,1365),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int KING_HIGH_DANGER = gS(-17,-66);

const int KING_MED_DANGER = gS(-6,-28);

const int KING_LOW_DANGER = gS(1,3);

const int BISHOP_PAIR_BONUS = gS(14,74);

const int PAWN_SUPPORTED = gS(14,5);

const int DOUBLED_PAWN_PENALTY = gS(-6,-22);

const int ISOLATED_PAWN_PENALTY = gS(-4,-10);

const int PAWN_BLOCKED = gS(1,26);

const int PASSER_BLOCKED = gS(1,46);

const int PAWN_CONNECTED = gS(6,4);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(9,6);

const int MINOR_BEHIND_PAWN = gS(6,13);

const int MINOR_BEHIND_PASSER = gS(7,14);

const int KING_AHEAD_PASSER = gS(-18,11);

const int KING_EQUAL_PASSER = gS(6,8);

const int KING_BEHIND_PASSER = gS(8,-5);

const int KING_OPEN_FILE = gS(-49,0);

const int KING_OWN_SEMI_FILE = gS(-30,18);

const int KING_ENEMY_SEMI_LINE = gS(-16,8);

const int KING_ATTACK_PAWN = gS(29,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,35), gS(-2,27), gS(-5,11),
           gS(-3,0), gS(-2,-4), gS(10,-8), gS(0,-18),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(39,44), gS(-13,18), gS(-1,-1),
           gS(-2,-13), gS(0,-24), gS(5,-35), gS(15,-42),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(22,9), gS(22,12),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(6,2), gS(13,10),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-2,11), gS(-26,-14), gS(-26,-49), gS(-30,-76), gS(-18,-13),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(0,17), gS(7,34), gS(10,30), gS(0,2),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-4,0), gS(24,15), gS(21,11), gS(-3,3),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-9), gS(0,16), gS(9,18), gS(-7,26),
           gS(0,9), gS(-3,-29), gS(6,-1), gS(4,4),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(8,6), gS(-6,19), gS(19,7), gS(0,14),
           gS(10,5), gS(-4,-4), gS(-10,5), gS(5,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-27), gS(-15,-15), gS(-8,21),
           gS(16,55), gS(27,120), gS(71,131), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,17), gS(6,16), gS(0,3), gS(-1,0),
           gS(-4,-2), gS(-14,0), gS(-7,16), gS(8,13),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-101), gS(0,-47), gS(0,-33), gS(0,-16), gS(0,-32), gS(0,-27), gS(0,-25), gS(-12,-93),
           gS(-13,-37), gS(0,28), gS(5,21), gS(0,5), gS(0,1), gS(3,17), gS(0,35), gS(-12,-38),
           gS(-2,-7), gS(28,29), gS(34,29), gS(12,18), gS(12,15), gS(35,26), gS(26,29), gS(-7,-14),
           gS(-9,-10), gS(22,12), gS(30,27), gS(0,32), gS(7,26), gS(32,23), gS(28,11), gS(-38,-14),
           gS(-22,-33), gS(23,0), gS(26,19), gS(-14,37), gS(0,32), gS(6,21), gS(27,2), gS(-50,-23),
           gS(-49,-12), gS(-5,3), gS(-4,15), gS(-27,32), gS(-14,27), gS(-14,19), gS(-1,3), gS(-37,-12),
           gS(-11,-9), gS(-21,11), gS(-16,16), gS(-55,26), gS(-37,21), gS(-35,22), gS(-20,8), gS(-17,-16),
           gS(-27,-55), gS(0,-26), gS(-30,-3), gS(-26,-23), gS(-16,-34), gS(-44,-3), gS(-3,-25), gS(-14,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(62,75), gS(43,90), gS(62,87), gS(69,73), gS(79,73), gS(82,78), gS(-6,95), gS(-16,87),
           gS(-1,46), gS(22,43), gS(36,36), gS(40,16), gS(50,17), gS(88,25), gS(53,32), gS(12,34),
           gS(-2,20), gS(3,18), gS(5,15), gS(20,0), gS(28,4), gS(28,9), gS(9,16), gS(1,9),
           gS(-11,4), gS(-7,8), gS(-3,0), gS(3,0), gS(9,0), gS(4,2), gS(1,2), gS(-8,-4),
           gS(-16,-1), gS(-17,0), gS(-13,4), gS(-12,5), gS(-5,8), gS(-5,7), gS(-4,-5), gS(-13,-11),
           gS(-15,2), gS(-13,2), gS(-16,14), gS(-7,15), gS(-14,21), gS(6,16), gS(4,-1), gS(-10,-15),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(27,25), gS(22,31), gS(0,40), gS(8,35), gS(13,35), gS(25,36), gS(41,30), gS(52,27),
           gS(-3,23), gS(-5,26), gS(16,21), gS(29,21), gS(27,22), gS(45,7), gS(11,21), gS(16,17),
           gS(-8,16), gS(30,2), gS(20,12), gS(42,1), gS(53,-4), gS(54,2), gS(70,-11), gS(18,7),
           gS(-8,9), gS(8,7), gS(16,9), gS(38,2), gS(31,2), gS(28,1), gS(28,0), gS(14,2),
           gS(-20,0), gS(-12,11), gS(-14,13), gS(-3,6), gS(-4,5), gS(-7,8), gS(5,5), gS(-8,-3),
           gS(-24,-14), gS(-7,-3), gS(-15,-3), gS(-8,-8), gS(-7,-7), gS(-8,-6), gS(9,-6), gS(-14,-17),
           gS(-55,-8), gS(-18,-15), gS(-11,-9), gS(-5,-14), gS(-4,-14), gS(-2,-18), gS(-3,-19), gS(-53,-9),
           gS(-17,-11), gS(-11,-10), gS(-7,-7), gS(0,-14), gS(-1,-14), gS(-3,-4), gS(-1,-12), gS(-11,-21),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,6), gS(-43,8), gS(-82,17), gS(-81,23), gS(-70,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-50,1), gS(-19,5), gS(-9,7), gS(-15,14), gS(-7,11), gS(0,6), gS(-29,5), gS(-26,-3),
           gS(1,1), gS(0,8), gS(17,3), gS(5,8), gS(22,7), gS(5,9), gS(21,2), gS(14,0),
           gS(-11,0), gS(15,5), gS(2,6), gS(22,12), gS(18,13), gS(14,3), gS(17,2), gS(-4,0),
           gS(-2,-12), gS(3,-1), gS(11,6), gS(16,7), gS(24,6), gS(2,5), gS(11,0), gS(0,-12),
           gS(-2,-16), gS(10,-7), gS(6,-3), gS(13,3), gS(9,4), gS(10,-4), gS(11,-8), gS(12,-11),
           gS(8,-30), gS(8,-30), gS(10,-14), gS(3,0), gS(7,-2), gS(8,-13), gS(17,-25), gS(5,-43),
           gS(6,-36), gS(2,-14), gS(1,-1), gS(0,-5), gS(0,-1), gS(0,-5), gS(-1,-9), gS(2,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-64), gS(-36,-20), gS(-68,11), gS(-14,0), gS(0,1), gS(-56,13), gS(-29,-12), gS(-95,-70),
           gS(-14,-25), gS(-13,-7), gS(25,-4), gS(48,16), gS(38,17), gS(51,-12), gS(-21,-3), gS(-8,-22),
           gS(-17,-19), gS(7,-2), gS(31,30), gS(33,28), gS(57,18), gS(45,25), gS(25,-9), gS(4,-17),
           gS(2,-11), gS(11,3), gS(29,27), gS(30,39), gS(22,32), gS(42,20), gS(21,3), gS(13,-9),
           gS(-4,-9), gS(10,9), gS(16,31), gS(22,34), gS(26,36), gS(21,32), gS(27,11), gS(7,0),
           gS(-19,-33), gS(1,-5), gS(6,11), gS(13,26), gS(17,24), gS(14,8), gS(7,0), gS(-4,-25),
           gS(-22,-16), gS(-28,0), gS(-6,-8), gS(6,9), gS(1,11), gS(0,-6), gS(-16,-2), gS(-12,-3),
           gS(-40,-46), gS(-13,-27), gS(-11,-16), gS(-8,5), gS(0,3), gS(-8,-9), gS(-11,-20), gS(-41,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-2,3), gS(0,21), gS(0,38), gS(4,51), gS(11,61), gS(31,49), gS(5,48), gS(13,36),
           gS(-18,-7), gS(-55,32), gS(-17,23), gS(-41,69), gS(-40,104), gS(0,70), gS(-54,69), gS(-19,35),
           gS(-15,-19), gS(-8,-19), gS(-11,13), gS(-10,41), gS(2,60), gS(13,78), gS(33,42), gS(5,57),
           gS(-5,-33), gS(-3,0), gS(-7,1), gS(-15,45), gS(-14,65), gS(-7,70), gS(4,57), gS(8,28),
           gS(-2,-41), gS(2,-13), gS(-3,-9), gS(-10,28), gS(-7,27), gS(-6,25), gS(10,3), gS(3,8),
           gS(-2,-58), gS(8,-41), gS(2,-19), gS(1,-26), gS(1,-25), gS(0,-9), gS(14,-31), gS(4,-45),
           gS(-3,-66), gS(3,-59), gS(12,-74), gS(7,-44), gS(10,-48), gS(14,-85), gS(10,-82), gS(-12,-57),
           gS(-1,-66), gS(2,-78), gS(4,-82), gS(7,-64), gS(8,-71), gS(3,-85), gS(6,-84), gS(-3,-75),
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
