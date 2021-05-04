#ifndef EVAL_H
#define EVAL_H

#include "defs.h"
#include "movegen.h"
#include "bitutils.h"

/**
 * @brief Represent CuttOffState of the node saved in the transposition table.
 */ 
enum TuningFeature{
        BISHOP_PAIR,
        ROOK_OPEN,
        ROOK_SEMI
};

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
 * @brief Array of masks indexed by [Color][sideTo_OO] containing all squares
 * that indicates where King is castled
 * 0 - kingside castle
 * 1 - queenside castle
 * 
 */ 
extern U64 KING_OO_MASKS[2][2];

/**
 * @brief Array of masks indexed by [Color][sideTo_OO][mask_NUM]
 * 0 - kindgside castle masks
 * 1 - queenside castle masks
 * 
 * maskNUMs - just all masks
 */
extern U64 KING_PAWN_MASKS[2][2][7];

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
 * @brief Бонусы и штрафы за то, насколько король в опасности
 * 
 */ 
const int  KING_HIGH_DANGER  = gS(-14,-28);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-7,-16);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(0,9);   // слабый пешечный щит
const int  KING_SAFE         = gS(4,24);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-19,-105), gS(-6,-46), gS(-1,-19), gS(1,-2), gS(4,11), gS(5,22), gS(5,29),
           gS(3,31), gS(4,33), gS(7,31), gS(13,27), gS(29,17), gS(45,28), gS(77,-6),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-75), gS(1,-29), gS(4,-4), gS(7,10), gS(10,19),
           gS(13,26), gS(17,24), gS(24,16), gS(34,1),
};

const int KING_MOBILITY[9] = {
           gS(11,-1), gS(15,-2), gS(9,4), gS(4,9), gS(-2,8),
           gS(-8,5), gS(-6,3), gS(-17,-2), gS(-11,-15),
};

const int ROOK_MOBILITY[15] = {
           gS(-20,-102), gS(-12,-66), gS(-8,-34), gS(-6,-13), gS(-7,5), gS(-9,19), gS(-3,23),
           gS(0,26), gS(4,31), gS(9,36), gS(11,40), gS(17,42), gS(21,43), gS(36,29),
           gS(88,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-38,-102), gS(-18,-164), gS(-5,-132), gS(-2,-78), gS(-1,-45), gS(0,-23), gS(2,-5),
           gS(3,7), gS(6,17), gS(8,23), gS(9,30), gS(10,35), gS(10,39), gS(9,45),
           gS(8,51), gS(5,55), gS(4,57), gS(4,57), gS(4,57), gS(9,53), gS(12,48),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
};

const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
const int COUNT_TO_POWER[8] = {0, 0, 50, 75, 80, 88, 95, 100};

/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 * 'vBishopEG': 288, 'vBishopOP': 336, 'vKnightEG': 310, 'vKnightOP': 316, 'vPawnEG': 112, 'vQueenEG': 1005, 'vQueenOP': 1100, 'vRookEG': 500, 'vRookOP': 470}
    Finished game 201 (Drofa_dev vs Drofa_1.2.9): 1-0 {White wins by adjudication}
    Score of Drofa_dev vs Drofa_1.2.9: 67 - 63 - 70  [0.510] 200
    {'vBishopEG': 357, 'vBishopOP': 336, 'vKnightEG': 328, 'vKnightOP': 304, 'vPawnEG': 86, 'vQueenEG': 995, 'vQueenOP': 1190, 'vRookEG': 565, 'vRookOP': 465}
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(77,125),
        [ROOK] = gS(421,739),
        [KNIGHT] = gS(284,407),
        [BISHOP] = gS(304,437),
        [QUEEN] = gS(940,1353),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(14,76);

const int PAWN_SUPPORTED = gS(7,5);

const int DOUBLED_PAWN_PENALTY = gS(-8,-30);

const int ISOLATED_PAWN_PENALTY = gS(-3,-5);

const int PAWN_BLOCKED = gS(1,27);

const int PASSER_BLOCKED = gS(2,45);

const int PAWN_DISTORTION = gS(-1,-1);

const int BISHOP_RAMMED_PENALTY = gS(-1,-9);

const int BISHOP_CENTER_CONTROL = gS(10,8);

const int MINOR_BEHIND_PAWN = gS(8,13);

const int MINOR_BEHIND_PASSER = gS(9,13);

const int KING_AHEAD_PASSER = gS(-12,9);

const int KING_EQUAL_PASSER = gS(4,2);

const int KING_BEHIND_PASSER = gS(4,-4);

const int KING_PAWN_TENSION = gS(-15,0);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(4,30), gS(-5,26), gS(-8,10),
           gS(-5,-1), gS(-4,-4), gS(10,-9), gS(0,-19),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(47,74), gS(-16,12), gS(-5,-3),
           gS(-5,-16), gS(-3,-27), gS(4,-38), gS(14,-45),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(24,9), gS(24,10), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(5,9), gS(17,9), };


const int HANGING_PIECE[5] = {
           gS(0,0), gS(-26,-17), gS(-25,-52), gS(-32,-72), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-12,-29), gS(-16,-15), gS(-8,20),
           gS(14,54), gS(25,118), gS(77,128), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,15), gS(6,16), gS(0,4), gS(-1,0),
           gS(-3,-1), gS(-15,2), gS(-7,17), gS(11,11),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-94), gS(-3,-47), gS(-2,-33), gS(-1,-18), gS(-3,-32), gS(-4,-27), gS(-5,-25), gS(-12,-86),
           gS(-13,-37), gS(0,34), gS(5,23), gS(0,5), gS(0,4), gS(3,23), gS(0,44), gS(-12,-36),
           gS(-3,-3), gS(27,38), gS(33,33), gS(12,20), gS(12,17), gS(35,36), gS(27,42), gS(-7,-8),
           gS(-9,-9), gS(22,20), gS(29,29), gS(0,31), gS(8,27), gS(33,27), gS(30,21), gS(-36,-10),
           gS(-22,-33), gS(22,2), gS(26,18), gS(-15,33), gS(2,29), gS(12,20), gS(24,5), gS(-53,-21),
           gS(-52,-15), gS(-9,1), gS(-7,11), gS(-28,26), gS(-16,23), gS(-16,16), gS(-9,2), gS(-52,-12),
           gS(0,-12), gS(-2,5), gS(-24,15), gS(-64,24), gS(-44,19), gS(-46,22), gS(0,2), gS(-3,-21),
           gS(-27,-56), gS(2,-28), gS(-30,-6), gS(-38,-27), gS(-28,-37), gS(-56,-7), gS(-5,-27), gS(-15,-73),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(62,72), gS(50,97), gS(69,93), gS(75,79), gS(81,77), gS(84,81), gS(-8,101), gS(-22,83),
           gS(-3,47), gS(29,53), gS(41,44), gS(44,24), gS(54,24), gS(95,30), gS(63,38), gS(8,33),
           gS(-5,20), gS(5,26), gS(7,22), gS(21,7), gS(30,10), gS(30,14), gS(12,22), gS(-5,8),
           gS(-15,4), gS(-6,13), gS(-4,4), gS(4,3), gS(9,4), gS(5,5), gS(2,5), gS(-15,-5),
           gS(-19,-2), gS(-15,2), gS(-13,6), gS(-11,6), gS(-4,9), gS(-3,8), gS(2,-3), gS(-15,-13),
           gS(-18,0), gS(-12,5), gS(-17,15), gS(-10,16), gS(-14,22), gS(5,17), gS(7,0), gS(-17,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,26), gS(22,32), gS(0,41), gS(7,36), gS(11,36), gS(24,36), gS(40,30), gS(51,27),
           gS(-3,27), gS(-7,32), gS(15,28), gS(25,30), gS(22,31), gS(41,13), gS(6,25), gS(14,21),
           gS(-8,19), gS(32,4), gS(20,16), gS(42,5), gS(53,0), gS(48,8), gS(70,-8), gS(13,11),
           gS(-7,10), gS(9,9), gS(18,11), gS(39,4), gS(32,3), gS(28,4), gS(30,0), gS(11,5),
           gS(-19,-1), gS(-11,11), gS(-13,11), gS(-2,4), gS(-4,3), gS(-8,8), gS(5,5), gS(-11,-2),
           gS(-24,-17), gS(-8,-5), gS(-15,-6), gS(-7,-12), gS(-6,-12), gS(-9,-8), gS(8,-9), gS(-15,-19),
           gS(-53,-12), gS(-16,-19), gS(-10,-14), gS(-3,-19), gS(-2,-20), gS(-2,-23), gS(-2,-24), gS(-55,-11),
           gS(-16,-15), gS(-9,-13), gS(-6,-11), gS(1,-18), gS(0,-18), gS(-2,-8), gS(0,-16), gS(-11,-24),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,6), gS(-43,8), gS(-75,17), gS(-79,24), gS(-66,19), gS(-91,16), gS(-15,0), gS(-48,0),
           gS(-48,1), gS(-10,3), gS(-7,8), gS(-14,17), gS(-7,15), gS(0,8), gS(-26,5), gS(-21,-4),
           gS(4,0), gS(4,7), gS(24,3), gS(8,9), gS(24,7), gS(10,9), gS(25,1), gS(16,-2),
           gS(-9,0), gS(19,4), gS(5,5), gS(22,15), gS(19,14), gS(15,3), gS(20,1), gS(-7,0),
           gS(0,-12), gS(7,-2), gS(11,8), gS(18,7), gS(25,7), gS(3,5), gS(14,-1), gS(3,-14),
           gS(0,-17), gS(11,-7), gS(7,-4), gS(14,1), gS(9,3), gS(12,-6), gS(12,-9), gS(14,-11),
           gS(8,-29), gS(9,-33), gS(11,-16), gS(4,-2), gS(8,-4), gS(9,-16), gS(18,-28), gS(4,-40),
           gS(6,-37), gS(1,-15), gS(1,-4), gS(1,-7), gS(1,-3), gS(0,-8), gS(-1,-10), gS(3,-37),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-143,-69), gS(-35,-19), gS(-64,11), gS(-14,0), gS(0,1), gS(-52,13), gS(-28,-11), gS(-87,-70),
           gS(-11,-24), gS(-12,-5), gS(32,-4), gS(49,17), gS(37,16), gS(53,-12), gS(-21,-3), gS(-7,-21),
           gS(-19,-18), gS(12,-4), gS(32,30), gS(31,31), gS(58,19), gS(45,24), gS(28,-12), gS(4,-16),
           gS(0,-10), gS(8,5), gS(26,28), gS(25,39), gS(13,36), gS(38,22), gS(14,8), gS(7,-7),
           gS(-4,-9), gS(10,8), gS(15,30), gS(21,31), gS(23,34), gS(20,32), gS(24,11), gS(7,-1),
           gS(-18,-34), gS(1,-7), gS(5,8), gS(13,22), gS(17,21), gS(13,5), gS(7,-2), gS(-4,-27),
           gS(-21,-17), gS(-28,-2), gS(-6,-12), gS(7,4), gS(2,6), gS(0,-12), gS(-17,-3), gS(-11,-4),
           gS(-40,-46), gS(-13,-29), gS(-11,-19), gS(-7,1), gS(0,0), gS(-7,-12), gS(-10,-21), gS(-40,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-4,3), gS(0,19), gS(0,33), gS(3,44), gS(9,53), gS(31,48), gS(8,49), gS(15,35),
           gS(-18,0), gS(-57,41), gS(-17,30), gS(-41,73), gS(-48,112), gS(0,72), gS(-56,72), gS(-18,39),
           gS(-15,-15), gS(-7,-15), gS(-11,16), gS(-12,43), gS(0,60), gS(9,78), gS(27,48), gS(-2,61),
           gS(-4,-31), gS(-2,0), gS(-9,5), gS(-17,50), gS(-16,66), gS(-11,74), gS(1,58), gS(4,32),
           gS(0,-41), gS(3,-13), gS(-3,-8), gS(-10,28), gS(-6,27), gS(-10,26), gS(9,3), gS(0,7),
           gS(0,-58), gS(10,-42), gS(4,-22), gS(4,-31), gS(3,-28), gS(0,-12), gS(15,-33), gS(4,-47),
           gS(0,-66), gS(7,-63), gS(16,-80), gS(10,-50), gS(14,-54), gS(17,-92), gS(14,-86), gS(-10,-57),
           gS(2,-70), gS(5,-82), gS(8,-88), gS(12,-70), gS(12,-75), gS(6,-90), gS(9,-84), gS(-1,-74),
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
 * @brief Returns the evaluated advantage of the given color in centipawns
 * assuming the game is in the given phase
 *
 * @tparam phase Phase of game to evaluate for
 * @param board Board to evaluate
 * @param color Color to evaluate advantage of
 * @return Advantage of the given color in centipawns, assuming the given
 * game phase
 */
template<GamePhase phase>
int evaluateForPhase(const Board &, Color);

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
 * @brief Evaluates pawn structure and returns a score in centipawns
 *
 * This function internally uses Eval::isolatedPawns(), Eval::passedPawns()
 * and Eval::doubledPawns(), weights each value according to its score
 * and returns the pawn structure score in centipawns.
 *
 * Additionally, if the board's pawn structure has been seen before, this
 * function will look up its value from the pawn structure hash table or
 * store its score in the table if it hasn't yet been seen.
 *
 * @return The score for the given color (in centipawns), considering only
 * its pawn structure
 */
int evaluatePawnStructure(const Board &, Color, GamePhase);


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
 * @brief This function analyses king safety.
 * If there is NO Queen for an opponents it returns 0
 * If there is, it assign penalty or bonus dependent on the pawn 
 * chain position around the KING
 * 
 * Replaces _pawnsShieldingKing function functionality
 * Elo gain test vs _pawnsShieldingKing: 
 */ 
int kingSafety(const Board &, Color, int);

/**
 * @brief This function takes number of each pieceType count for each
 * side and (assuming best play) returns if the position is deadDraw
 * 
 * Returns true is position is drawn, returns false if there is some play left.
 * Based on Vice function.
 *
 */ 
bool IsItDeadDraw (int w_N, int w_B, int w_R, int w_Q,
                   int b_N, int b_B, int b_R, int b_Q);

/**
 * @brief Set value for a MATERIAL_VALUES_TUNABLE array
 * which is used for optuna tuning
 */ 
void SetupTuning(int phase, PieceType piece, int value);

/**
 * @brief Set value for a MATERIAL_VALUES_TUNABLE array
 * which is used for optuna tuning
 */ 
void SetupFeatureTuning(int phase, TuningFeature feature, int value);

};

#endif
