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
           gS(-24,-103), gS(-13,-43), gS(-7,-17), gS(-3,-1), gS(0,12), gS(1,22), gS(0,30),
           gS(0,32), gS(0,36), gS(4,34), gS(10,32), gS(27,21), gS(44,32), gS(75,-2),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-71), gS(1,-24), gS(5,0), gS(7,14), gS(10,22),
           gS(12,28), gS(16,26), gS(23,18), gS(31,3),
};

const int KING_MOBILITY[9] = {
           gS(10,-2), gS(14,-3), gS(9,0), gS(5,4), gS(0,3),
           gS(-6,1), gS(-1,0), gS(-15,0), gS(-19,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-21,-105), gS(-13,-69), gS(-9,-35), gS(-7,-14), gS(-8,4), gS(-10,18), gS(-4,21),
           gS(0,26), gS(4,31), gS(9,35), gS(11,40), gS(17,44), gS(21,45), gS(36,33),
           gS(89,3),
};

const int QUEEN_MOBILITY[28] = {
           gS(-44,-114), gS(-21,-185), gS(-9,-133), gS(-6,-76), gS(-4,-43), gS(-2,-21), gS(-1,-3),
           gS(0,8), gS(3,17), gS(5,23), gS(6,30), gS(7,34), gS(8,38), gS(7,43),
           gS(6,49), gS(3,53), gS(2,56), gS(2,56), gS(2,57), gS(7,52), gS(12,47),
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
        [PAWN] = gS(68,125),
        [ROOK] = gS(409,765),
        [KNIGHT] = gS(274,422),
        [BISHOP] = gS(301,455),
        [QUEEN] = gS(940,1367),
        [KING] = gS(0, 0)
};

const int TEMPO = 7;
const int KING_HIGH_DANGER = gS(-20,-69);

const int KING_MED_DANGER = gS(-6,-31);

const int KING_LOW_DANGER = gS(4,7);

const int BISHOP_PAIR_BONUS = gS(15,74);

const int PAWN_SUPPORTED = gS(15,5);

const int DOUBLED_PAWN_PENALTY = gS(-7,-20);

const int ISOLATED_PAWN_PENALTY = gS(-4,-10);

const int PAWN_BLOCKED = gS(0,24);

const int PASSER_BLOCKED = gS(6,19);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(9,5);

const int MINOR_BEHIND_PAWN = gS(7,23);

const int MINOR_BEHIND_PASSER = gS(7,15);

const int KING_AHEAD_PASSER = gS(-19,13);

const int KING_EQUAL_PASSER = gS(8,4);

const int KING_BEHIND_PASSER = gS(8,-7);

const int KING_OPEN_FILE = gS(-49,-1);

const int KING_OWN_SEMI_FILE = gS(-30,16);

const int KING_ENEMY_SEMI_LINE = gS(-17,9);

const int KING_ATTACK_PAWN = gS(28,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(5,32), gS(-1,22), gS(-3,7),
           gS(-2,-4), gS(-1,-8), gS(12,-13), gS(1,-24),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(30,36), gS(-16,27), gS(-1,6),
           gS(-1,-6), gS(0,-17), gS(7,-28), gS(17,-35),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(23,5), gS(22,11),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(7,0), gS(13,9),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-2,11), gS(-26,-14), gS(-26,-48), gS(-30,-76), gS(-18,-13),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(0,17), gS(7,35), gS(10,30), gS(0,3),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-5,0), gS(25,16), gS(22,13), gS(-5,6),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-9), gS(1,15), gS(9,12), gS(-7,21),
           gS(-2,7), gS(-1,-30), gS(4,-8), gS(4,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(9,9), gS(-7,26), gS(18,7), gS(0,20),
           gS(9,5), gS(-3,-6), gS(-13,4), gS(7,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,-1), gS(8,2), gS(7,10),
           gS(16,33), gS(12,28), gS(1,4),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-13,-12), gS(-16,1), gS(-10,37),
           gS(20,47), gS(35,76), gS(73,93), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(11,12), gS(7,11), gS(1,-1), gS(0,-5),
           gS(-3,-7), gS(-14,-3), gS(-7,12), gS(8,8),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-8,9), gS(-13,29), gS(17,34),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(0,27), gS(4,68), gS(5,80),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-103), gS(0,-47), gS(0,-33), gS(0,-15), gS(0,-32), gS(0,-27), gS(0,-25), gS(-12,-94),
           gS(-13,-37), gS(0,26), gS(5,20), gS(0,4), gS(0,0), gS(3,16), gS(0,33), gS(-12,-38),
           gS(-2,-7), gS(28,28), gS(34,29), gS(12,18), gS(12,15), gS(36,27), gS(26,30), gS(-7,-12),
           gS(-9,-9), gS(22,14), gS(30,29), gS(0,34), gS(7,28), gS(32,25), gS(28,13), gS(-38,-12),
           gS(-22,-32), gS(24,1), gS(26,21), gS(-14,39), gS(0,34), gS(6,23), gS(28,3), gS(-47,-22),
           gS(-46,-13), gS(-2,1), gS(-3,13), gS(-27,31), gS(-14,27), gS(-13,18), gS(0,1), gS(-32,-14),
           gS(-12,-10), gS(-22,9), gS(-15,14), gS(-54,24), gS(-36,18), gS(-35,20), gS(-22,7), gS(-18,-18),
           gS(-29,-57), gS(-1,-26), gS(-32,-3), gS(-24,-25), gS(-15,-37), gS(-44,-4), gS(-4,-25), gS(-16,-72),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(62,67), gS(42,83), gS(60,82), gS(69,69), gS(77,68), gS(80,73), gS(-4,89), gS(-15,79),
           gS(0,45), gS(25,41), gS(40,35), gS(44,16), gS(53,18), gS(91,26), gS(56,32), gS(13,34),
           gS(-2,19), gS(2,16), gS(4,13), gS(19,-1), gS(28,1), gS(27,5), gS(8,13), gS(0,10),
           gS(-11,4), gS(-8,6), gS(-4,0), gS(2,-2), gS(8,-1), gS(3,1), gS(0,0), gS(-9,-4),
           gS(-17,-1), gS(-19,0), gS(-15,4), gS(-14,5), gS(-7,8), gS(-7,8), gS(-5,-5), gS(-14,-10),
           gS(-13,4), gS(-10,5), gS(-14,16), gS(-6,17), gS(-12,23), gS(8,18), gS(7,2), gS(-9,-13),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(25,27), gS(20,33), gS(0,41), gS(8,36), gS(13,36), gS(25,36), gS(41,31), gS(52,27),
           gS(-4,24), gS(-6,26), gS(15,21), gS(29,21), gS(27,21), gS(45,6), gS(11,19), gS(16,17),
           gS(-9,19), gS(30,4), gS(20,13), gS(42,2), gS(53,-2), gS(55,3), gS(71,-10), gS(18,9),
           gS(-9,12), gS(8,9), gS(16,11), gS(38,4), gS(31,4), gS(28,3), gS(28,1), gS(14,5),
           gS(-20,2), gS(-12,13), gS(-13,14), gS(-3,7), gS(-4,6), gS(-7,9), gS(5,6), gS(-8,-1),
           gS(-24,-15), gS(-7,-3), gS(-15,-3), gS(-7,-8), gS(-7,-7), gS(-8,-6), gS(9,-7), gS(-14,-17),
           gS(-55,-13), gS(-18,-17), gS(-10,-12), gS(-4,-16), gS(-4,-16), gS(-2,-20), gS(-3,-21), gS(-54,-12),
           gS(-16,-16), gS(-10,-14), gS(-6,-11), gS(0,-17), gS(-1,-16), gS(-2,-6), gS(-1,-15), gS(-11,-25),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,6), gS(-43,8), gS(-83,17), gS(-81,23), gS(-70,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-51,3), gS(-23,8), gS(-11,7), gS(-17,14), gS(-11,12), gS(0,6), gS(-32,7), gS(-26,-1),
           gS(0,4), gS(0,9), gS(16,5), gS(4,9), gS(22,7), gS(4,11), gS(21,3), gS(13,0),
           gS(-12,0), gS(14,5), gS(2,6), gS(22,12), gS(19,13), gS(14,3), gS(17,2), gS(-4,1),
           gS(-2,-10), gS(3,-1), gS(10,7), gS(16,8), gS(24,7), gS(2,6), gS(10,0), gS(0,-11),
           gS(-2,-17), gS(10,-8), gS(6,-3), gS(13,3), gS(9,4), gS(10,-4), gS(11,-8), gS(12,-11),
           gS(9,-34), gS(8,-32), gS(10,-15), gS(3,-2), gS(7,-3), gS(8,-14), gS(17,-26), gS(5,-45),
           gS(6,-38), gS(2,-16), gS(1,-2), gS(0,-6), gS(0,-2), gS(0,-6), gS(-1,-10), gS(2,-39),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-63), gS(-37,-20), gS(-69,11), gS(-14,0), gS(0,1), gS(-56,13), gS(-30,-12), gS(-98,-70),
           gS(-17,-22), gS(-13,-6), gS(20,-2), gS(45,17), gS(38,17), gS(48,-11), gS(-21,-3), gS(-11,-21),
           gS(-17,-17), gS(5,0), gS(31,31), gS(32,30), gS(54,21), gS(44,27), gS(24,-6), gS(5,-15),
           gS(3,-10), gS(12,3), gS(29,27), gS(31,40), gS(22,33), gS(43,20), gS(21,3), gS(13,-8),
           gS(-3,-8), gS(11,8), gS(16,31), gS(23,34), gS(27,36), gS(22,32), gS(27,11), gS(7,0),
           gS(-18,-33), gS(2,-5), gS(7,10), gS(14,26), gS(18,24), gS(15,8), gS(8,0), gS(-3,-25),
           gS(-21,-18), gS(-27,-1), gS(-5,-10), gS(7,8), gS(2,10), gS(0,-7), gS(-15,-2), gS(-11,-6),
           gS(-40,-48), gS(-13,-29), gS(-10,-17), gS(-8,4), gS(0,3), gS(-8,-9), gS(-10,-20), gS(-41,-46),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-2,3), gS(0,21), gS(0,38), gS(4,52), gS(11,64), gS(31,49), gS(4,48), gS(13,34),
           gS(-18,-7), gS(-54,29), gS(-16,20), gS(-41,68), gS(-38,101), gS(0,70), gS(-53,66), gS(-18,32),
           gS(-14,-19), gS(-8,-19), gS(-10,12), gS(-10,41), gS(2,60), gS(14,78), gS(34,42), gS(7,56),
           gS(-5,-33), gS(-4,0), gS(-7,1), gS(-15,45), gS(-14,66), gS(-7,71), gS(4,57), gS(9,28),
           gS(-2,-41), gS(2,-13), gS(-4,-7), gS(-11,29), gS(-7,29), gS(-7,26), gS(9,3), gS(2,9),
           gS(-2,-58), gS(7,-40), gS(1,-19), gS(0,-26), gS(0,-25), gS(0,-9), gS(13,-30), gS(3,-43),
           gS(-3,-66), gS(3,-59), gS(12,-75), gS(6,-45), gS(9,-49), gS(13,-85), gS(10,-81), gS(-12,-57),
           gS(-2,-66), gS(1,-78), gS(3,-82), gS(7,-65), gS(7,-71), gS(2,-85), gS(6,-84), gS(-4,-75),
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
