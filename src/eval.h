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
const int  KING_HIGH_DANGER  = gS(-14,-27);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-7,-16);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(0,8);   // слабый пешечный щит
const int  KING_SAFE         = gS(4,24);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-17,-105), gS(-5,-47), gS(0,-19), gS(2,-3), gS(5,10), gS(6,21), gS(6,27),
           gS(5,30), gS(6,32), gS(9,30), gS(14,26), gS(30,17), gS(46,29), gS(78,-5),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-7,-76), gS(1,-30), gS(5,-5), gS(8,9), gS(10,18),
           gS(13,24), gS(18,23), gS(25,15), gS(34,0),
};

const int  KING_MOBILITY[9] = {
           gS(11,-1), gS(14,-2), gS(8,5), gS(3,10), gS(-3,9),
           gS(-8,5), gS(-5,3), gS(-14,-3), gS(-9,-16),
};

const int  ROOK_MOBILITY[15] = {
           gS(-19,-101), gS(-11,-66), gS(-7,-34), gS(-5,-13), gS(-6,5), gS(-8,19), gS(-2,22),
           gS(1,25), gS(5,30), gS(10,35), gS(11,39), gS(17,41), gS(20,41), gS(35,27),
           gS(87,0),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-38,-101), gS(-17,-162), gS(-5,-132), gS(-2,-78), gS(0,-45), gS(1,-23), gS(2,-5),
           gS(4,7), gS(6,17), gS(8,23), gS(9,30), gS(10,35), gS(10,39), gS(9,45),
           gS(8,51), gS(5,55), gS(4,57), gS(3,57), gS(3,57), gS(8,53), gS(11,48),
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
        [PAWN] = gS(79,120),
        [ROOK] = gS(425,730),
        [KNIGHT] = gS(287,404),
        [BISHOP] = gS(306,433),
        [QUEEN] = gS(936,1350),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(16,75);

const int  PAWN_SUPPORTED = gS(8,5);

const int  DOUBLED_PAWN_PENALTY = gS(-9,-29);

const int  ISOLATED_PAWN_PENALTY = gS(-3,-6);

const int  PAWN_BLOCKED = gS(2,26);

const int  PASSER_BLOCKED = gS(3,46);

const int  PAWN_DISTORTION = gS(-1,-1);

const int  BISHOP_RAMMED_PENALTY = gS(-1,-9);

const int  BISHOP_CENTER_CONTROL = gS(10,7);

const int  MINOR_BEHIND_PAWN = gS(6,21);

const int  MINOR_BEHIND_PASSER = gS(8,13);

const int  KING_AHEAD_PASSER = gS(-10,8);

const int  KING_EQUAL_PASSER = gS(6,1);

const int  KING_BEHIND_PASSER = gS(4,-5);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(7,28), gS(-3,24), gS(-8,10),
           gS(-5,-1), gS(-5,-4), gS(10,-9), gS(-1,-18),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(42,80), gS(-18,13), gS(-5,-3),
           gS(-4,-16), gS(-2,-27), gS(5,-39), gS(13,-46),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(24,7), gS(25,9) };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,7), gS(17,9) };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-27,-18), gS(-25,-52), gS(-32,-72), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-14,-27), gS(-17,-13), gS(-8,21),
           gS(15,54), gS(28,114), gS(81,122), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(12,13), gS(8,15), gS(1,4), gS(0,1),
           gS(-3,0), gS(-16,3), gS(-7,17), gS(11,11),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-93), gS(-4,-47), gS(-3,-33), gS(-2,-18), gS(-4,-32), gS(-5,-27), gS(-6,-25), gS(-12,-85),
           gS(-13,-37), gS(0,33), gS(5,22), gS(0,5), gS(0,4), gS(3,22), gS(0,43), gS(-12,-36),
           gS(-3,-4), gS(27,36), gS(32,31), gS(12,19), gS(12,16), gS(34,33), gS(27,39), gS(-7,-9),
           gS(-9,-10), gS(22,19), gS(29,27), gS(0,29), gS(8,26), gS(33,24), gS(30,19), gS(-36,-10),
           gS(-22,-33), gS(22,3), gS(26,17), gS(-15,32), gS(3,28), gS(13,20), gS(25,5), gS(-53,-21),
           gS(-52,-14), gS(-9,3), gS(-7,12), gS(-27,27), gS(-15,23), gS(-15,17), gS(-9,4), gS(-53,-11),
           gS(-1,-11), gS(-3,7), gS(-24,17), gS(-63,25), gS(-44,20), gS(-45,23), gS(-1,5), gS(-4,-20),
           gS(-27,-55), gS(3,-27), gS(-30,-5), gS(-38,-25), gS(-29,-35), gS(-56,-5), gS(-5,-26), gS(-16,-72),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(63,72), gS(51,96), gS(70,92), gS(76,78), gS(82,77), gS(84,80), gS(-8,100), gS(-22,83),
           gS(-2,46), gS(30,51), gS(42,43), gS(45,25), gS(55,25), gS(98,30), gS(65,36), gS(10,32),
           gS(-5,19), gS(6,25), gS(7,22), gS(22,7), gS(30,11), gS(32,14), gS(13,21), gS(-3,8),
           gS(-15,4), gS(-6,13), gS(-4,5), gS(4,4), gS(9,4), gS(5,6), gS(2,6), gS(-15,-4),
           gS(-20,-2), gS(-15,3), gS(-13,6), gS(-12,7), gS(-4,10), gS(-2,9), gS(2,-3), gS(-15,-11),
           gS(-18,1), gS(-12,5), gS(-17,16), gS(-10,16), gS(-14,22), gS(6,17), gS(7,0), gS(-18,-16),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(29,26), gS(22,32), gS(0,40), gS(7,35), gS(10,35), gS(23,34), gS(39,29), gS(50,26),
           gS(-3,27), gS(-8,32), gS(14,28), gS(24,29), gS(21,30), gS(39,12), gS(5,24), gS(13,20),
           gS(-8,19), gS(32,4), gS(20,16), gS(42,5), gS(52,0), gS(46,7), gS(69,-9), gS(11,11),
           gS(-6,10), gS(10,9), gS(19,11), gS(40,4), gS(33,3), gS(27,3), gS(29,0), gS(10,4),
           gS(-18,0), gS(-10,11), gS(-12,12), gS(-1,4), gS(-2,3), gS(-10,8), gS(4,4), gS(-13,-2),
           gS(-22,-16), gS(-7,-4), gS(-14,-6), gS(-5,-12), gS(-4,-12), gS(-11,-9), gS(7,-9), gS(-16,-19),
           gS(-52,-11), gS(-15,-18), gS(-8,-14), gS(-1,-19), gS(0,-20), gS(-2,-23), gS(-2,-24), gS(-55,-11),
           gS(-14,-14), gS(-7,-13), gS(-4,-10), gS(2,-18), gS(1,-17), gS(-1,-9), gS(0,-16), gS(-10,-23),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-50,6), gS(-43,8), gS(-75,16), gS(-79,22), gS(-66,18), gS(-91,15), gS(-15,0), gS(-48,0),
           gS(-48,1), gS(-10,4), gS(-7,8), gS(-15,17), gS(-7,16), gS(0,8), gS(-26,6), gS(-21,-4),
           gS(4,0), gS(5,7), gS(25,3), gS(8,9), gS(25,8), gS(11,9), gS(26,1), gS(17,-3),
           gS(-9,0), gS(20,4), gS(5,5), gS(23,13), gS(20,13), gS(16,2), gS(21,0), gS(-7,0),
           gS(0,-12), gS(7,-2), gS(12,7), gS(19,6), gS(26,5), gS(4,4), gS(15,-1), gS(4,-15),
           gS(0,-17), gS(11,-7), gS(8,-4), gS(15,0), gS(10,2), gS(13,-6), gS(13,-9), gS(15,-11),
           gS(8,-28), gS(9,-32), gS(11,-15), gS(4,-3), gS(9,-4), gS(10,-16), gS(18,-27), gS(4,-39),
           gS(6,-36), gS(1,-14), gS(1,-4), gS(1,-6), gS(1,-3), gS(0,-8), gS(-1,-10), gS(3,-36),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-142,-70), gS(-35,-20), gS(-63,11), gS(-14,0), gS(0,1), gS(-51,13), gS(-28,-12), gS(-86,-70),
           gS(-11,-24), gS(-12,-5), gS(32,-4), gS(49,17), gS(37,16), gS(54,-12), gS(-21,-3), gS(-7,-21),
           gS(-19,-18), gS(12,-4), gS(32,29), gS(32,30), gS(59,19), gS(46,24), gS(29,-12), gS(5,-16),
           gS(0,-10), gS(8,5), gS(26,27), gS(25,38), gS(15,35), gS(39,22), gS(16,8), gS(8,-7),
           gS(-5,-9), gS(10,8), gS(14,29), gS(21,31), gS(24,33), gS(20,32), gS(25,11), gS(7,-1),
           gS(-19,-34), gS(0,-7), gS(5,7), gS(13,22), gS(17,20), gS(13,5), gS(7,-2), gS(-5,-27),
           gS(-21,-17), gS(-28,-2), gS(-7,-13), gS(6,3), gS(1,5), gS(0,-12), gS(-17,-3), gS(-11,-4),
           gS(-40,-46), gS(-14,-29), gS(-12,-19), gS(-8,1), gS(0,0), gS(-8,-12), gS(-11,-21), gS(-40,-45),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-5,3), gS(0,19), gS(0,33), gS(3,43), gS(9,52), gS(31,48), gS(8,48), gS(14,34),
           gS(-18,0), gS(-57,41), gS(-17,30), gS(-41,73), gS(-50,111), gS(-1,71), gS(-57,71), gS(-19,39),
           gS(-15,-15), gS(-7,-15), gS(-11,16), gS(-13,43), gS(0,60), gS(7,77), gS(24,47), gS(-7,60),
           gS(-4,-31), gS(-2,1), gS(-10,6), gS(-18,50), gS(-16,66), gS(-13,73), gS(-1,58), gS(1,31),
           gS(1,-41), gS(4,-13), gS(-3,-8), gS(-9,28), gS(-5,27), gS(-12,26), gS(9,3), gS(-1,7),
           gS(0,-58), gS(12,-42), gS(5,-22), gS(5,-31), gS(4,-28), gS(1,-13), gS(16,-33), gS(4,-47),
           gS(1,-66), gS(9,-63), gS(18,-80), gS(11,-49), gS(16,-53), gS(18,-92), gS(15,-85), gS(-10,-57),
           gS(3,-70), gS(7,-82), gS(9,-88), gS(13,-69), gS(14,-74), gS(8,-89), gS(10,-84), gS(0,-74),
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
