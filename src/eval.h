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
           gS(-24,-103), gS(-12,-44), gS(-6,-18), gS(-3,-2), gS(0,11), gS(1,22), gS(1,29),
           gS(0,32), gS(1,36), gS(4,35), gS(10,32), gS(27,22), gS(45,31), gS(75,-1),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-71), gS(1,-24), gS(4,0), gS(7,14), gS(10,22),
           gS(12,28), gS(16,27), gS(23,18), gS(32,4),
};

const int KING_MOBILITY[9] = {
           gS(10,-2), gS(14,-6), gS(9,0), gS(5,4), gS(-1,5),
           gS(-6,2), gS(-1,1), gS(-16,0), gS(-17,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-22,-104), gS(-14,-69), gS(-9,-34), gS(-7,-12), gS(-9,5), gS(-10,19), gS(-4,22),
           gS(0,26), gS(4,31), gS(9,36), gS(11,40), gS(17,43), gS(22,43), gS(37,31),
           gS(89,2),
};

const int QUEEN_MOBILITY[28] = {
           gS(-44,-112), gS(-21,-181), gS(-9,-133), gS(-6,-76), gS(-4,-43), gS(-2,-21), gS(0,-3),
           gS(1,8), gS(3,17), gS(5,23), gS(6,30), gS(8,34), gS(8,38), gS(7,43),
           gS(6,49), gS(4,53), gS(3,56), gS(2,56), gS(2,57), gS(8,52), gS(12,47),
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
        [PAWN] = gS(68,124),
        [ROOK] = gS(410,760),
        [KNIGHT] = gS(274,420),
        [BISHOP] = gS(301,453),
        [QUEEN] = gS(942,1365),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int KING_HIGH_DANGER = gS(-18,-67);

const int KING_MED_DANGER = gS(-6,-29);

const int KING_LOW_DANGER = gS(2,4);

const int BISHOP_PAIR_BONUS = gS(14,74);

const int PAWN_SUPPORTED = gS(14,5);

const int DOUBLED_PAWN_PENALTY = gS(-7,-20);

const int ISOLATED_PAWN_PENALTY = gS(-4,-10);

const int PAWN_BLOCKED = gS(1,25);

const int PASSER_BLOCKED = gS(2,34);

const int PAWN_CONNECTED = gS(6,4);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(10,6);

const int MINOR_BEHIND_PAWN = gS(7,22);

const int MINOR_BEHIND_PASSER = gS(7,14);

const int KING_AHEAD_PASSER = gS(-18,12);

const int KING_EQUAL_PASSER = gS(6,7);

const int KING_BEHIND_PASSER = gS(8,-6);

const int KING_OPEN_FILE = gS(-49,0);

const int KING_OWN_SEMI_FILE = gS(-29,15);

const int KING_ENEMY_SEMI_LINE = gS(-17,9);

const int KING_ATTACK_PAWN = gS(28,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,34), gS(-2,24), gS(-5,9),
           gS(-2,-2), gS(-1,-7), gS(11,-11), gS(0,-20),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(35,44), gS(-14,21), gS(-1,2),
           gS(-1,-10), gS(0,-21), gS(6,-32), gS(16,-40),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(22,7), gS(21,11),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(7,0), gS(12,9),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-2,11), gS(-26,-13), gS(-26,-49), gS(-30,-76), gS(-18,-13),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(0,17), gS(7,34), gS(10,30), gS(0,3),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-4,0), gS(25,16), gS(21,12), gS(-4,5),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-10), gS(0,15), gS(9,17), gS(-7,25),
           gS(0,8), gS(-2,-30), gS(6,-2), gS(3,4),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(8,7), gS(-7,21), gS(19,7), gS(0,16),
           gS(10,5), gS(-4,-5), gS(-10,5), gS(6,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-10,-22), gS(-13,-6), gS(-7,25),
           gS(18,54), gS(27,100), gS(67,116), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(11,14), gS(6,14), gS(0,0), gS(-1,-3),
           gS(-4,-4), gS(-14,0), gS(-7,14), gS(8,10),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-4,6), gS(-8,0), gS(-5,6),
           gS(-6,15), gS(1,44), gS(14,37),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-102), gS(0,-47), gS(0,-33), gS(0,-15), gS(0,-32), gS(0,-27), gS(0,-25), gS(-12,-93),
           gS(-13,-37), gS(0,28), gS(5,21), gS(0,5), gS(0,1), gS(3,17), gS(0,35), gS(-12,-38),
           gS(-2,-7), gS(28,29), gS(34,29), gS(12,18), gS(12,15), gS(35,26), gS(26,29), gS(-7,-14),
           gS(-9,-10), gS(22,12), gS(30,28), gS(0,32), gS(7,27), gS(32,24), gS(28,11), gS(-38,-13),
           gS(-22,-33), gS(23,0), gS(26,20), gS(-14,37), gS(0,33), gS(6,21), gS(27,2), gS(-49,-23),
           gS(-48,-12), gS(-4,3), gS(-4,14), gS(-27,31), gS(-14,27), gS(-13,19), gS(0,3), gS(-35,-12),
           gS(-12,-9), gS(-22,10), gS(-16,15), gS(-55,25), gS(-37,20), gS(-35,21), gS(-21,8), gS(-18,-17),
           gS(-28,-55), gS(0,-26), gS(-31,-3), gS(-25,-24), gS(-16,-35), gS(-44,-4), gS(-3,-25), gS(-15,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,72), gS(42,87), gS(61,85), gS(68,71), gS(78,71), gS(81,76), gS(-6,93), gS(-16,85),
           gS(0,45), gS(23,41), gS(37,34), gS(41,14), gS(51,15), gS(89,24), gS(54,31), gS(13,32),
           gS(-1,20), gS(3,18), gS(5,16), gS(20,0), gS(28,4), gS(28,9), gS(9,16), gS(1,10),
           gS(-10,5), gS(-7,8), gS(-3,1), gS(3,0), gS(9,0), gS(4,2), gS(1,1), gS(-8,-3),
           gS(-16,0), gS(-17,0), gS(-13,4), gS(-12,5), gS(-5,8), gS(-5,7), gS(-4,-5), gS(-13,-10),
           gS(-14,3), gS(-12,3), gS(-16,15), gS(-7,15), gS(-14,21), gS(6,16), gS(4,-1), gS(-10,-14),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(27,28), gS(22,32), gS(0,41), gS(8,36), gS(13,35), gS(25,36), gS(41,30), gS(52,28),
           gS(-3,24), gS(-5,26), gS(16,22), gS(29,22), gS(27,22), gS(45,7), gS(11,21), gS(16,18),
           gS(-8,16), gS(30,2), gS(20,12), gS(42,1), gS(53,-4), gS(54,2), gS(70,-11), gS(18,7),
           gS(-8,9), gS(8,7), gS(16,10), gS(38,2), gS(31,2), gS(28,1), gS(28,0), gS(14,2),
           gS(-20,0), gS(-12,11), gS(-13,13), gS(-3,6), gS(-4,5), gS(-7,8), gS(5,5), gS(-8,-3),
           gS(-24,-14), gS(-7,-3), gS(-15,-3), gS(-8,-8), gS(-7,-7), gS(-8,-6), gS(9,-6), gS(-14,-17),
           gS(-56,-9), gS(-18,-15), gS(-11,-10), gS(-5,-14), gS(-4,-14), gS(-2,-18), gS(-3,-19), gS(-53,-9),
           gS(-17,-14), gS(-11,-11), gS(-7,-9), gS(0,-16), gS(-1,-15), gS(-3,-4), gS(-2,-13), gS(-11,-22),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,6), gS(-43,8), gS(-82,17), gS(-81,23), gS(-70,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-50,2), gS(-21,6), gS(-10,7), gS(-15,14), gS(-9,11), gS(0,6), gS(-30,6), gS(-26,-2),
           gS(1,2), gS(0,8), gS(16,4), gS(4,8), gS(22,7), gS(4,10), gS(21,2), gS(14,0),
           gS(-11,0), gS(15,5), gS(2,6), gS(22,13), gS(18,13), gS(14,3), gS(17,2), gS(-4,0),
           gS(-2,-11), gS(3,-1), gS(11,6), gS(17,8), gS(24,7), gS(2,6), gS(11,0), gS(0,-11),
           gS(-2,-16), gS(10,-7), gS(6,-3), gS(13,3), gS(9,4), gS(10,-5), gS(11,-8), gS(12,-11),
           gS(8,-31), gS(8,-31), gS(10,-15), gS(3,-1), gS(7,-3), gS(8,-14), gS(17,-25), gS(5,-43),
           gS(6,-37), gS(2,-14), gS(1,-1), gS(0,-5), gS(0,-1), gS(0,-5), gS(-1,-9), gS(2,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-63), gS(-36,-20), gS(-68,11), gS(-14,0), gS(0,1), gS(-56,13), gS(-29,-12), gS(-96,-70),
           gS(-15,-24), gS(-13,-7), gS(23,-3), gS(47,16), gS(38,17), gS(50,-12), gS(-21,-3), gS(-9,-22),
           gS(-17,-19), gS(6,-1), gS(31,30), gS(32,29), gS(55,19), gS(44,25), gS(24,-8), gS(4,-17),
           gS(3,-11), gS(11,3), gS(29,27), gS(30,39), gS(22,33), gS(42,20), gS(21,3), gS(13,-9),
           gS(-3,-9), gS(11,9), gS(16,31), gS(22,34), gS(27,36), gS(21,32), gS(27,11), gS(7,0),
           gS(-19,-33), gS(1,-5), gS(6,11), gS(14,26), gS(18,24), gS(14,8), gS(7,0), gS(-4,-25),
           gS(-21,-16), gS(-28,0), gS(-6,-9), gS(6,8), gS(2,10), gS(0,-6), gS(-16,-2), gS(-11,-4),
           gS(-40,-46), gS(-13,-27), gS(-11,-16), gS(-8,5), gS(0,3), gS(-8,-9), gS(-10,-20), gS(-41,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-2,3), gS(0,21), gS(0,38), gS(4,52), gS(11,62), gS(31,49), gS(4,48), gS(13,35),
           gS(-18,-7), gS(-54,31), gS(-16,22), gS(-41,69), gS(-39,103), gS(0,70), gS(-54,68), gS(-19,34),
           gS(-14,-19), gS(-8,-19), gS(-11,12), gS(-10,41), gS(2,60), gS(13,78), gS(33,42), gS(5,56),
           gS(-5,-33), gS(-3,0), gS(-7,1), gS(-15,45), gS(-14,65), gS(-7,70), gS(4,57), gS(8,28),
           gS(-2,-41), gS(2,-13), gS(-3,-8), gS(-10,28), gS(-7,28), gS(-7,25), gS(10,3), gS(2,8),
           gS(-2,-58), gS(7,-40), gS(2,-19), gS(1,-26), gS(1,-25), gS(0,-9), gS(14,-30), gS(4,-44),
           gS(-3,-66), gS(3,-59), gS(12,-74), gS(6,-44), gS(10,-48), gS(13,-85), gS(10,-81), gS(-12,-57),
           gS(-1,-66), gS(1,-78), gS(3,-82), gS(7,-64), gS(7,-71), gS(2,-85), gS(6,-84), gS(-3,-75),
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
