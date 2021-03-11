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
const int  KING_HIGH_DANGER  = gS(-13,-28);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-6,-17);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(1,7);   // слабый пешечный щит
const int  KING_SAFE         = gS(3,26);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-21,-109), gS(-8,-50), gS(-3,-22), gS(0,-5), gS(4,9), gS(7,21), gS(7,29),
           gS(7,32), gS(9,36), gS(14,35), gS(21,31), gS(38,23), gS(48,35), gS(77,1),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-7,-79), gS(0,-30), gS(3,-4), gS(6,11), gS(8,20),
           gS(11,27), gS(16,26), gS(22,18), gS(32,2),
};

const int  KING_MOBILITY[9] = {
           gS(13,1), gS(14,2), gS(7,8), gS(2,12), gS(-4,9),
           gS(-8,6), gS(-5,4), gS(-14,-2), gS(-6,-15),
};

const int  ROOK_MOBILITY[15] = {
           gS(-18,-94), gS(-12,-61), gS(-8,-29), gS(-6,-9), gS(-7,8), gS(-9,20), gS(-3,23),
           gS(0,25), gS(5,28), gS(10,32), gS(11,35), gS(17,37), gS(20,38), gS(35,24),
           gS(86,-2),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-34,-78), gS(-17,-116), gS(-3,-119), gS(0,-82), gS(1,-47), gS(3,-23), gS(4,-6),
           gS(6,7), gS(8,16), gS(10,22), gS(11,29), gS(12,34), gS(12,38), gS(10,43),
           gS(8,49), gS(5,53), gS(3,56), gS(2,56), gS(1,57), gS(4,54), gS(7,49),
           gS(10,40), gS(11,37), gS(22,29), gS(15,24), gS(21,24), gS(26,29), gS(24,26),
};

const int PIECE_ATTACK_POWER[6] = {0, 40, 35, 20, 80, 0};
const int COUNT_TO_POWER[8] = {0, 0, 50, 75, 80, 88, 95, 100};

/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 * 'vBishopEG': 288, 'vBishopOP': 336, 'vKnightEG': 310, 'vKnightOP': 316, 'vPawnEG': 112, 'vQueenEG': 1005, 'vQueenOP': 1100, 'vRookEG': 500, 'vRookOP': 470}
    Finished game 201 (Drofa_dev vs Drofa_1.2.9): 1-0 {White wins by adjudication}
    Score of Drofa_dev vs Drofa_1.2.9: 67 - 63 - 70  [0.510] 200

    {'vBishopEG': 357, 'vBishopOP': 336, 'vKnightEG': 328, 'vKnightOP': 304, 'vPawnEG': 86, 'vQueenEG': 995, 'vQueenOP': 1190, 'vRookEG': 565, 'vRookOP': 465}

 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(75,111),
        [ROOK] = gS(411,730),
        [KNIGHT] = gS(283,405),
        [BISHOP] = gS(303,439),
        [QUEEN] = gS(898,1359),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(15,76);

const int  PAWN_SUPPORTED = gS(7,5);

const int  DOUBLED_PAWN_PENALTY = gS(-12,-26);

const int  ISOLATED_PAWN_PENALTY = gS(-8,-13);

const int  BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(7,26), gS(-5,21), gS(-10,7),
           gS(-7,-4), gS(-7,-7), gS(8,-12), gS(-3,-21),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(37,79), gS(-17,14), gS(-4,-2),
           gS(-3,-15), gS(-1,-26), gS(6,-37), gS(14,-44),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {  gS(23,8), gS(27,15) };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(5,13), gS(18,17), };

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-25), gS(-16,-12), gS(-6,21),
           gS(17,53), gS(27,112), gS(69,108), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(13,16), gS(7,17), gS(-1,4), gS(-3,2),
           gS(-5,0), gS(-19,3), gS(-8,18), gS(13,13),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-70), gS(-11,-39), gS(-10,-27), gS(-9,-16), gS(-11,-27), gS(-12,-22), gS(-13,-24), gS(-12,-63),
           gS(-13,-37), gS(0,34), gS(3,24), gS(-4,7), gS(-4,5), gS(1,23), gS(-5,44), gS(-12,-32),
           gS(-4,-4), gS(17,39), gS(19,33), gS(5,20), gS(5,17), gS(20,35), gS(17,41), gS(-7,-8),
           gS(-9,-10), gS(13,21), gS(21,28), gS(-5,29), gS(6,25), gS(23,25), gS(25,19), gS(-28,-12),
           gS(-16,-35), gS(17,3), gS(24,17), gS(-12,29), gS(4,26), gS(13,18), gS(28,3), gS(-42,-25),
           gS(-41,-19), gS(-8,2), gS(-5,10), gS(-26,25), gS(-13,21), gS(-13,15), gS(-5,1), gS(-51,-14),
           gS(0,-13), gS(-2,6), gS(-22,15), gS(-61,23), gS(-42,18), gS(-43,21), gS(-1,3), gS(-3,-22),
           gS(-26,-59), gS(4,-29), gS(-28,-8), gS(-36,-28), gS(-26,-38), gS(-53,-8), gS(-4,-27), gS(-15,-75),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(63,81), gS(52,92), gS(70,90), gS(71,76), gS(81,77), gS(76,84), gS(-3,98), gS(-17,93),
           gS(-3,51), gS(25,46), gS(42,39), gS(41,19), gS(53,21), gS(98,27), gS(60,33), gS(9,38),
           gS(-5,26), gS(1,24), gS(6,21), gS(21,5), gS(30,9), gS(30,14), gS(10,21), gS(-2,15),
           gS(-14,12), gS(-9,14), gS(0,7), gS(4,5), gS(8,6), gS(10,9), gS(0,7), gS(-13,3),
           gS(-17,5), gS(-15,5), gS(-10,11), gS(-6,11), gS(0,14), gS(1,14), gS(3,0), gS(-12,-3),
           gS(-15,10), gS(-12,10), gS(-15,22), gS(-7,23), gS(-11,29), gS(9,24), gS(8,4), gS(-14,-7),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(28,26), gS(22,32), gS(0,40), gS(7,35), gS(10,35), gS(23,34), gS(36,30), gS(46,27),
           gS(-3,27), gS(-8,32), gS(14,28), gS(23,29), gS(20,30), gS(41,11), gS(6,24), gS(14,20),
           gS(-6,20), gS(35,5), gS(21,17), gS(42,5), gS(52,0), gS(49,9), gS(72,-8), gS(15,11),
           gS(-5,11), gS(11,10), gS(19,12), gS(41,4), gS(34,5), gS(25,5), gS(29,1), gS(11,6),
           gS(-18,1), gS(-11,12), gS(-12,13), gS(-4,7), gS(-5,5), gS(-10,8), gS(2,4), gS(-14,0),
           gS(-23,-15), gS(-8,-5), gS(-16,-5), gS(-7,-10), gS(-5,-11), gS(-13,-9), gS(5,-10), gS(-18,-17),
           gS(-52,-11), gS(-16,-19), gS(-10,-14), gS(-2,-19), gS(-2,-21), gS(-4,-26), gS(-4,-26), gS(-56,-12),
           gS(-15,-15), gS(-8,-14), gS(-5,-11), gS(1,-18), gS(0,-18), gS(-2,-11), gS(0,-18), gS(-11,-25),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-40,9), gS(-32,5), gS(-56,5), gS(-63,10), gS(-48,7), gS(-72,2), gS(-11,-2), gS(-38,2),
           gS(-47,1), gS(-3,6), gS(-6,5), gS(-21,9), gS(-16,7), gS(0,6), gS(-22,9), gS(-21,-5),
           gS(-2,-2), gS(6,6), gS(30,6), gS(8,6), gS(23,6), gS(21,14), gS(27,1), gS(5,-1),
           gS(-13,-5), gS(13,0), gS(3,5), gS(22,11), gS(17,13), gS(13,2), gS(12,1), gS(-11,-1),
           gS(-3,-13), gS(1,-7), gS(8,8), gS(15,6), gS(21,5), gS(2,4), gS(8,-5), gS(-1,-14),
           gS(-1,-15), gS(10,-3), gS(10,0), gS(15,2), gS(9,4), gS(14,0), gS(12,-6), gS(12,-8),
           gS(10,-18), gS(13,-22), gS(12,-12), gS(3,-6), gS(8,-5), gS(9,-13), gS(22,-13), gS(10,-31),
           gS(13,-24), gS(4,-10), gS(1,-9), gS(-1,-12), gS(-1,-8), gS(2,-6), gS(0,-7), gS(10,-24),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-127,-77), gS(-25,-25), gS(-47,3), gS(-13,-4), gS(-5,0), gS(-35,4), gS(-18,-13), gS(-63,-56),
           gS(-14,-26), gS(-13,-9), gS(29,-8), gS(44,13), gS(32,13), gS(51,-16), gS(-21,-8), gS(-8,-23),
           gS(-18,-18), gS(12,-5), gS(30,28), gS(29,28), gS(55,18), gS(50,24), gS(32,-10), gS(0,-16),
           gS(-2,-10), gS(4,7), gS(23,28), gS(22,38), gS(11,38), gS(34,24), gS(12,15), gS(5,-3),
           gS(-7,-7), gS(7,9), gS(10,30), gS(13,32), gS(16,34), gS(16,32), gS(20,12), gS(3,1),
           gS(-20,-32), gS(-2,-6), gS(1,7), gS(11,22), gS(15,20), gS(8,4), gS(3,-2), gS(-8,-23),
           gS(-24,-14), gS(-28,1), gS(-8,-12), gS(5,2), gS(1,5), gS(-2,-12), gS(-15,0), gS(-10,-3),
           gS(-40,-39), gS(-12,-29), gS(-12,-20), gS(-10,-1), gS(-2,-3), gS(-7,-10), gS(-10,-20), gS(-41,-38),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-8,3), gS(0,17), gS(3,26), gS(9,35), gS(16,43), gS(34,43), gS(16,40), gS(18,30),
           gS(-18,-2), gS(-56,37), gS(-16,25), gS(-33,59), gS(-46,101), gS(3,59), gS(-48,51), gS(-18,33),
           gS(-14,-15), gS(-6,-14), gS(-8,15), gS(-11,39), gS(0,57), gS(14,76), gS(27,50), gS(-6,62),
           gS(-4,-31), gS(-3,5), gS(-9,6), gS(-16,49), gS(-14,67), gS(-15,75), gS(-3,66), gS(0,36),
           gS(0,-40), gS(1,-12), gS(-3,-5), gS(-9,31), gS(-6,28), gS(-14,29), gS(6,5), gS(-4,12),
           gS(-2,-56), gS(8,-41), gS(3,-21), gS(1,-28), gS(1,-26), gS(-1,-12), gS(12,-33), gS(0,-41),
           gS(-1,-64), gS(6,-63), gS(15,-79), gS(7,-49), gS(12,-53), gS(14,-91), gS(12,-86), gS(-16,-47),
           gS(1,-69), gS(4,-80), gS(6,-88), gS(10,-68), gS(11,-73), gS(4,-85), gS(4,-67), gS(-7,-55),
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
bool IsItDeadDraw (int w_P, int w_N, int w_B, int w_R, int w_Q,
int b_P, int b_N, int b_B, int b_R, int b_Q);

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
