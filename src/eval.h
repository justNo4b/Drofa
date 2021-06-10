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
const int  KING_HIGH_DANGER  = gS(-17,-26);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-6,-16);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(-1,10);   // слабый пешечный щит
const int  KING_SAFE         = gS(2,19);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-23,-103), gS(-11,-44), gS(-5,-17), gS(-1,-2), gS(1,11), gS(3,22), gS(3,28),
           gS(1,31), gS(3,34), gS(5,32), gS(11,29), gS(27,19), gS(45,28), gS(75,-3),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-6,-72), gS(2,-25), gS(5,0), gS(8,14), gS(11,22),
           gS(13,28), gS(18,27), gS(25,19), gS(34,4),
};

const int KING_MOBILITY[9] = {
           gS(9,-2), gS(14,-6), gS(9,1), gS(6,6), gS(0,6),
           gS(-7,3), gS(-4,2), gS(-18,1), gS(-14,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-102), gS(-15,-66), gS(-10,-32), gS(-8,-11), gS(-9,7), gS(-10,20), gS(-5,24),
           gS(0,27), gS(3,31), gS(8,37), gS(11,40), gS(17,42), gS(22,42), gS(37,29),
           gS(89,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-42,-106), gS(-20,-172), gS(-8,-132), gS(-4,-76), gS(-2,-43), gS(-1,-21), gS(0,-3),
           gS(2,8), gS(5,17), gS(7,23), gS(8,30), gS(9,34), gS(10,38), gS(9,43),
           gS(8,49), gS(6,53), gS(5,56), gS(4,57), gS(4,57), gS(9,53), gS(12,48),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
};

const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
const int COUNT_TO_POWER[8] = {0, 0, 40, 65, 80, 87, 95, 100};
const int SAFE_SHIELD_SAFETY = -50;

/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 * 'vBishopEG': 288, 'vBishopOP': 336, 'vKnightEG': 310, 'vKnightOP': 316, 'vPawnEG': 112, 'vQueenEG': 1005, 'vQueenOP': 1100, 'vRookEG': 500, 'vRookOP': 470}
    Finished game 201 (Drofa_dev vs Drofa_1.2.9): 1-0 {White wins by adjudication}
    Score of Drofa_dev vs Drofa_1.2.9: 67 - 63 - 70  [0.510] 200
    {'vBishopEG': 357, 'vBishopOP': 336, 'vKnightEG': 328, 'vKnightOP': 304, 'vPawnEG': 86, 'vQueenEG': 995, 'vQueenOP': 1190, 'vRookEG': 565, 'vRookOP': 465}
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(70,126),
        [ROOK] = gS(415,752),
        [KNIGHT] = gS(278,414),
        [BISHOP] = gS(304,448),
        [QUEEN] = gS(949,1365),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(14,76);

const int PAWN_SUPPORTED = gS(13,5);

const int PAWN_BLOCKED = gS(1,26);

const int PASSER_BLOCKED = gS(1,44);

const int PAWN_DISTORTION = gS(0,0);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(10,7);

const int MINOR_BEHIND_PAWN = gS(7,12);

const int MINOR_BEHIND_PASSER = gS(8,13);

const int KING_AHEAD_PASSER = gS(-17,10);

const int KING_EQUAL_PASSER = gS(7,9);

const int KING_BEHIND_PASSER = gS(5,-5);

const int KING_OPEN_FILE = gS(-49,-1);

const int KING_OWN_SEMI_FILE = gS(-31,17);

const int KING_ENEMY_SEMI_LINE = gS(-16,8);

const int KING_ATTACK_PAWN = gS(21,41);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,35), gS(-3,26), gS(-5,11),
           gS(-4,0), gS(-3,-3), gS(11,-8), gS(1,-18),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(44,43), gS(-16,18), gS(-4,-1),
           gS(-4,-14), gS(-2,-25), gS(4,-35), gS(14,-42),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {  gS(22,10), gS(22,11), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,3), gS(15,10), };

const int PAWN_CONNECTED[2] = {
           gS(4,3), gS(6,12),
};

const int DOUBLED_PAWN_PENALTY[2] = {
           gS(-6,-20), gS(-13,-45),
};

const int ISOLATED_PAWN_PENALTY[2] = {
           gS(-4,-5), gS(-9,-11),
};


/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn 
 * and OUR pieces attacking enemy pawns - [PAWN] 
 */
const int HANGING_PIECE[5] = {
           gS(-3,11), gS(-26,-15), gS(-26,-49), gS(-31,-76), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-12,-27), gS(-15,-14), gS(-7,21),
           gS(17,56), gS(27,120), gS(73,133), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(12,18), gS(5,15), gS(0,4), gS(0,0),
           gS(-4,0), gS(-14,2), gS(-9,15), gS(11,13),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-98), gS(0,-47), gS(0,-33), gS(0,-17), gS(0,-32), gS(0,-27), gS(-1,-25), gS(-12,-90),
           gS(-13,-37), gS(0,30), gS(5,22), gS(0,5), gS(0,2), gS(3,19), gS(0,38), gS(-12,-37),
           gS(-2,-6), gS(27,31), gS(33,29), gS(12,19), gS(12,15), gS(35,27), gS(26,30), gS(-7,-13),
           gS(-9,-10), gS(22,13), gS(30,27), gS(0,32), gS(7,26), gS(32,23), gS(28,11), gS(-38,-13),
           gS(-22,-33), gS(22,0), gS(26,19), gS(-14,36), gS(0,32), gS(8,20), gS(24,2), gS(-53,-23),
           gS(-52,-13), gS(-9,2), gS(-7,14), gS(-28,31), gS(-16,27), gS(-17,18), gS(-9,3), gS(-48,-11),
           gS(1,-9), gS(-2,6), gS(-22,17), gS(-61,26), gS(-44,21), gS(-43,23), gS(0,4), gS(-1,-18),
           gS(-26,-53), gS(0,-24), gS(-29,-3), gS(-32,-23), gS(-21,-36), gS(-54,-3), gS(-5,-24), gS(-15,-69),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,73), gS(46,95), gS(65,89), gS(72,76), gS(79,75), gS(83,80), gS(-6,100), gS(-20,85),
           gS(-2,47), gS(24,50), gS(37,40), gS(41,20), gS(52,20), gS(89,28), gS(55,37), gS(10,35),
           gS(-3,20), gS(3,22), gS(4,17), gS(19,2), gS(27,6), gS(27,11), gS(9,19), gS(-1,10),
           gS(-12,5), gS(-8,11), gS(-4,1), gS(2,0), gS(8,1), gS(3,3), gS(0,4), gS(-11,-3),
           gS(-18,-1), gS(-17,1), gS(-14,4), gS(-13,5), gS(-6,8), gS(-6,7), gS(-1,-2), gS(-13,-11),
           gS(-15,2), gS(-12,5), gS(-17,15), gS(-8,15), gS(-14,21), gS(6,17), gS(4,1), gS(-12,-16),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,27), gS(23,33), gS(0,42), gS(8,38), gS(13,37), gS(25,37), gS(41,31), gS(53,27),
           gS(-1,23), gS(-4,27), gS(17,23), gS(29,23), gS(26,24), gS(44,9), gS(10,22), gS(16,18),
           gS(-8,16), gS(31,2), gS(21,13), gS(44,2), gS(54,-3), gS(51,4), gS(70,-10), gS(16,8),
           gS(-8,9), gS(9,7), gS(16,10), gS(38,2), gS(30,3), gS(28,2), gS(28,0), gS(13,2),
           gS(-20,0), gS(-12,11), gS(-14,12), gS(-4,5), gS(-5,5), gS(-7,8), gS(5,5), gS(-9,-3),
           gS(-25,-14), gS(-8,-3), gS(-16,-4), gS(-8,-9), gS(-8,-8), gS(-8,-6), gS(9,-7), gS(-14,-17),
           gS(-55,-8), gS(-18,-15), gS(-11,-10), gS(-5,-15), gS(-4,-15), gS(-2,-19), gS(-2,-20), gS(-53,-9),
           gS(-18,-12), gS(-11,-10), gS(-7,-8), gS(0,-15), gS(-1,-14), gS(-3,-4), gS(-1,-12), gS(-11,-21),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,5), gS(-43,8), gS(-80,17), gS(-81,23), gS(-69,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-49,1), gS(-13,4), gS(-8,7), gS(-14,15), gS(-4,13), gS(0,7), gS(-27,5), gS(-26,-3),
           gS(1,1), gS(1,8), gS(21,3), gS(7,9), gS(23,8), gS(8,9), gS(21,2), gS(15,0),
           gS(-11,0), gS(17,6), gS(3,6), gS(23,13), gS(20,13), gS(14,4), gS(18,3), gS(-5,0),
           gS(0,-12), gS(5,0), gS(10,7), gS(17,7), gS(24,6), gS(2,6), gS(12,0), gS(3,-13),
           gS(-2,-16), gS(10,-7), gS(5,-4), gS(12,2), gS(8,4), gS(10,-5), gS(11,-8), gS(14,-11),
           gS(8,-30), gS(7,-32), gS(10,-15), gS(2,-2), gS(6,-3), gS(8,-15), gS(17,-27), gS(5,-42),
           gS(5,-37), gS(2,-15), gS(1,-2), gS(0,-6), gS(0,-2), gS(0,-7), gS(-1,-10), gS(3,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-65), gS(-35,-20), gS(-67,11), gS(-14,0), gS(0,1), gS(-56,13), gS(-28,-12), gS(-91,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-3), gS(50,18), gS(38,17), gS(52,-11), gS(-21,-3), gS(-7,-21),
           gS(-18,-18), gS(11,-2), gS(32,30), gS(35,28), gS(60,19), gS(45,24), gS(26,-9), gS(4,-17),
           gS(1,-11), gS(10,4), gS(28,26), gS(29,38), gS(22,32), gS(43,20), gS(20,4), gS(12,-8),
           gS(-4,-9), gS(10,8), gS(15,30), gS(21,33), gS(24,35), gS(20,32), gS(27,12), gS(7,0),
           gS(-20,-33), gS(0,-5), gS(5,10), gS(13,25), gS(17,23), gS(13,8), gS(7,0), gS(-5,-25),
           gS(-21,-16), gS(-28,0), gS(-7,-10), gS(6,7), gS(1,8), gS(-1,-9), gS(-16,-3), gS(-11,-3),
           gS(-40,-46), gS(-13,-27), gS(-11,-17), gS(-8,4), gS(0,2), gS(-8,-11), gS(-11,-20), gS(-40,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-3,3), gS(0,20), gS(0,35), gS(3,47), gS(9,55), gS(31,48), gS(8,51), gS(15,38),
           gS(-18,-4), gS(-56,36), gS(-17,26), gS(-41,72), gS(-43,108), gS(0,70), gS(-54,72), gS(-19,39),
           gS(-15,-17), gS(-8,-18), gS(-12,15), gS(-11,42), gS(1,60), gS(11,78), gS(29,45), gS(3,61),
           gS(-5,-33), gS(-3,0), gS(-7,2), gS(-15,45), gS(-14,65), gS(-7,71), gS(4,59), gS(9,30),
           gS(-1,-41), gS(3,-13), gS(-3,-9), gS(-10,28), gS(-6,27), gS(-6,24), gS(11,3), gS(3,8),
           gS(-1,-58), gS(9,-42), gS(3,-21), gS(2,-28), gS(2,-27), gS(0,-10), gS(16,-32), gS(5,-46),
           gS(-1,-66), gS(5,-61), gS(14,-76), gS(8,-46), gS(11,-49), gS(15,-88), gS(12,-83), gS(-10,-57),
           gS(0,-67), gS(3,-79), gS(5,-84), gS(8,-64), gS(9,-71), gS(4,-87), gS(8,-84), gS(-2,-75),
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
