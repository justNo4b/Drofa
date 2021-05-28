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
const int  KING_HIGH_DANGER  = gS(-16,-27);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-6,-16);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(0,10);   // слабый пешечный щит
const int  KING_SAFE         = gS(2,21);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-21,-104), gS(-9,-44), gS(-3,-17), gS(0,-1), gS(2,12), gS(4,22), gS(4,28),
           gS(2,31), gS(3,33), gS(6,32), gS(12,28), gS(28,18), gS(45,28), gS(76,-4),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-73), gS(1,-25), gS(5,0), gS(7,13), gS(10,21),
           gS(14,27), gS(18,25), gS(25,17), gS(34,3),
};

const int KING_MOBILITY[9] = {
           gS(10,-2), gS(14,-5), gS(8,2), gS(5,7), gS(-1,6),
           gS(-7,3), gS(-4,3), gS(-17,1), gS(-11,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-21,-102), gS(-13,-66), gS(-9,-33), gS(-7,-12), gS(-9,6), gS(-10,20), gS(-4,24),
           gS(0,27), gS(4,31), gS(9,36), gS(12,40), gS(17,42), gS(22,41), gS(37,28),
           gS(89,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-40,-104), gS(-19,-168), gS(-7,-132), gS(-4,-77), gS(-2,-44), gS(0,-21), gS(1,-3),
           gS(3,8), gS(5,17), gS(7,23), gS(9,30), gS(10,35), gS(11,39), gS(10,44),
           gS(8,50), gS(6,54), gS(5,57), gS(4,57), gS(4,57), gS(9,53), gS(12,48),
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
        [PAWN] = gS(75,128),
        [ROOK] = gS(420,748),
        [KNIGHT] = gS(281,412),
        [BISHOP] = gS(305,444),
        [QUEEN] = gS(948,1362),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(15,76);

const int PAWN_SUPPORTED = gS(8,2);

const int DOUBLED_PAWN_PENALTY = gS(-5,-27);

const int ISOLATED_PAWN_PENALTY = gS(-4,-8);

const int PAWN_BLOCKED = gS(2,26);

const int PASSER_BLOCKED = gS(2,46);

const int PAWN_DISTORTION = gS(-1,-1);

const int BISHOP_RAMMED_PENALTY = gS(-1,-9);

const int BISHOP_CENTER_CONTROL = gS(11,6);

const int MINOR_BEHIND_PAWN = gS(8,12);

const int MINOR_BEHIND_PASSER = gS(9,13);

const int KING_AHEAD_PASSER = gS(-15,9);

const int KING_EQUAL_PASSER = gS(6,7);

const int KING_BEHIND_PASSER = gS(6,-4);

const int KING_OPEN_FILE = gS(-48,-1);

const int KING_OWN_SEMI_FILE = gS(-29,18);

const int KING_ENEMY_SEMI_LINE = gS(-16,8);

const int KING_ATTACK_PAWN = gS(16,40);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(4,33), gS(-3,25), gS(-7,10),
           gS(-5,0), gS(-4,-4), gS(10,-9), gS(0,-19),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(49,45), gS(-15,17), gS(-4,-1),
           gS(-4,-13), gS(-3,-24), gS(3,-35), gS(13,-42),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {  gS(22,10), gS(22,11), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,3), gS(15,9), };


/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn 
 * and OUR pieces attacking enemy pawns - [PAWN] 
 */
const int HANGING_PIECE[5] = {
           gS(-3,11), gS(-26,-16), gS(-26,-50), gS(-31,-75), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-12,-28), gS(-16,-15), gS(-8,21),
           gS(16,55), gS(26,118), gS(76,132), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,17), gS(6,16), gS(0,3), gS(0,0),
           gS(-3,-1), gS(-14,1), gS(-8,17), gS(9,13),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-96), gS(-1,-47), gS(0,-33), gS(0,-17), gS(-1,-32), gS(-2,-27), gS(-3,-25), gS(-12,-88),
           gS(-13,-37), gS(0,32), gS(5,23), gS(0,5), gS(0,4), gS(3,22), gS(0,41), gS(-12,-36),
           gS(-2,-4), gS(27,34), gS(33,32), gS(12,20), gS(12,18), gS(35,31), gS(26,34), gS(-7,-10),
           gS(-9,-9), gS(22,16), gS(30,28), gS(0,33), gS(7,27), gS(32,24), gS(28,13), gS(-37,-12),
           gS(-22,-33), gS(22,0), gS(26,18), gS(-14,36), gS(1,31), gS(10,19), gS(24,1), gS(-53,-24),
           gS(-52,-15), gS(-9,1), gS(-7,13), gS(-27,30), gS(-15,26), gS(-16,17), gS(-9,2), gS(-50,-12),
           gS(0,-11), gS(-2,4), gS(-22,15), gS(-61,26), gS(-44,20), gS(-43,22), gS(-1,2), gS(-3,-19),
           gS(-28,-55), gS(0,-26), gS(-29,-4), gS(-33,-24), gS(-23,-37), gS(-55,-4), gS(-6,-25), gS(-17,-72),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,72), gS(48,96), gS(67,91), gS(74,78), gS(81,77), gS(84,81), gS(-7,101), gS(-22,83),
           gS(-3,47), gS(26,51), gS(40,43), gS(43,22), gS(54,22), gS(93,30), gS(60,38), gS(9,34),
           gS(-5,20), gS(4,23), gS(6,20), gS(20,5), gS(28,9), gS(29,13), gS(10,20), gS(-4,10),
           gS(-14,4), gS(-7,11), gS(-4,4), gS(4,3), gS(8,4), gS(4,5), gS(0,5), gS(-14,-4),
           gS(-19,-1), gS(-16,2), gS(-13,5), gS(-11,6), gS(-5,9), gS(-3,9), gS(0,-2), gS(-14,-11),
           gS(-17,1), gS(-12,4), gS(-17,15), gS(-9,16), gS(-14,21), gS(6,18), gS(6,0), gS(-15,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,27), gS(23,33), gS(0,42), gS(8,38), gS(12,37), gS(25,37), gS(41,31), gS(52,27),
           gS(-1,23), gS(-5,27), gS(16,23), gS(27,24), gS(24,25), gS(42,9), gS(8,22), gS(15,18),
           gS(-8,16), gS(32,2), gS(21,13), gS(43,2), gS(53,-3), gS(49,4), gS(70,-10), gS(14,8),
           gS(-7,8), gS(9,7), gS(17,9), gS(39,2), gS(31,2), gS(28,1), gS(28,-1), gS(12,2),
           gS(-20,0), gS(-11,11), gS(-13,12), gS(-3,5), gS(-4,5), gS(-8,8), gS(5,5), gS(-10,-3),
           gS(-24,-15), gS(-8,-3), gS(-15,-4), gS(-7,-9), gS(-6,-8), gS(-9,-6), gS(9,-7), gS(-15,-17),
           gS(-54,-9), gS(-17,-16), gS(-10,-11), gS(-4,-15), gS(-3,-16), gS(-2,-19), gS(-2,-21), gS(-53,-9),
           gS(-17,-11), gS(-10,-10), gS(-7,-7), gS(0,-15), gS(-1,-14), gS(-3,-4), gS(-1,-12), gS(-11,-20),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,5), gS(-43,8), gS(-78,17), gS(-80,23), gS(-68,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-48,1), gS(-12,4), gS(-7,8), gS(-14,16), gS(-5,14), gS(0,7), gS(-27,5), gS(-25,-4),
           gS(2,1), gS(2,7), gS(22,4), gS(8,9), gS(24,7), gS(9,9), gS(23,2), gS(14,-1),
           gS(-11,0), gS(18,6), gS(4,6), gS(22,13), gS(19,13), gS(15,4), gS(19,2), gS(-6,0),
           gS(0,-12), gS(6,-1), gS(11,7), gS(18,7), gS(25,6), gS(2,6), gS(13,0), gS(2,-13),
           gS(-1,-17), gS(11,-7), gS(6,-4), gS(13,2), gS(9,4), gS(11,-5), gS(12,-8), gS(14,-11),
           gS(8,-30), gS(8,-32), gS(11,-15), gS(3,-2), gS(8,-3), gS(9,-15), gS(17,-26), gS(5,-42),
           gS(5,-37), gS(2,-15), gS(1,-2), gS(1,-6), gS(0,-2), gS(0,-7), gS(-1,-10), gS(3,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-144,-67), gS(-35,-20), gS(-66,11), gS(-14,0), gS(0,1), gS(-54,13), gS(-28,-12), gS(-89,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-3), gS(49,17), gS(37,16), gS(52,-11), gS(-21,-3), gS(-7,-21),
           gS(-19,-18), gS(12,-2), gS(32,30), gS(34,28), gS(58,18), gS(45,23), gS(27,-10), gS(3,-17),
           gS(0,-11), gS(9,4), gS(28,26), gS(27,38), gS(19,32), gS(41,19), gS(18,3), gS(10,-9),
           gS(-4,-9), gS(10,8), gS(15,30), gS(21,32), gS(23,34), gS(20,31), gS(25,11), gS(6,0),
           gS(-19,-33), gS(1,-5), gS(5,10), gS(13,25), gS(17,23), gS(13,8), gS(7,0), gS(-5,-25),
           gS(-21,-16), gS(-27,0), gS(-7,-9), gS(6,7), gS(1,9), gS(0,-9), gS(-15,-3), gS(-10,-4),
           gS(-40,-46), gS(-13,-27), gS(-11,-17), gS(-8,4), gS(0,2), gS(-7,-11), gS(-11,-20), gS(-40,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-4,3), gS(0,19), gS(0,34), gS(3,45), gS(9,54), gS(31,48), gS(8,49), gS(14,37),
           gS(-18,-2), gS(-57,38), gS(-18,27), gS(-42,72), gS(-46,109), gS(0,70), gS(-55,72), gS(-18,39),
           gS(-15,-17), gS(-8,-17), gS(-12,15), gS(-12,42), gS(0,60), gS(9,78), gS(28,46), gS(0,60),
           gS(-5,-32), gS(-3,0), gS(-8,3), gS(-16,47), gS(-15,65), gS(-9,72), gS(2,59), gS(6,30),
           gS(0,-41), gS(3,-13), gS(-3,-9), gS(-10,28), gS(-6,27), gS(-8,24), gS(11,3), gS(2,8),
           gS(0,-58), gS(10,-42), gS(4,-21), gS(3,-29), gS(3,-27), gS(1,-10), gS(16,-32), gS(5,-47),
           gS(0,-66), gS(6,-61), gS(15,-76), gS(9,-47), gS(12,-50), gS(17,-89), gS(14,-83), gS(-10,-57),
           gS(1,-68), gS(5,-80), gS(7,-86), gS(10,-65), gS(11,-72), gS(6,-87), gS(9,-84), gS(-1,-75),
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
