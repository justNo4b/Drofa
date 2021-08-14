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
    U64 AttackedByKing[2];
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
           gS(-13,-66), gS(-2,-26), gS(4,-13), gS(10,-2), gS(13,7), gS(15,10), gS(16,14),
           gS(16,14), gS(17,15), gS(19,14), gS(20,13), gS(31,5), gS(38,22), gS(70,-5),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-5,-78), gS(8,-32), gS(15,-6), gS(20,3), gS(25,10),
           gS(29,16), gS(33,14), gS(38,8), gS(42,0),
};

const int KING_MOBILITY[9] = {
           gS(18,-6), gS(10,-3), gS(5,2), gS(1,5), gS(-3,4),
           gS(-10,3), gS(-4,3), gS(-10,0), gS(-6,-16),
};

const int ROOK_MOBILITY[15] = {
           gS(-22,-88), gS(-10,-52), gS(-5,-30), gS(1,-16), gS(3,-3), gS(1,8), gS(7,10),
           gS(8,11), gS(12,16), gS(15,20), gS(17,25), gS(20,28), gS(24,26), gS(31,25),
           gS(56,15),
};

const int QUEEN_MOBILITY[28] = {
           gS(-17,-114), gS(-12,-179), gS(-10,-121), gS(-9,-67), gS(-7,-40), gS(-6,-24), gS(-4,-9),
           gS(-2,0), gS(-1,11), gS(0,20), gS(0,26), gS(1,31), gS(2,35), gS(2,38),
           gS(2,42), gS(3,46), gS(2,50), gS(4,49), gS(5,49), gS(9,47), gS(12,42),
           gS(17,40), gS(12,42), gS(23,29), gS(11,26), gS(19,22), gS(24,27), gS(22,24),
};

/**
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int UNCONTESTED_KING_ATTACK [6] = {
      -70, -20, 0, 100, 150, 200
  };
  const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  /**@}*/


/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(76,113),
        [ROOK] = gS(472,702),
        [KNIGHT] = gS(333,386),
        [BISHOP] = gS(354,417),
        [QUEEN] = gS(955,1334),
        [KING] = gS(0, 0)
};

const int TEMPO = 10;
const int KING_HIGH_DANGER = gS(-49,-68);

const int KING_MED_DANGER = gS(-20,-43);

const int KING_LOW_DANGER = gS(29,-22);

const int BISHOP_PAIR_BONUS = gS(33,52);

const int PAWN_SUPPORTED = gS(12,8);

const int DOUBLED_PAWN_PENALTY = gS(-7,-21);

const int ISOLATED_PAWN_PENALTY = gS(-6,-5);

const int PAWN_BLOCKED = gS(3,14);

const int PASSER_BLOCKED = gS(11,0);

const int BISHOP_RAMMED_PENALTY = gS(-2,-6);

const int BISHOP_CENTER_CONTROL = gS(10,8);

const int MINOR_BEHIND_PAWN = gS(8,2);

const int MINOR_BEHIND_PASSER = gS(13,1);

const int KING_AHEAD_PASSER = gS(-23,16);

const int KING_EQUAL_PASSER = gS(10,7);

const int KING_BEHIND_PASSER = gS(6,-8);

const int KING_OPEN_FILE = gS(-62,7);

const int KING_OWN_SEMI_FILE = gS(-24,14);

const int KING_ENEMY_SEMI_LINE = gS(-17,8);

const int KING_ATTACK_PAWN = gS(21,50);


const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(1,37), gS(-5,26), gS(-2,7),
           gS(0,-1), gS(0,-1), gS(16,-4), gS(11,-4),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(-35,-3), gS(14,-19), gS(3,0),
           gS(5,11), gS(0,21), gS(1,22), gS(-11,21),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(32,-4), gS(32,0),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(14,-3), gS(22,12),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-62,-36), gS(-48,-42), gS(-51,-67), gS(-45,-20),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(12,33), gS(14,35), gS(13,30), gS(0,26),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-6,16), gS(62,21), gS(41,37), gS(-2,-4),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-2), gS(6,20), gS(6,8), gS(2,23),
           gS(-3,5), gS(-13,-23), gS(-2,2), gS(0,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(11,5), gS(11,30), gS(20,7), gS(25,26),
           gS(12,5), gS(-12,-10), gS(-3,3), gS(10,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,0), gS(6,6), gS(9,11),
           gS(19,26), gS(34,55), gS(3,22),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(0,-8), gS(-3,7), gS(-1,46),
           gS(15,55), gS(28,77), gS(61,86), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(8,23), gS(3,13), gS(2,3), gS(0,-9),
           gS(-4,-7), gS(-2,3), gS(0,14), gS(7,16),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-3,10), gS(5,25), gS(36,34),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(11,29), gS(21,63), gS(13,127),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-9,-21), gS(14,4), gS(-1,-18), gS(-12,-5),
           gS(-16,-5), gS(-21,-8), gS(-22,-10), gS(-20,-8),
};

const int KING_PSQT_BLACK[64] = {
           gS(-10,-103), gS(0,-45), gS(0,-29), gS(0,-14), gS(0,-29), gS(0,-24), gS(0,-21), gS(-9,-94),
           gS(-11,-31), gS(0,27), gS(5,25), gS(1,18), gS(1,11), gS(4,28), gS(0,31), gS(-8,-27),
           gS(-2,-6), gS(27,24), gS(31,33), gS(12,35), gS(14,36), gS(36,34), gS(24,28), gS(-4,-5),
           gS(-10,-14), gS(19,13), gS(21,24), gS(-4,37), gS(-1,34), gS(23,24), gS(22,14), gS(-40,-13),
           gS(-23,-27), gS(16,1), gS(9,16), gS(-24,31), gS(-15,28), gS(-12,19), gS(9,2), gS(-56,-18),
           gS(-43,-22), gS(5,0), gS(-15,12), gS(-38,25), gS(-34,25), gS(-29,15), gS(-6,0), gS(-42,-18),
           gS(-11,-24), gS(-4,-2), gS(-2,4), gS(-22,14), gS(-23,14), gS(-3,6), gS(-8,0), gS(-16,-25),
           gS(-30,-57), gS(1,-33), gS(-18,-13), gS(-34,-12), gS(11,-42), gS(-14,-21), gS(-1,-30), gS(-14,-64),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(60,69), gS(41,82), gS(50,72), gS(67,64), gS(72,64), gS(67,64), gS(-2,88), gS(-6,81),
           gS(7,36), gS(14,38), gS(26,26), gS(48,14), gS(50,14), gS(74,17), gS(37,31), gS(29,24),
           gS(-7,17), gS(1,18), gS(7,7), gS(21,6), gS(26,7), gS(22,5), gS(11,11), gS(0,10),
           gS(-19,3), gS(-16,9), gS(0,0), gS(15,0), gS(14,0), gS(14,-1), gS(-2,1), gS(-13,-3),
           gS(-18,-5), gS(-18,0), gS(-6,-1), gS(-7,7), gS(0,6), gS(0,2), gS(8,-10), gS(-12,-12),
           gS(-21,5), gS(-11,5), gS(-14,15), gS(-3,17), gS(-5,19), gS(14,7), gS(11,-1), gS(-11,-6),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(21,16), gS(15,23), gS(-1,30), gS(1,22), gS(8,22), gS(21,21), gS(33,16), gS(41,14),
           gS(3,18), gS(-1,18), gS(15,15), gS(29,8), gS(23,10), gS(39,2), gS(4,12), gS(13,10),
           gS(1,20), gS(22,8), gS(17,12), gS(28,2), gS(44,-3), gS(47,-4), gS(48,-6), gS(6,9),
           gS(-1,15), gS(1,13), gS(15,11), gS(21,3), gS(21,1), gS(23,0), gS(12,1), gS(0,4),
           gS(-14,10), gS(-12,15), gS(-5,12), gS(-2,7), gS(0,5), gS(-2,5), gS(-2,5), gS(-18,5),
           gS(-17,0), gS(-9,1), gS(-5,1), gS(-5,-1), gS(-3,-1), gS(0,-7), gS(2,-8), gS(-12,-10),
           gS(-21,-8), gS(-6,-9), gS(-1,-6), gS(0,-10), gS(9,-14), gS(9,-20), gS(6,-23), gS(-38,-11),
           gS(-5,-10), gS(1,-13), gS(2,-8), gS(11,-21), gS(15,-24), gS(14,-21), gS(-7,-11), gS(-2,-25),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-39,4), gS(-43,0), gS(-80,16), gS(-82,18), gS(-67,18), gS(-91,12), gS(-17,-4), gS(-38,-5),
           gS(-40,0), gS(-18,0), gS(-10,0), gS(-14,13), gS(-9,10), gS(-7,1), gS(-31,-1), gS(-25,-5),
           gS(-2,0), gS(0,3), gS(3,3), gS(17,1), gS(20,2), gS(4,2), gS(10,-1), gS(13,-5),
           gS(-9,-1), gS(12,3), gS(2,3), gS(24,7), gS(17,8), gS(15,2), gS(12,-2), gS(1,-8),
           gS(8,-6), gS(0,1), gS(10,2), gS(20,5), gS(24,1), gS(6,2), gS(11,0), gS(10,-16),
           gS(0,-15), gS(19,-7), gS(10,-5), gS(12,4), gS(6,10), gS(20,-8), gS(15,-10), gS(7,-11),
           gS(18,-26), gS(13,-25), gS(19,-14), gS(8,-1), gS(17,-2), gS(17,-7), gS(23,-23), gS(10,-37),
           gS(0,-34), gS(12,-13), gS(0,-2), gS(5,-1), gS(9,-4), gS(-9,4), gS(1,-10), gS(3,-32),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-144,-64), gS(-37,-18), gS(-72,8), gS(-14,-3), gS(0,0), gS(-62,5), gS(-30,-11), gS(-95,-69),
           gS(-21,-15), gS(-8,1), gS(9,0), gS(36,10), gS(24,7), gS(42,-11), gS(-14,0), gS(-10,-19),
           gS(-11,-5), gS(15,1), gS(24,21), gS(31,17), gS(54,7), gS(38,9), gS(14,-7), gS(11,-13),
           gS(10,-2), gS(10,1), gS(18,17), gS(35,21), gS(25,19), gS(36,13), gS(14,-5), gS(24,-11),
           gS(1,0), gS(13,7), gS(16,24), gS(25,21), gS(26,26), gS(29,17), gS(25,3), gS(11,-2),
           gS(-12,-18), gS(8,-3), gS(7,9), gS(20,24), gS(23,20), gS(17,5), gS(22,-4), gS(-2,-8),
           gS(-9,-22), gS(-12,4), gS(3,-5), gS(8,9), gS(13,8), gS(11,-2), gS(1,1), gS(0,-11),
           gS(-40,-44), gS(-13,-17), gS(-4,-8), gS(0,3), gS(7,4), gS(-4,-5), gS(-8,-8), gS(-32,-42),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-15,-3), gS(-5,13), gS(-5,28), gS(-3,40), gS(-4,48), gS(18,35), gS(-7,35), gS(-9,19),
           gS(-13,-6), gS(-31,25), gS(-13,23), gS(-37,63), gS(-43,89), gS(-15,55), gS(-27,62), gS(0,28),
           gS(-9,-17), gS(0,-11), gS(-6,18), gS(0,42), gS(0,56), gS(3,60), gS(8,27), gS(-6,37),
           gS(-2,-26), gS(-10,2), gS(-3,10), gS(-6,42), gS(-12,61), gS(-10,57), gS(-8,43), gS(2,14),
           gS(0,-36), gS(-5,-7), gS(-2,1), gS(-6,34), gS(0,28), gS(-8,20), gS(6,3), gS(-5,2),
           gS(0,-49), gS(10,-32), gS(5,-13), gS(3,-12), gS(3,-11), gS(7,-10), gS(13,-20), gS(5,-36),
           gS(4,-61), gS(9,-56), gS(19,-64), gS(12,-39), gS(15,-39), gS(21,-73), gS(22,-79), gS(0,-52),
           gS(3,-63), gS(7,-73), gS(5,-75), gS(5,-59), gS(7,-64), gS(-5,-80), gS(1,-81), gS(5,-71),
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
