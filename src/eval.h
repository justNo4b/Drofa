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
const int  KING_MED_DANGER   = gS(-7,-15);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(0,10);   // слабый пешечный щит
const int  KING_SAFE         = gS(3,22);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-21,-104), gS(-9,-45), gS(-4,-18), gS(-1,-2), gS(1,12), gS(3,22), gS(2,29),
           gS(1,31), gS(2,33), gS(4,32), gS(11,27), gS(26,18), gS(43,24), gS(74,-8),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-9,-74), gS(0,-28), gS(3,-2), gS(6,12), gS(8,20),
           gS(11,27), gS(15,26), gS(22,18), gS(31,2),
};

const int KING_MOBILITY[9] = {
           gS(12,-1), gS(15,-2), gS(9,5), gS(5,9), gS(-2,8),
           gS(-8,5), gS(-6,3), gS(-17,-2), gS(-14,-15),
};

const int ROOK_MOBILITY[15] = {
           gS(-21,-102), gS(-13,-66), gS(-9,-33), gS(-7,-13), gS(-8,5), gS(-10,19), gS(-4,23),
           gS(0,26), gS(3,31), gS(8,36), gS(10,40), gS(16,43), gS(21,43), gS(36,29),
           gS(89,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-39,-106), gS(-19,-172), gS(-6,-133), gS(-3,-78), gS(-2,-44), gS(0,-22), gS(1,-4),
           gS(2,7), gS(5,16), gS(7,23), gS(8,30), gS(9,35), gS(9,39), gS(9,45),
           gS(7,51), gS(5,55), gS(4,57), gS(3,58), gS(3,58), gS(9,53), gS(12,48),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
};


const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
const int COUNT_TO_POWER[8] = {0, 0, 50, 75, 80, 88, 95, 100};
const int SAFE_SHIELD_SAFETY = -50;

/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 * 'vBishopEG': 288, 'vBishopOP': 336, 'vKnightEG': 310, 'vKnightOP': 316, 'vPawnEG': 112, 'vQueenEG': 1005, 'vQueenOP': 1100, 'vRookEG': 500, 'vRookOP': 470}
    Finished game 201 (Drofa_dev vs Drofa_1.2.9): 1-0 {White wins by adjudication}
    Score of Drofa_dev vs Drofa_1.2.9: 67 - 63 - 70  [0.510] 200
    {'vBishopEG': 357, 'vBishopOP': 336, 'vKnightEG': 328, 'vKnightOP': 304, 'vPawnEG': 86, 'vQueenEG': 995, 'vQueenOP': 1190, 'vRookEG': 565, 'vRookOP': 465}
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(75,126),
        [ROOK] = gS(413,748),
        [KNIGHT] = gS(278,413),
        [BISHOP] = gS(302,442),
        [QUEEN] = gS(943,1361),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(15,76);

const int PAWN_SUPPORTED = gS(8,5);

const int DOUBLED_PAWN_PENALTY = gS(-8,-31);

const int ISOLATED_PAWN_PENALTY = gS(-3,-5);

const int PAWN_BLOCKED = gS(1,27);

const int PASSER_BLOCKED = gS(1,46);

const int PAWN_DISTORTION = gS(-1,-1);

const int BISHOP_RAMMED_PENALTY = gS(-1,-10);

const int BISHOP_CENTER_CONTROL = gS(10,9);

const int MINOR_BEHIND_PAWN = gS(6,24);

const int MINOR_BEHIND_PASSER = gS(8,14);

const int KING_AHEAD_PASSER = gS(-15,10);

const int KING_EQUAL_PASSER = gS(3,3);

const int KING_BEHIND_PASSER = gS(7,-4);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(2,31), gS(-5,27), gS(-8,11),
           gS(-5,0), gS(-4,-4), gS(11,-9), gS(0,-18),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(52,73), gS(-16,13), gS(-4,-3),
           gS(-5,-16), gS(-2,-27), gS(5,-38), gS(14,-45),
           gS(0,-10),
};


/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(23,9), gS(24,10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(6,9), gS(16,9),
};

const int BISHOP_ENEMY_PAWNS[9] = {
           gS(3,-5), gS(-1,0), gS(-3,2), gS(-3,5),
           gS(-1,6), gS(0,6), gS(2,5), gS(6,3),
           gS(4,1),
};

const int HANGING_PIECE[5] = {
           gS(-11,-2), gS(-26,-17), gS(-25,-52), gS(-32,-73), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-30), gS(-15,-16), gS(-8,20),
           gS(14,54), gS(24,118), gS(73,133), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(9,15), gS(5,15), gS(0,3), gS(-1,0),
           gS(-4,-1), gS(-15,2), gS(-7,16), gS(11,11),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-97), gS(0,-47), gS(0,-33), gS(0,-18), gS(0,-32), gS(-1,-27), gS(-2,-25), gS(-12,-89),
           gS(-13,-37), gS(0,35), gS(5,24), gS(0,5), gS(0,4), gS(3,25), gS(0,46), gS(-12,-36),
           gS(-3,-3), gS(28,40), gS(33,35), gS(12,21), gS(12,18), gS(35,37), gS(28,43), gS(-7,-8),
           gS(-9,-10), gS(22,21), gS(29,29), gS(0,32), gS(8,27), gS(33,27), gS(29,21), gS(-36,-10),
           gS(-22,-34), gS(22,1), gS(25,18), gS(-16,33), gS(0,29), gS(9,20), gS(23,5), gS(-53,-22),
           gS(-52,-16), gS(-9,1), gS(-7,11), gS(-29,26), gS(-17,23), gS(-16,16), gS(-8,1), gS(-51,-14),
           gS(0,-14), gS(-1,4), gS(-23,14), gS(-64,24), gS(-44,18), gS(-46,22), gS(0,2), gS(-2,-22),
           gS(-26,-57), gS(3,-30), gS(-28,-8), gS(-36,-29), gS(-27,-39), gS(-54,-8), gS(-3,-29), gS(-13,-75),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(60,71), gS(49,97), gS(66,93), gS(73,78), gS(81,77), gS(84,81), gS(-8,101), gS(-23,81),
           gS(-2,48), gS(28,55), gS(41,45), gS(44,25), gS(55,25), gS(94,32), gS(61,40), gS(9,34),
           gS(-4,20), gS(5,26), gS(7,22), gS(21,7), gS(30,11), gS(30,15), gS(12,22), gS(-4,9),
           gS(-15,4), gS(-6,13), gS(-4,5), gS(4,3), gS(9,4), gS(4,6), gS(2,6), gS(-14,-5),
           gS(-19,-2), gS(-15,3), gS(-13,6), gS(-12,7), gS(-4,10), gS(-3,9), gS(1,-3), gS(-15,-12),
           gS(-17,1), gS(-11,5), gS(-17,16), gS(-10,16), gS(-14,22), gS(5,17), gS(7,0), gS(-17,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,26), gS(22,33), gS(0,41), gS(8,36), gS(11,36), gS(24,36), gS(40,31), gS(51,27),
           gS(-3,27), gS(-6,32), gS(16,28), gS(27,29), gS(25,29), gS(43,12), gS(8,24), gS(15,20),
           gS(-9,19), gS(31,5), gS(20,16), gS(42,5), gS(53,0), gS(51,6), gS(71,-8), gS(14,11),
           gS(-8,10), gS(8,9), gS(16,12), gS(37,4), gS(30,4), gS(28,3), gS(30,0), gS(12,4),
           gS(-20,0), gS(-11,10), gS(-14,12), gS(-4,5), gS(-5,4), gS(-8,8), gS(6,4), gS(-11,-3),
           gS(-24,-17), gS(-8,-5), gS(-16,-6), gS(-8,-12), gS(-6,-12), gS(-9,-9), gS(9,-9), gS(-15,-19),
           gS(-54,-11), gS(-17,-19), gS(-10,-14), gS(-4,-19), gS(-3,-19), gS(-2,-23), gS(-2,-24), gS(-55,-11),
           gS(-16,-15), gS(-10,-13), gS(-6,-10), gS(0,-18), gS(0,-18), gS(-3,-8), gS(0,-16), gS(-11,-24),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-52,3), gS(-44,7), gS(-79,18), gS(-81,26), gS(-70,21), gS(-92,17), gS(-15,0), gS(-50,0),
           gS(-48,0), gS(-12,2), gS(-8,8), gS(-14,17), gS(-5,15), gS(0,7), gS(-28,5), gS(-22,-4),
           gS(2,1), gS(2,8), gS(22,2), gS(7,8), gS(23,7), gS(8,8), gS(23,2), gS(15,-1),
           gS(-10,0), gS(18,5), gS(3,6), gS(21,15), gS(19,15), gS(14,4), gS(19,3), gS(-6,1),
           gS(0,-13), gS(5,0), gS(10,7), gS(17,9), gS(24,8), gS(2,6), gS(12,0), gS(2,-14),
           gS(-2,-16), gS(10,-7), gS(6,-4), gS(13,1), gS(8,3), gS(11,-7), gS(11,-9), gS(13,-11),
           gS(8,-32), gS(8,-33), gS(10,-16), gS(3,-1), gS(7,-3), gS(8,-15), gS(16,-28), gS(3,-43),
           gS(5,-38), gS(1,-16), gS(1,-3), gS(0,-6), gS(0,-2), gS(0,-8), gS(-2,-11), gS(2,-40),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-144,-66), gS(-37,-18), gS(-67,12), gS(-14,0), gS(0,1), gS(-55,14), gS(-30,-10), gS(-90,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-4), gS(49,17), gS(37,17), gS(52,-12), gS(-21,-2), gS(-7,-21),
           gS(-19,-18), gS(12,-4), gS(32,30), gS(32,30), gS(58,19), gS(43,24), gS(27,-11), gS(4,-16),
           gS(0,-10), gS(7,5), gS(26,28), gS(25,39), gS(17,35), gS(39,22), gS(15,8), gS(8,-7),
           gS(-4,-8), gS(10,8), gS(14,30), gS(20,32), gS(23,34), gS(19,32), gS(24,11), gS(6,0),
           gS(-19,-34), gS(1,-7), gS(5,8), gS(13,23), gS(17,21), gS(13,6), gS(7,-2), gS(-5,-27),
           gS(-21,-17), gS(-28,-1), gS(-7,-12), gS(6,5), gS(1,6), gS(0,-11), gS(-17,-4), gS(-11,-4),
           gS(-40,-46), gS(-12,-29), gS(-11,-18), gS(-8,2), gS(0,1), gS(-7,-12), gS(-10,-21), gS(-40,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-4,3), gS(0,19), gS(0,33), gS(3,45), gS(9,55), gS(31,49), gS(8,51), gS(15,37),
           gS(-18,0), gS(-57,41), gS(-17,30), gS(-41,74), gS(-46,112), gS(0,72), gS(-56,74), gS(-18,40),
           gS(-15,-16), gS(-8,-16), gS(-11,16), gS(-12,43), gS(0,61), gS(11,78), gS(28,48), gS(0,60),
           gS(-5,-32), gS(-3,0), gS(-9,4), gS(-17,50), gS(-15,66), gS(-10,74), gS(1,59), gS(5,32),
           gS(0,-42), gS(3,-14), gS(-3,-8), gS(-10,28), gS(-6,27), gS(-9,26), gS(10,3), gS(0,7),
           gS(0,-58), gS(10,-42), gS(4,-22), gS(3,-31), gS(3,-29), gS(0,-12), gS(15,-33), gS(4,-47),
           gS(0,-67), gS(7,-64), gS(16,-79), gS(9,-50), gS(13,-54), gS(17,-92), gS(14,-86), gS(-10,-58),
           gS(1,-70), gS(5,-82), gS(7,-88), gS(10,-69), gS(11,-75), gS(6,-90), gS(9,-85), gS(-1,-75),
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
int kingSafety(const Board &, Color, int, evalBits *);

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
