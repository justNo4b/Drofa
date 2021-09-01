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
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int START_ATTACK_VALUE = -50;
  const int ATTACK_TEMPO = 35;
  const int UNCONTESTED_KING_ATTACK [6] = {
      -70, -20, 0, 100, 150, 200
  };
  const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  /**@}*/

const int TEMPO = 10;

const int KING_HIGH_DANGER = gS(-54,-68);

const int KING_MED_DANGER = gS(-22,-43);

const int KING_LOW_DANGER = gS(54,-40);

const int BISHOP_PAIR_BONUS = gS(30,54);

const int PAWN_SUPPORTED = gS(12,8);

const int DOUBLED_PAWN_PENALTY = gS(-7,-21);

const int ISOLATED_PAWN_PENALTY = gS(-6,-5);

const int PAWN_BLOCKED = gS(3,14);

const int PASSER_BLOCKED = gS(10,0);

const int BISHOP_RAMMED_PENALTY = gS(-3,-6);

const int BISHOP_CENTER_CONTROL = gS(9,9);

const int MINOR_BEHIND_PAWN = gS(5,10);

const int MINOR_BEHIND_PASSER = gS(13,0);

const int KING_AHEAD_PASSER = gS(-22,15);

const int KING_EQUAL_PASSER = gS(10,7);

const int KING_BEHIND_PASSER = gS(5,-7);

const int KING_OPEN_FILE = gS(-64,8);

const int KING_OWN_SEMI_FILE = gS(-25,14);

const int KING_ENEMY_SEMI_LINE = gS(-18,8);

const int KING_ATTACK_PAWN = gS(19,50);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,0), gS(6,6), gS(9,11),
           gS(19,26), gS(38,57), gS(5,29),
};

const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(0,-8), gS(-3,8), gS(-1,46),
           gS(15,55), gS(26,78), gS(58,86),
};

const int PASSED_PAWN_FILES[8] = {
           gS(7,23), gS(2,13), gS(1,3), gS(0,-9),
           gS(-5,-7), gS(-3,2), gS(0,14), gS(7,16),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-3,9), gS(5,25), gS(46,30),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(12,28), gS(29,59), gS(23,122),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-7,-22), gS(15,3), gS(0,-18), gS(-11,-6),
           gS(-14,-7), gS(-21,-8), gS(-21,-11), gS(-20,-9),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(2,37), gS(-7,26), gS(-4,8),
           gS(-1,0), gS(0,0), gS(16,-3), gS(13,-2),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-40,-2), gS(13,-18), gS(3,0),
           gS(5,11), gS(0,21), gS(1,22), gS(-10,21),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(31,-4), gS(33,-1),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(14,-3), gS(22,12),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-64,-39), gS(-49,-43), gS(-52,-67), gS(-48,-22),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(14,33), gS(14,36), gS(15,30),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-4,19), gS(62,22), gS(42,41),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(60,24), gS(38,9), gS(42,14),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-2,2), gS(4,22), gS(5,8), gS(0,25),
           gS(-6,5), gS(-16,-18), gS(-4,2), gS(0,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(11,4), gS(12,28), gS(20,7), gS(25,24),
           gS(12,4), gS(-14,-9), gS(-5,2), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-12,-66), gS(-2,-27), gS(5,-14), gS(11,-3), gS(14,6), gS(16,10), gS(16,14),
           gS(17,13), gS(18,15), gS(19,13), gS(22,11), gS(36,3), gS(36,18), gS(68,-7),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-4,-85), gS(8,-34), gS(15,-7), gS(20,2), gS(25,10),
           gS(29,16), gS(33,14), gS(38,8), gS(42,0),
};

const int KING_MOBILITY[9] = {
           gS(18,-11), gS(10,-2), gS(5,2), gS(1,6), gS(-3,5),
           gS(-10,4), gS(-6,4), gS(-12,0), gS(-2,-17),
};

const int ROOK_MOBILITY[15] = {
           gS(-22,-90), gS(-10,-53), gS(-5,-32), gS(1,-17), gS(3,-4), gS(1,8), gS(7,9),
           gS(8,10), gS(12,15), gS(14,20), gS(17,24), gS(20,27), gS(25,25), gS(32,24),
           gS(55,15),
};

const int QUEEN_MOBILITY[28] = {
           gS(-17,-114), gS(-12,-173), gS(-10,-116), gS(-9,-65), gS(-7,-41), gS(-5,-25), gS(-4,-11),
           gS(-2,-1), gS(0,10), gS(0,19), gS(1,25), gS(2,31), gS(4,35), gS(4,38),
           gS(5,41), gS(5,45), gS(5,48), gS(8,46), gS(8,46), gS(11,43), gS(12,37),
           gS(16,34), gS(10,38), gS(21,26), gS(9,24), gS(17,20), gS(22,25), gS(20,22),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(-8,-101), gS(0,-43), gS(0,-26), gS(0,-12), gS(0,-27), gS(0,-22), gS(0,-19), gS(-7,-94),
           gS(-9,-27), gS(0,31), gS(5,29), gS(1,24), gS(1,13), gS(3,31), gS(0,31), gS(-6,-25),
           gS(-1,-4), gS(27,24), gS(29,35), gS(10,38), gS(13,39), gS(34,34), gS(22,28), gS(-2,-3),
           gS(-9,-13), gS(17,13), gS(16,24), gS(-5,36), gS(-3,33), gS(19,23), gS(20,14), gS(-39,-14),
           gS(-22,-25), gS(12,0), gS(3,16), gS(-24,29), gS(-17,26), gS(-17,18), gS(3,2), gS(-56,-16),
           gS(-38,-21), gS(7,-3), gS(-17,10), gS(-40,23), gS(-37,23), gS(-30,13), gS(-6,-1), gS(-33,-16),
           gS(-4,-24), gS(-4,-4), gS(-1,2), gS(-22,11), gS(-22,12), gS(-2,3), gS(-10,-1), gS(-5,-26),
           gS(-25,-54), gS(0,-35), gS(-19,-15), gS(-31,-15), gS(13,-46), gS(-12,-25), gS(-1,-32), gS(-6,-65),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(58,70), gS(41,82), gS(47,70), gS(67,62), gS(70,64), gS(62,64), gS(-2,88), gS(-2,79),
           gS(6,35), gS(12,37), gS(24,25), gS(48,12), gS(49,13), gS(71,17), gS(36,30), gS(28,22),
           gS(-8,16), gS(0,17), gS(6,6), gS(20,6), gS(25,6), gS(22,4), gS(10,10), gS(-1,9),
           gS(-19,2), gS(-17,9), gS(-1,0), gS(14,-1), gS(13,0), gS(13,-2), gS(-3,0), gS(-14,-4),
           gS(-19,-6), gS(-19,0), gS(-7,-2), gS(-7,6), gS(-1,6), gS(0,1), gS(7,-11), gS(-12,-13),
           gS(-22,4), gS(-11,5), gS(-14,14), gS(-4,16), gS(-5,19), gS(14,7), gS(11,-2), gS(-11,-7),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(24,15), gS(15,22), gS(0,29), gS(1,22), gS(8,21), gS(20,20), gS(31,16), gS(40,14),
           gS(5,17), gS(0,17), gS(17,13), gS(32,6), gS(26,8), gS(37,2), gS(0,14), gS(13,11),
           gS(3,18), gS(21,9), gS(18,11), gS(29,1), gS(44,-3), gS(45,-4), gS(39,-2), gS(2,11),
           gS(0,14), gS(0,14), gS(16,10), gS(21,3), gS(21,1), gS(22,0), gS(9,2), gS(0,5),
           gS(-15,9), gS(-11,14), gS(-3,11), gS(-2,7), gS(0,5), gS(-2,3), gS(-4,6), gS(-19,5),
           gS(-17,0), gS(-10,1), gS(-5,0), gS(-4,-1), gS(-3,-1), gS(0,-8), gS(2,-7), gS(-13,-10),
           gS(-21,-8), gS(-5,-10), gS(0,-7), gS(0,-10), gS(11,-15), gS(8,-21), gS(5,-23), gS(-39,-11),
           gS(-5,-11), gS(0,-13), gS(2,-9), gS(11,-21), gS(15,-24), gS(14,-22), gS(-7,-11), gS(-3,-25),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-36,1), gS(-43,-1), gS(-78,16), gS(-81,18), gS(-65,18), gS(-91,13), gS(-19,-6), gS(-32,-8),
           gS(-38,0), gS(-17,0), gS(-10,0), gS(-12,13), gS(-9,10), gS(-7,2), gS(-28,-3), gS(-22,-5),
           gS(-3,0), gS(0,3), gS(2,4), gS(19,-1), gS(22,0), gS(5,0), gS(9,0), gS(13,-5),
           gS(-9,-1), gS(12,5), gS(2,3), gS(24,7), gS(16,9), gS(16,2), gS(10,0), gS(3,-9),
           gS(8,-6), gS(-1,2), gS(11,2), gS(19,6), gS(23,2), gS(5,3), gS(10,0), gS(9,-17),
           gS(0,-15), gS(19,-7), gS(9,-6), gS(11,4), gS(5,9), gS(19,-9), gS(15,-10), gS(7,-10),
           gS(19,-24), gS(13,-26), gS(19,-13), gS(8,-1), gS(16,-2), gS(16,-7), gS(22,-23), gS(10,-35),
           gS(0,-34), gS(11,-11), gS(0,-2), gS(4,-1), gS(9,-4), gS(-9,3), gS(0,-9), gS(2,-30),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-147,-64), gS(-35,-14), gS(-74,8), gS(-13,-2), gS(0,0), gS(-65,3), gS(-29,-9), gS(-92,-67),
           gS(-23,-13), gS(-10,2), gS(7,0), gS(34,9), gS(21,6), gS(39,-10), gS(-12,0), gS(-9,-17),
           gS(-10,-3), gS(16,1), gS(21,22), gS(31,14), gS(53,4), gS(38,8), gS(15,-5), gS(12,-13),
           gS(9,-2), gS(9,1), gS(18,17), gS(34,20), gS(25,18), gS(37,11), gS(15,-6), gS(25,-12),
           gS(0,1), gS(13,6), gS(16,23), gS(25,20), gS(26,25), gS(30,16), gS(25,2), gS(11,-1),
           gS(-12,-18), gS(7,-4), gS(7,8), gS(20,22), gS(23,19), gS(17,4), gS(22,-6), gS(-2,-8),
           gS(-7,-24), gS(-10,4), gS(3,-6), gS(9,7), gS(13,6), gS(11,-4), gS(1,2), gS(0,-13),
           gS(-40,-41), gS(-13,-16), gS(-2,-7), gS(0,2), gS(8,3), gS(-5,-5), gS(-8,-7), gS(-28,-38),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-13,-1), gS(-1,13), gS(-4,25), gS(-3,36), gS(-5,43), gS(13,29), gS(-7,31), gS(-9,19),
           gS(-12,-2), gS(-28,25), gS(-10,25), gS(-33,61), gS(-42,84), gS(-18,51), gS(-21,59), gS(2,25),
           gS(-9,-16), gS(1,-6), gS(-4,21), gS(3,42), gS(1,55), gS(3,55), gS(9,26), gS(-5,32),
           gS(-2,-24), gS(-9,4), gS(-2,14), gS(-3,41), gS(-8,60), gS(-7,53), gS(-6,40), gS(4,9),
           gS(0,-34), gS(-4,-4), gS(-2,3), gS(-4,36), gS(1,28), gS(-7,17), gS(8,4), gS(-5,0),
           gS(-2,-45), gS(9,-30), gS(4,-11), gS(3,-10), gS(2,-8), gS(7,-11), gS(12,-15), gS(5,-34),
           gS(3,-59), gS(8,-57), gS(18,-66), gS(11,-39), gS(13,-39), gS(19,-73), gS(21,-81), gS(0,-50),
           gS(2,-63), gS(5,-73), gS(4,-76), gS(4,-62), gS(5,-65), gS(-6,-80), gS(0,-79), gS(7,-70),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(77,112), gS(481,694), gS(337,382), gS(362,409), gS(980,1314),
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
