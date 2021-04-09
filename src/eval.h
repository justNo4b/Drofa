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
const int  KING_HIGH_DANGER  = gS(-13,-27);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-6,-17);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(1,8);   // слабый пешечный щит
const int  KING_SAFE         = gS(4,26);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-16,-106), gS(-4,-47), gS(1,-20), gS(3,-3), gS(6,10), gS(7,21), gS(7,28),
           gS(5,30), gS(6,33), gS(10,31), gS(15,27), gS(32,17), gS(48,31), gS(80,-3),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-9,-76), gS(0,-30), gS(4,-5), gS(6,9), gS(9,17),
           gS(12,24), gS(17,23), gS(24,16), gS(34,1),
};

const int  KING_MOBILITY[9] = {
           gS(13,1), gS(14,1), gS(7,7), gS(2,10), gS(-4,8),
           gS(-8,4), gS(-5,2), gS(-14,-3), gS(-8,-16),
};

const int  ROOK_MOBILITY[15] = {
           gS(-18,-98), gS(-12,-66), gS(-8,-33), gS(-5,-13), gS(-6,5), gS(-8,19), gS(-2,21),
           gS(1,25), gS(6,29), gS(11,34), gS(12,38), gS(17,41), gS(20,41), gS(35,27),
           gS(87,0),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-37,-95), gS(-18,-152), gS(-5,-130), gS(-2,-79), gS(0,-46), gS(0,-23), gS(2,-5),
           gS(4,7), gS(7,16), gS(9,23), gS(10,30), gS(11,35), gS(11,39), gS(10,45),
           gS(8,51), gS(5,55), gS(4,57), gS(3,57), gS(3,57), gS(8,53), gS(11,49),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
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
        [PAWN] = gS(77,113),
        [ROOK] = gS(420,729),
        [KNIGHT] = gS(288,404),
        [BISHOP] = gS(304,435),
        [QUEEN] = gS(925,1350),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(15,76);

const int  PAWN_SUPPORTED = gS(8,6);

const int  DOUBLED_PAWN_PENALTY = gS(-13,-25);

const int  ISOLATED_PAWN_PENALTY = gS(-8,-12);

const int  PAWN_BLOCKED = gS(2,25);

const int  PASSER_BLOCKED = gS(2,44);

const int  BISHOP_RAMMED_PENALTY = gS(-1,-9);

const int  BISHOP_CENTER_CONTROL = gS(10,5);

const int  KING_AHEAD_PASSER = gS(-6,7);

const int  KING_EQUAL_PASSER = gS(4,1);

const int  KING_BEHIND_PASSER = gS(3,-5);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(7,27), gS(-4,24), gS(-9,9),
           gS(-6,-2), gS(-6,-5), gS(8,-10), gS(-2,-19),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(41,80), gS(-19,14), gS(-5,-2),
           gS(-3,-15), gS(-1,-26), gS(6,-38), gS(15,-45),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(24,7), gS(26,10) };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,7), gS(16,10) };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-26,-18), gS(-24,-52), gS(-31,-71), gS(-17,-11),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-14,-26), gS(-18,-13), gS(-8,21),
           gS(16,53), gS(28,114), gS(74,114), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(14,15), gS(8,16), gS(0,4), gS(-2,1),
           gS(-4,0), gS(-18,2), gS(-7,17), gS(13,12),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-88), gS(-6,-45), gS(-5,-32), gS(-4,-18), gS(-6,-31), gS(-7,-27), gS(-8,-25), gS(-12,-81),
           gS(-13,-37), gS(0,33), gS(5,22), gS(0,5), gS(0,4), gS(3,22), gS(0,43), gS(-12,-36),
           gS(-3,-4), gS(25,37), gS(29,32), gS(10,19), gS(10,16), gS(30,33), gS(25,39), gS(-7,-9),
           gS(-9,-10), gS(20,20), gS(27,27), gS(0,29), gS(7,25), gS(31,24), gS(29,19), gS(-34,-11),
           gS(-21,-34), gS(22,3), gS(26,17), gS(-15,32), gS(3,28), gS(13,19), gS(25,5), gS(-51,-22),
           gS(-50,-15), gS(-8,3), gS(-6,12), gS(-26,27), gS(-14,23), gS(-14,17), gS(-6,3), gS(-52,-12),
           gS(0,-11), gS(-3,7), gS(-24,17), gS(-63,25), gS(-44,20), gS(-45,23), gS(-2,5), gS(-4,-20),
           gS(-28,-56), gS(3,-27), gS(-29,-5), gS(-37,-25), gS(-27,-35), gS(-55,-6), gS(-5,-25), gS(-16,-73),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(65,80), gS(49,91), gS(69,90), gS(73,75), gS(81,75), gS(82,79), gS(-8,97), gS(-19,91),
           gS(-3,48), gS(25,43), gS(39,37), gS(40,19), gS(51,19), gS(94,25), gS(59,30), gS(9,35),
           gS(-5,23), gS(1,21), gS(5,19), gS(20,4), gS(28,8), gS(30,12), gS(9,18), gS(-3,13),
           gS(-14,9), gS(-8,12), gS(-1,5), gS(5,3), gS(10,4), gS(8,7), gS(1,5), gS(-13,0),
           gS(-17,3), gS(-14,3), gS(-10,8), gS(-7,9), gS(0,12), gS(1,11), gS(4,-2), gS(-12,-5),
           gS(-15,7), gS(-11,7), gS(-14,19), gS(-7,21), gS(-11,26), gS(10,21), gS(9,2), gS(-15,-9),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(28,26), gS(22,31), gS(0,40), gS(7,35), gS(10,35), gS(23,34), gS(38,30), gS(49,26),
           gS(-3,27), gS(-8,32), gS(14,28), gS(24,29), gS(20,30), gS(39,12), gS(5,24), gS(13,20),
           gS(-9,19), gS(32,4), gS(20,16), gS(42,5), gS(52,0), gS(45,7), gS(69,-9), gS(11,11),
           gS(-6,10), gS(10,9), gS(18,11), gS(40,4), gS(33,3), gS(26,3), gS(28,0), gS(10,4),
           gS(-18,0), gS(-10,11), gS(-12,12), gS(-2,5), gS(-3,4), gS(-10,8), gS(4,4), gS(-13,-2),
           gS(-22,-16), gS(-7,-4), gS(-14,-5), gS(-5,-11), gS(-4,-12), gS(-11,-8), gS(7,-9), gS(-16,-18),
           gS(-51,-11), gS(-15,-18), gS(-8,-14), gS(-1,-19), gS(0,-20), gS(-3,-24), gS(-2,-24), gS(-55,-11),
           gS(-14,-14), gS(-7,-13), gS(-4,-10), gS(2,-18), gS(1,-18), gS(-2,-9), gS(0,-17), gS(-11,-23),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-45,9), gS(-40,8), gS(-72,13), gS(-76,19), gS(-63,15), gS(-88,11), gS(-14,0), gS(-44,3),
           gS(-48,0), gS(-10,2), gS(-7,7), gS(-17,15), gS(-10,13), gS(0,7), gS(-25,6), gS(-22,-4),
           gS(3,-1), gS(4,5), gS(24,3), gS(7,7), gS(24,7), gS(12,8), gS(25,0), gS(16,-5),
           gS(-11,-3), gS(19,1), gS(3,3), gS(21,12), gS(18,11), gS(14,0), gS(19,-2), gS(-7,-2),
           gS(0,-14), gS(6,-4), gS(10,5), gS(17,5), gS(25,4), gS(3,3), gS(14,-3), gS(2,-16),
           gS(1,-15), gS(11,-6), gS(9,-2), gS(17,0), gS(12,2), gS(13,-3), gS(12,-7), gS(15,-8),
           gS(11,-25), gS(12,-26), gS(13,-14), gS(6,-3), gS(11,-1), gS(11,-13), gS(21,-17), gS(8,-33),
           gS(10,-31), gS(3,-12), gS(3,-5), gS(2,-9), gS(2,-6), gS(4,-2), gS(0,-8), gS(8,-29),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-139,-72), gS(-33,-21), gS(-59,9), gS(-14,-1), gS(-1,0), gS(-47,11), gS(-26,-14), gS(-81,-68),
           gS(-12,-25), gS(-13,-6), gS(30,-5), gS(47,16), gS(35,15), gS(53,-13), gS(-22,-4), gS(-8,-22),
           gS(-20,-19), gS(10,-5), gS(30,28), gS(30,29), gS(57,18), gS(45,22), gS(28,-13), gS(3,-16),
           gS(-2,-11), gS(6,4), gS(24,26), gS(23,37), gS(12,34), gS(36,21), gS(14,7), gS(6,-8),
           gS(-6,-10), gS(8,7), gS(12,28), gS(18,30), gS(21,33), gS(17,31), gS(23,11), gS(5,-1),
           gS(-19,-34), gS(0,-7), gS(5,8), gS(14,22), gS(18,20), gS(12,5), gS(5,-1), gS(-7,-25),
           gS(-21,-16), gS(-25,0), gS(-6,-12), gS(7,3), gS(2,8), gS(0,-10), gS(-13,0), gS(-9,-2),
           gS(-38,-44), gS(-11,-28), gS(-10,-19), gS(-8,0), gS(0,0), gS(-6,-8), gS(-9,-19), gS(-38,-42),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-5,3), gS(0,19), gS(0,32), gS(5,41), gS(11,50), gS(32,47), gS(10,46), gS(15,33),
           gS(-17,-1), gS(-57,40), gS(-17,29), gS(-40,71), gS(-50,110), gS(-1,69), gS(-55,67), gS(-18,39),
           gS(-15,-15), gS(-7,-15), gS(-11,16), gS(-13,43), gS(0,60), gS(7,77), gS(24,48), gS(-7,61),
           gS(-4,-31), gS(-2,1), gS(-10,6), gS(-18,49), gS(-17,66), gS(-14,73), gS(-1,59), gS(1,32),
           gS(0,-41), gS(3,-13), gS(-3,-8), gS(-10,28), gS(-6,27), gS(-12,26), gS(9,3), gS(-1,7),
           gS(0,-58), gS(11,-42), gS(5,-22), gS(4,-31), gS(4,-28), gS(0,-13), gS(15,-33), gS(4,-46),
           gS(1,-66), gS(8,-63), gS(17,-79), gS(10,-49), gS(15,-53), gS(18,-92), gS(15,-84), gS(-10,-55),
           gS(3,-70), gS(7,-82), gS(9,-88), gS(13,-69), gS(13,-73), gS(7,-89), gS(10,-82), gS(-1,-72),
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
