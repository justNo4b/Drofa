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
const int KING_HIGH_DANGER =  gS(-13,-14);   // применять когда жив ферзь и мы не рокированы
const int KING_MED_DANGER  =  gS( -4,-33);   // мы рокированы, но пешечный щит кривой
const int KING_LOW_DANGER  =  gS(0,0);   // слабый пешечный щит
const int KING_SAFE        =  gS(0,32);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
            gS(-25, -25), gS(-13,-17), gS(  0,-13), gS(  4, -5), gS(  8,  0), gS( 10,  6), gS( 12,  9),
            gS( 14,  12), gS( 16, 15), gS( 18, 17), gS( 20, 19), gS( 22, 21), gS( 23, 23), gS( 23, 23),
};

const int KNIGHT_MOBILITY[9] = {
            gS(-25,-25), gS(-10,-17), gS( 0,-10), gS( 5,  0), gS( 9, 5),
            gS( 13, 10), gS( 18, 18), gS(21, 24), gS(21, 24)
};

const int KING_MOBILITY[9] = {
            gS( -3, -4), gS(  0, -1), gS( 5,  0), gS( 5,  2), gS( 3, 5),
            gS( -1,  8), gS( -3, 11), gS(-5, 11), gS(-7,  8)
};

const int ROOK_MOBILITY[15] = {
            gS(-21,-13), gS(-13, -7), gS( -7, -5), gS( -3, -1), gS( -1,  0), gS( 0,  2), gS(  2,  4),
            gS(  5,  6), gS(  7,  8), gS( 10, 12), gS( 13, 14), gS( 14, 16), gS( 14, 16), gS( 13, 15),
            gS( 12, 15),
};

const int QUEEN_MOBILITY[28] = {
            gS(-34,-50), gS(-17,-20), gS(-11,-13), gS( -7, -9), gS( -3, -5), gS( -1, -3), gS(  0, -1),
            gS(  1,  0), gS(  3,  1), gS(  5,  3), gS(  7,  5), gS(  9,  7), gS( 11,  9), gS( 13, 11),
            gS( 15, 13), gS( 16, 15), gS( 17, 17), gS( 18, 18), gS( 19, 19), gS( 21, 21), gS( 23, 23),
            gS( 25, 25), gS( 25, 25), gS( 26, 26), gS( 26, 26), gS( 26, 26), gS( 26, 26), gS( 26, 26),
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
        [PAWN] = gS(68,114),
        [ROOK] = gS(376,706),
        [KNIGHT] = gS(280,409),
        [BISHOP] = gS(299,433),
        [QUEEN] = gS(991,1274),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_RAMMED_PENALTY = gS(-3,-11);

const int PAWN_SUPPORTED = gS(11,2);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
        gS(0,  0), gS(0, 15), gS(0, 7), gS(0, 3), gS(0, 0),
        gS(0, -1), gS(0, -5), gS(0, -10), gS(0, -10)
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
        gS(0,  0), gS(0, 15), gS(0, 7), gS(0, 3), gS(0, 0),
        gS(0, -1), gS(0, -5), gS(0, -10), gS(0, -10)
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(50, 25), gS(40, 15)};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = { gS (12, 37), gS ( 2, 27) };

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
                gS(0,0), gS(-20,3), gS(-16,4), gS(-10,24),
                gS(19,40), gS(53,92), gS(85,99), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
                gS(-3,30), gS(-2,26), gS(-6,11), gS(-10,0),
                 gS(3,1), gS(20,4), gS(13,16), gS(9,8),
};

/**
 * @brief Penalties given to a player for having a doubled pawn (opening/endgame)
 */
const int DOUBLED_PAWN_PENALTY = gS(-8,-28);

/**
 * @brief Penalties given to a player for having an isolated pawn (opening/endgame)
 */
const int ISOLATED_PAWN_PENALTY = gS(-7,-15);

/**
 * @brief Bonuses given to a player for having bishops on black and white squares (opening/endgame)
 */
const int BISHOP_PAIR_BONUS = gS(18,68);

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
