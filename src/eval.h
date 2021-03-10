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
const int  KING_HIGH_DANGER  = gS(-13,-29);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-6,-19);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(1,9);   // слабый пешечный щит
const int  KING_SAFE         = gS(4,25);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
 gS(-10,-89), gS(-7,-49), gS(-3,-23), gS(0,-7), gS(4,8), gS(6,21), gS(7,28),
 gS(6,33), gS(8,37), gS(10,35), gS(18,31), gS(33,22), gS(42,37), gS(55,7),
};

const int  KNIGHT_MOBILITY[9] = {
 gS(-9,-85), gS(0,-35), gS(3,-4), gS(5,12), gS(8,24),
 gS(10,33), gS(14,33), gS(20,25), gS(29,11),
};

const int  KING_MOBILITY[9] = {
 gS(11,-12), gS(9,8), gS(2,12), gS(0,15), gS(-4,11),
 gS(-7,8), gS(-3,6), gS(-10,-1), gS(-1,-13),
};

const int  ROOK_MOBILITY[15] = {
 gS(-15,-98), gS(-10,-53), gS(-10,-22), gS(-8,-9), gS(-9,8), gS(-12,20), gS(-6,23),
 gS(-2,25), gS(3,27), gS(8,32), gS(7,36), gS(12,39), gS(16,39), gS(29,27),
 gS(83,0),
};

const int  QUEEN_MOBILITY[28] = {
 gS(-26,-65), gS(2,-77), gS(3,-90), gS(3,-84), gS(4,-66), gS(4,-45), gS(4,-25),
 gS(5,-11), gS(6,0), gS(7,10), gS(7,18), gS(7,25), gS(5,34), gS(2,43),
 gS(1,52), gS(-4,60), gS(-7,65), gS(-9,67), gS(-8,67), gS(-2,63), gS(8,52),
 gS(18,44), gS(22,39), gS(28,35), gS(24,29), gS(26,29), gS(26,28), gS(26,28),
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
        [PAWN] = gS(70,112),
        [ROOK] = gS(382,744),
        [KNIGHT] = gS(269,415),
        [BISHOP] = gS(288,449),
        [QUEEN] = gS(861,1369),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(13,74);

const int  PAWN_SUPPORTED = gS(7,5);

const int  DOUBLED_PAWN_PENALTY = gS(-12,-26);

const int  ISOLATED_PAWN_PENALTY = gS(-9,-13);

const int  BISHOP_RAMMED_PENALTY = gS(-1,-11);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
 gS(0,0), gS(1,29), gS(-9,24), gS(-13,8),
 gS(-7,-5), gS(-3,-11), gS(12,-17), gS(-1,-25),
 gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
 gS(0,0), gS(45,81), gS(-10,13), gS(-4,-3),
 gS(-6,-15), gS(-6,-26), gS(0,-37), gS(6,-44),
 gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {   gS(24,9), gS(12,6),};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,13), gS(6,4) };

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
 gS(0,0), gS(-16,-24), gS(-19,-13), gS(-7,20),
 gS(16,53), gS(40,109), gS(70,108), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
 gS(9,21), gS(5,21), gS(0,6), gS(0,3),
 gS(-4,0), gS(-17,1), gS(-9,19), gS(15,11),
};


const int  KING_PSQT_BLACK[64] = {
 gS(-10,-29), gS(-15,-17), gS(-14,-13), gS(-13,-9), gS(-14,-13), gS(-15,-11), gS(-15,-13), gS(-10,-28),
 gS(-11,-20), gS(-8,22), gS(-8,16), gS(-12,2), gS(-12,1), gS(-9,17), gS(-9,34), gS(-11,-19),
 gS(-8,-8), gS(-1,36), gS(-2,33), gS(-7,17), gS(-8,15), gS(-1,38), gS(-1,40), gS(-9,-10),
 gS(-11,-16), gS(-3,21), gS(0,27), gS(-10,28), gS(-6,24), gS(0,25), gS(3,20), gS(-15,-16),
 gS(-15,-36), gS(-1,5), gS(1,17), gS(-10,27), gS(-1,25), gS(6,16), gS(22,3), gS(-21,-31),
 gS(-22,-27), gS(-6,0), gS(-4,7), gS(-17,21), gS(-3,17), gS(0,9), gS(9,-4), gS(-29,-21),
 gS(9,-18), gS(7,2), gS(-13,10), gS(-57,19), gS(-26,12), gS(-34,16), gS(10,-1), gS(12,-28),
 gS(-20,-53), gS(32,-31), gS(-7,-13), gS(-57,-22), gS(-9,-47), gS(-59,-8), gS(9,-31), gS(1,-81),
};

const int  PAWN_PSQT_BLACK[64] = {
 gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
 gS(60,89), gS(53,93), gS(60,95), gS(55,83), gS(58,87), gS(54,89), gS(31,83), gS(30,86),
 gS(-6,57), gS(14,50), gS(27,46), gS(26,26), gS(42,28), gS(101,33), gS(47,39), gS(11,43),
 gS(-8,29), gS(0,24), gS(1,22), gS(17,5), gS(29,11), gS(36,15), gS(16,20), gS(1,15),
 gS(-13,14), gS(-10,15), gS(0,7), gS(2,5), gS(9,9), gS(15,9), gS(6,7), gS(-9,3),
 gS(-14,6), gS(-18,6), gS(-9,11), gS(-6,14), gS(2,17), gS(9,15), gS(8,-1), gS(-6,-4),
 gS(-13,12), gS(-15,11), gS(-17,20), gS(-9,21), gS(-14,33), gS(17,24), gS(14,4), gS(-9,-10),
 gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
 gS(20,29), gS(16,34), gS(10,36), gS(13,33), gS(14,31), gS(17,37), gS(17,36), gS(23,39),
 gS(-2,28), gS(-6,32), gS(16,28), gS(23,31), gS(25,30), gS(31,14), gS(14,19), gS(25,17),
 gS(-7,20), gS(33,7), gS(19,18), gS(36,8), gS(44,5), gS(34,17), gS(40,7), gS(16,9),
 gS(-3,10), gS(8,12), gS(16,14), gS(37,7), gS(32,5), gS(29,4), gS(23,5), gS(11,6),
 gS(-17,2), gS(-15,13), gS(-14,14), gS(-3,6), gS(-3,3), gS(-6,4), gS(7,4), gS(-14,-2),
 gS(-23,-12), gS(-12,-3), gS(-17,-4), gS(-3,-11), gS(-4,-13), gS(-11,-10), gS(5,-10), gS(-17,-14),
 gS(-38,-16), gS(-12,-22), gS(-12,-14), gS(-2,-21), gS(-4,-21), gS(-1,-30), gS(-1,-23), gS(-33,-20),
 gS(-12,-17), gS(-10,-14), gS(-8,-11), gS(0,-17), gS(-1,-19), gS(-1,-10), gS(-1,-17), gS(-10,-28),
};

const int  BISHOP_PSQT_BLACK[64] = {
 gS(-21,-7), gS(-12,-2), gS(-17,-3), gS(-19,0), gS(-15,-1), gS(-22,-11), gS(-9,-4), gS(-19,-10),
 gS(-34,-7), gS(-3,3), gS(-9,2), gS(-9,3), gS(-14,2), gS(-2,5), gS(-16,5), gS(-10,-7),
 gS(-11,-5), gS(1,6), gS(14,7), gS(-1,2), gS(13,7), gS(25,18), gS(19,10), gS(-2,-3),
 gS(-19,-4), gS(14,6), gS(-1,6), gS(23,12), gS(15,16), gS(14,8), gS(6,10), gS(-18,2),
 gS(-14,-9), gS(0,-1), gS(4,12), gS(16,12), gS(18,9), gS(-3,6), gS(3,0), gS(-5,-14),
 gS(-8,-12), gS(3,-1), gS(3,3), gS(7,6), gS(3,5), gS(6,1), gS(5,-6), gS(6,-5),
 gS(0,-9), gS(5,-20), gS(8,-11), gS(-5,-4), gS(0,-3), gS(8,-15), gS(18,-12), gS(2,-24),
 gS(4,-18), gS(1,-8), gS(-11,-8), gS(-14,-4), gS(-8,-9), gS(-6,-5), gS(-5,-11), gS(0,-12),
};

const int  KNIGHT_PSQT_BLACK[64] = {
 gS(-46,-39), gS(-12,-15), gS(-16,-8), gS(-10,-9), gS(-9,-9), gS(-13,-7), gS(-10,-10), gS(-22,-22),
 gS(-18,-21), gS(-11,-16), gS(22,-10), gS(21,15), gS(15,12), gS(18,-11), gS(-6,-11), gS(-6,-13),
 gS(-27,-26), gS(-2,-4), gS(19,22), gS(13,24), gS(31,17), gS(39,32), gS(20,0), gS(-4,-16),
 gS(-4,-9), gS(8,5), gS(24,25), gS(22,40), gS(10,44), gS(34,31), gS(10,19), gS(15,1),
 gS(-9,-10), gS(3,5), gS(9,31), gS(13,36), gS(13,33), gS(13,31), gS(21,9), gS(1,1),
 gS(-22,-37), gS(-4,-9), gS(-1,-1), gS(11,16), gS(15,13), gS(5,-4), gS(2,-8), gS(-12,-24),
 gS(-31,-21), gS(-28,-6), gS(-12,-18), gS(0,-2), gS(-4,0), gS(-1,-20), gS(-16,-3), gS(-14,-9),
 gS(-24,-23), gS(-18,-26), gS(-32,-23), gS(-10,-3), gS(-2,-8), gS(-6,-15), gS(-17,-19), gS(-31,-23),
};

const int  QUEEN_PSQT_BLACK[64] = {
 gS(-10,-8), gS(3,6), gS(3,7), gS(10,17), gS(19,25), gS(14,19), gS(6,10), gS(9,9),
 gS(-16,-8), gS(-49,9), gS(-5,3), gS(-5,13), gS(-1,36), gS(21,26), gS(-12,6), gS(11,11),
 gS(-22,-17), gS(-13,-13), gS(-8,1), gS(-4,13), gS(18,29), gS(41,47), gS(46,34), gS(9,28),
 gS(-17,-22), gS(-8,0), gS(-14,-4), gS(-12,26), gS(0,37), gS(1,43), gS(7,42), gS(2,27),
 gS(-8,-22), gS(-6,-6), gS(-8,-5), gS(-12,27), gS(-8,24), gS(-16,30), gS(1,13), gS(-8,18),
 gS(-15,-26), gS(0,-23), gS(-5,-5), gS(-5,-18), gS(-7,-12), gS(-9,1), gS(5,-11), gS(-6,-14),
 gS(-13,-28), gS(-2,-34), gS(5,-57), gS(-1,-31), gS(3,-36), gS(4,-62), gS(6,-44), gS(-15,-17),
 gS(-9,-35), gS(-14,-36), gS(-9,-51), gS(0,-47), gS(0,-45), gS(-25,-36), gS(-23,-24), gS(-19,-21),
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
