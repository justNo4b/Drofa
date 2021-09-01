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

const int KING_HIGH_DANGER = gS(-51,-68);

const int KING_MED_DANGER = gS(-21,-43);

const int KING_LOW_DANGER = gS(40,-31);

const int BISHOP_PAIR_BONUS = gS(30,53);

const int PAWN_SUPPORTED = gS(12,8);

const int DOUBLED_PAWN_PENALTY = gS(-7,-21);

const int ISOLATED_PAWN_PENALTY = gS(-6,-5);

const int PAWN_BLOCKED = gS(3,14);

const int PASSER_BLOCKED = gS(11,0);

const int BISHOP_RAMMED_PENALTY = gS(-3,-6);

const int BISHOP_CENTER_CONTROL = gS(9,9);

const int MINOR_BEHIND_PAWN = gS(10,8);

const int MINOR_BEHIND_PASSER = gS(13,0);

const int KING_AHEAD_PASSER = gS(-23,15);

const int KING_EQUAL_PASSER = gS(10,7);

const int KING_BEHIND_PASSER = gS(6,-7);

const int KING_OPEN_FILE = gS(-64,7);

const int KING_OWN_SEMI_FILE = gS(-25,14);

const int KING_ENEMY_SEMI_LINE = gS(-18,8);

const int KING_ATTACK_PAWN = gS(19,50);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,0), gS(6,6), gS(9,11),
           gS(18,26), gS(36,56), gS(4,25),
};

const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(0,-8), gS(-2,8), gS(-1,46),
           gS(14,55), gS(26,78), gS(59,85),
};

const int PASSED_PAWN_FILES[8] = {
           gS(7,23), gS(2,13), gS(1,3), gS(0,-9),
           gS(-4,-7), gS(-2,2), gS(0,14), gS(7,16),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-4,9), gS(4,25), gS(40,32),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(11,29), gS(25,61), gS(17,124),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-8,-21), gS(15,4), gS(0,-18), gS(-11,-6),
           gS(-15,-6), gS(-21,-8), gS(-22,-11), gS(-20,-9),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(1,37), gS(-6,26), gS(-3,7),
           gS(0,-1), gS(0,0), gS(16,-3), gS(10,-3),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-37,-2), gS(13,-18), gS(3,0),
           gS(5,11), gS(0,21), gS(0,22), gS(-12,20),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(32,-4), gS(33,0),
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
           gS(0,19), gS(-62,-37), gS(-48,-42), gS(-52,-67), gS(-46,-21),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(14,33), gS(14,35), gS(14,30),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-5,18), gS(61,21), gS(41,39),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(47,17), gS(33,6), gS(36,8),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-1,0), gS(5,21), gS(6,8), gS(0,24),
           gS(-5,5), gS(-14,-20), gS(-3,2), gS(0,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(11,5), gS(12,29), gS(20,7), gS(25,25),
           gS(12,5), gS(-12,-9), gS(-4,3), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-13,-66), gS(-3,-27), gS(4,-14), gS(10,-3), gS(13,6), gS(15,10), gS(16,14),
           gS(17,14), gS(18,15), gS(20,14), gS(22,12), gS(34,4), gS(37,20), gS(69,-6),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-4,-81), gS(8,-34), gS(15,-7), gS(20,2), gS(25,10),
           gS(29,16), gS(33,14), gS(38,9), gS(42,0),
};

const int KING_MOBILITY[9] = {
           gS(18,-8), gS(10,-2), gS(5,2), gS(1,6), gS(-3,4),
           gS(-10,4), gS(-6,3), gS(-11,0), gS(-4,-17),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-89), gS(-10,-53), gS(-5,-31), gS(1,-17), gS(3,-4), gS(1,8), gS(6,10),
           gS(8,10), gS(12,16), gS(14,20), gS(18,25), gS(21,27), gS(26,26), gS(32,25),
           gS(55,16),
};

const int QUEEN_MOBILITY[28] = {
           gS(-17,-114), gS(-13,-176), gS(-11,-119), gS(-10,-66), gS(-8,-41), gS(-6,-25), gS(-5,-10),
           gS(-3,-1), gS(-1,10), gS(0,19), gS(1,25), gS(2,31), gS(3,35), gS(4,38),
           gS(4,42), gS(5,46), gS(5,50), gS(7,48), gS(7,48), gS(10,45), gS(12,40),
           gS(16,37), gS(11,40), gS(22,28), gS(10,25), gS(18,21), gS(23,26), gS(21,23),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(-9,-102), gS(0,-44), gS(0,-28), gS(0,-13), gS(0,-28), gS(0,-23), gS(0,-20), gS(-8,-94),
           gS(-10,-29), gS(0,29), gS(5,27), gS(1,21), gS(1,12), gS(4,29), gS(0,31), gS(-7,-26),
           gS(-2,-5), gS(27,24), gS(30,34), gS(11,37), gS(14,38), gS(35,34), gS(23,28), gS(-3,-4),
           gS(-10,-14), gS(18,13), gS(19,24), gS(-4,37), gS(-2,34), gS(21,24), gS(21,14), gS(-40,-14),
           gS(-23,-26), gS(14,0), gS(6,16), gS(-24,30), gS(-16,27), gS(-14,18), gS(6,2), gS(-57,-18),
           gS(-42,-23), gS(6,-1), gS(-16,11), gS(-39,24), gS(-36,24), gS(-30,14), gS(-6,0), gS(-41,-18),
           gS(-10,-25), gS(-3,-3), gS(-1,3), gS(-22,13), gS(-22,13), gS(-2,4), gS(-9,0), gS(-15,-25),
           gS(-30,-56), gS(3,-33), gS(-16,-13), gS(-31,-13), gS(14,-44), gS(-11,-23), gS(0,-31), gS(-11,-64),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(59,69), gS(41,82), gS(48,71), gS(67,63), gS(71,64), gS(65,64), gS(-2,88), gS(-4,80),
           gS(6,36), gS(13,38), gS(24,25), gS(47,12), gS(49,13), gS(72,17), gS(36,31), gS(28,23),
           gS(-8,16), gS(0,17), gS(6,7), gS(20,6), gS(25,6), gS(21,4), gS(10,10), gS(-1,9),
           gS(-19,2), gS(-17,9), gS(-1,0), gS(14,-1), gS(13,0), gS(13,-2), gS(-2,1), gS(-14,-4),
           gS(-19,-6), gS(-19,0), gS(-7,-2), gS(-7,7), gS(-1,6), gS(-1,1), gS(7,-10), gS(-12,-12),
           gS(-21,4), gS(-11,5), gS(-14,14), gS(-4,17), gS(-5,19), gS(13,7), gS(11,-1), gS(-11,-7),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(22,15), gS(15,23), gS(0,30), gS(1,22), gS(8,22), gS(21,21), gS(32,16), gS(40,14),
           gS(4,18), gS(0,18), gS(16,14), gS(31,7), gS(25,9), gS(39,2), gS(2,13), gS(13,11),
           gS(2,19), gS(21,8), gS(18,12), gS(29,2), gS(44,-3), gS(47,-3), gS(44,-4), gS(4,10),
           gS(-1,14), gS(0,13), gS(15,10), gS(21,3), gS(21,1), gS(23,0), gS(11,2), gS(0,5),
           gS(-15,9), gS(-12,14), gS(-4,11), gS(-2,7), gS(0,5), gS(-1,4), gS(-3,6), gS(-19,5),
           gS(-17,0), gS(-10,0), gS(-5,0), gS(-5,-1), gS(-3,-1), gS(0,-7), gS(2,-7), gS(-13,-10),
           gS(-22,-9), gS(-6,-10), gS(-1,-7), gS(0,-10), gS(9,-14), gS(9,-20), gS(5,-23), gS(-39,-11),
           gS(-6,-11), gS(0,-14), gS(1,-9), gS(11,-21), gS(14,-24), gS(14,-21), gS(-7,-11), gS(-2,-26),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-37,3), gS(-43,0), gS(-79,16), gS(-81,18), gS(-66,18), gS(-91,12), gS(-18,-5), gS(-35,-6),
           gS(-39,0), gS(-18,0), gS(-10,0), gS(-13,13), gS(-9,10), gS(-7,1), gS(-29,-2), gS(-23,-5),
           gS(-3,0), gS(0,3), gS(2,3), gS(19,0), gS(22,1), gS(5,1), gS(10,0), gS(14,-5),
           gS(-9,-1), gS(12,4), gS(2,3), gS(25,7), gS(17,9), gS(16,2), gS(11,0), gS(3,-8),
           gS(8,-6), gS(0,2), gS(11,2), gS(19,5), gS(23,2), gS(5,3), gS(11,0), gS(10,-16),
           gS(0,-15), gS(19,-7), gS(9,-6), gS(11,4), gS(5,9), gS(19,-9), gS(15,-10), gS(7,-10),
           gS(19,-25), gS(13,-25), gS(19,-13), gS(7,-1), gS(16,-2), gS(16,-7), gS(22,-24), gS(10,-36),
           gS(0,-34), gS(11,-12), gS(0,-2), gS(4,-1), gS(8,-4), gS(-9,3), gS(0,-10), gS(2,-31),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-64), gS(-36,-16), gS(-73,8), gS(-13,-2), gS(0,0), gS(-63,4), gS(-29,-10), gS(-93,-68),
           gS(-22,-14), gS(-9,1), gS(8,0), gS(36,10), gS(23,7), gS(41,-10), gS(-13,0), gS(-9,-18),
           gS(-10,-4), gS(15,1), gS(22,21), gS(33,16), gS(55,6), gS(38,8), gS(15,-6), gS(12,-13),
           gS(9,-2), gS(9,1), gS(17,17), gS(35,21), gS(26,19), gS(37,12), gS(16,-5), gS(26,-11),
           gS(0,0), gS(13,6), gS(16,23), gS(25,21), gS(27,26), gS(30,17), gS(26,3), gS(11,-1),
           gS(-13,-18), gS(7,-4), gS(6,8), gS(19,23), gS(22,20), gS(17,4), gS(21,-5), gS(-2,-8),
           gS(-8,-23), gS(-11,4), gS(3,-6), gS(8,7), gS(13,6), gS(10,-3), gS(0,1), gS(0,-12),
           gS(-40,-43), gS(-14,-17), gS(-3,-7), gS(0,2), gS(7,3), gS(-5,-5), gS(-8,-8), gS(-30,-40),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-14,-2), gS(-3,13), gS(-4,27), gS(-3,38), gS(-4,46), gS(16,32), gS(-7,33), gS(-9,19),
           gS(-13,-4), gS(-30,25), gS(-11,24), gS(-35,62), gS(-42,87), gS(-15,54), gS(-24,61), gS(1,27),
           gS(-9,-17), gS(0,-9), gS(-5,19), gS(2,42), gS(2,56), gS(3,58), gS(9,27), gS(-4,35),
           gS(-2,-25), gS(-10,3), gS(-2,12), gS(-3,42), gS(-9,61), gS(-7,56), gS(-5,42), gS(5,12),
           gS(0,-35), gS(-4,-5), gS(-2,2), gS(-5,35), gS(1,28), gS(-6,19), gS(8,4), gS(-4,1),
           gS(-1,-47), gS(8,-31), gS(4,-12), gS(2,-11), gS(2,-10), gS(7,-10), gS(12,-18), gS(5,-35),
           gS(3,-60), gS(7,-57), gS(17,-65), gS(10,-40), gS(13,-40), gS(19,-73), gS(20,-80), gS(0,-51),
           gS(2,-63), gS(5,-73), gS(3,-76), gS(3,-62), gS(4,-65), gS(-6,-80), gS(0,-80), gS(6,-70),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(76,113),
        [ROOK] = gS(475,698),
        [KNIGHT] = gS(334,384),
        [BISHOP] = gS(358,413),
        [QUEEN] = gS(966,1324),
        [KING] = gS(0, 0)
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
