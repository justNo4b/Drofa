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

struct gS
{
    int16_t OP;
    int16_t EG;

    gS() : OP(0), EG(0) {}
    gS( int16_t o, int16_t e) : OP(o), EG(e) {}

    inline gS operator+(gS other_gS){
        return gS(OP + other_gS.OP, EG + other_gS.EG);
    }

    inline gS operator-(gS other_gS){
        return gS(OP - other_gS.OP, EG - other_gS.EG);
    }
};

struct evalBits{
    U64 EnemyPawnAttackMap[2];
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
const int KING_HIGH_DANGER = -17; // применять когда жив ферзь и мы не рокированы
const int KING_MED_DANGER = -5;   // мы рокированы, но пешечный щит кривой
const int KING_LOW_DANGER = 0;   // слабый пешечный щит
const int KING_SAFE = 5;         // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY [2][14] = {
            [OPENING] = {-25, -13,   0,   4,  8, 10, 12, 14, 16, 18, 20, 22, 23, 23},
            [ENDGAME] = {-25, -17, -13,  -5,  0,  6,  9, 12, 15, 17, 19, 21, 23, 23},
};

const int KNIGHT_MOBILITY [2][9] = {
            [OPENING] = {-25, -10,  0,  5,  9, 13, 18, 21, 21},
            [ENDGAME] = {-25, -17,-10,  0,  5, 10, 18, 24, 24},
};

const int KING_MOBILITY [2][9] = {
            [OPENING] = {-3,  0,  5,  5,  3, -1,  -3, -5, -7},
            [ENDGAME] = {-4, -1,  0,  2,  5,  8,  11, 11,  8},
};

const int ROOK_MOBILITY [2][15] = {
            [OPENING] = {-21,-13,  -7,  -3, -1,  0,  2,  5,  7, 10, 13, 14, 14, 13, 12},
            [ENDGAME] = {-13, -7,  -5,  -1,  0,  2,  4,  6,  8, 12, 14, 16, 16, 15, 15},
};

const int QUEEN_MOBILITY [2][28] = {
            [OPENING] = {-34, -17,  -11,   -7,  -3,  -1,   0,  1,  3, 5, 7, 9, 11, 13, 15, 16, 17, 18, 19, 21, 23, 25, 25, 26, 26, 26},
            [ENDGAME] = {-50, -20,  -13,   -9,  -5,  -3,  -1,  0,  1, 3, 5, 7,  9, 11, 13, 15, 17, 18, 19, 21, 23, 25, 25, 26, 26, 26},
};

/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 * 'vBishopEG': 288, 'vBishopOP': 336, 'vKnightEG': 310, 'vKnightOP': 316, 'vPawnEG': 112, 'vQueenEG': 1005, 'vQueenOP': 1100, 'vRookEG': 500, 'vRookOP': 470}
    Finished game 201 (Drofa_dev vs Drofa_1.2.9): 1-0 {White wins by adjudication}
    Score of Drofa_dev vs Drofa_1.2.9: 67 - 63 - 70  [0.510] 200

    {'vBishopEG': 357, 'vBishopOP': 336, 'vKnightEG': 328, 'vKnightOP': 304, 'vPawnEG': 86, 'vQueenEG': 995, 'vQueenOP': 1190, 'vRookEG': 565, 'vRookOP': 465}

 */
const int MATERIAL_VALUES[2][6] = {
    [OPENING] = {
        [PAWN] = 100,
        [ROOK] = 465,
        [KNIGHT] = 304,
        [BISHOP] = 336,
        [QUEEN] = 1190,
        [KING] = 0
    },
    [ENDGAME] = {
        [PAWN] = 86,
        [ROOK] = 565,
        [KNIGHT] = 328,
        [BISHOP] = 357,
        [QUEEN] = 995,
        [KING] = 0
    }
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {[OPENING] = 40, [ENDGAME] = 40};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {[OPENING] = 15, [ENDGAME] = 15};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_BONUS[2] = {[OPENING] = 10, [ENDGAME] = 35};
const int PASSED_PAWN_RANKS[2][8] = {
            [OPENING] = {0,  4,  7, 10, 15, 20, 45, 0}, 
            [ENDGAME] = {0, 10, 15, 35, 40, 45, 75, 0}
};

/**
 * @brief Penalties given to a player for having a doubled pawn (opening/endgame)
 */
const int DOUBLED_PAWN_PENALTY[2] = {[OPENING] = -20, [ENDGAME] = -30};

/**
 * @brief Penalties given to a player for having an isolated pawn (opening/endgame)
 */
const int ISOLATED_PAWN_PENALTY[2] = {[OPENING] = -15, [ENDGAME] = -30};

/**
 * @brief Bonuses given to a player for having bishops on black and white squares (opening/endgame)
 */
const int BISHOP_PAIR_BONUS[2] = {[OPENING] = 20, [ENDGAME] = 20};

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
    gS evaluateQUEEN(const Board &, Color, evalBits *);
    gS evaluateROOK(const Board &, Color, evalBits *);
    gS evaluateBISHOP(const Board &, Color, evalBits *);
    gS evaluateKNIGHT(const Board &, Color, evalBits *);
    gS evaluateKING(const Board &, Color, const evalBits &);

  /**@}*/

evalBits Setupbits(const Board &);

/**
 * @brief Returns the number of passed pawns that the given color has on the
 * given board
 *
 * @param board Board to check for passed pawns
 * @param color Color of player to check for passed pawns
 * @return
 */
gS passedPawns(const Board &, Color);

/**
 * @brief Returns the number doubled pawns for the given color on the
 * given board
 *
 * A doubled pawn is defined as being one of two pawns on the same file. Each
 * extra pawn on the same file is counted as another doubled pawn. For
 * example, a file with 2 pawns would count as having one doubled pawn
 * and a file with 3 pawns would count has having two.
 *
 * @param board Board to check for doubled pawns
 * @param color Color of doubled pawns to check
 * @return The number of doubled pawns that the given color has on the given
 * board
 */
int doubledPawns(const Board &, Color);

/**
 * @brief Returns the number of isolated pawns on the given board for the
 * given color
 *
 * @param board Board to check for isolated pawns
 * @param color Color of isolated pawns to check
 * @return The number of isolated pawns that the given color has on the given
 * board
 */
int isolatedPawns(const Board &, Color);

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
