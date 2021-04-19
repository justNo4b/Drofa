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
const int  KING_HIGH_DANGER  = gS(-15,-94);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-4,-30);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(-9,90);   // слабый пешечный щит
const int  KING_SAFE         = gS(7,19);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-11,-103), gS(-1,-20), gS(1,22), gS(4,46), gS(5,69), gS(5,89), gS(4,103),
           gS(0,112), gS(0,117), gS(0,121), gS(13,103), gS(33,91), gS(55,62), gS(84,22),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(0,-72), gS(6,1), gS(8,46), gS(9,72), gS(11,90),
           gS(12,107), gS(15,114), gS(19,112), gS(35,85),
};

const int  KING_MOBILITY[9] = {
           gS(12,-3), gS(16,-2), gS(12,4), gS(6,12), gS(-3,11),
           gS(-7,6), gS(-16,17), gS(-25,1), gS(1,-38),
};

const int  ROOK_MOBILITY[15] = {
           gS(-9,-97), gS(-5,-35), gS(-2,16), gS(-1,49), gS(-3,80), gS(-5,104), gS(-1,111),
           gS(2,118), gS(4,132), gS(7,143), gS(7,150), gS(15,149), gS(20,144), gS(44,116),
           gS(91,94),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-31,-103), gS(-5,-166), gS(8,-132), gS(12,-77), gS(14,-41), gS(15,-13), gS(16,10),
           gS(18,27), gS(20,41), gS(21,50), gS(22,59), gS(24,66), gS(24,71), gS(24,78),
           gS(22,87), gS(20,92), gS(19,95), gS(19,94), gS(20,93), gS(24,85), gS(26,76),
           gS(21,63), gS(15,51), gS(24,35), gS(13,27), gS(20,24), gS(26,29), gS(24,26),
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
        [PAWN] = gS(47,234),
        [ROOK] = gS(453,967),
        [KNIGHT] = gS(291,579),
        [BISHOP] = gS(311,624),
        [QUEEN] = gS(1124,1571),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(6,117);

const int  PAWN_SUPPORTED = gS(4,20);

const int  DOUBLED_PAWN_PENALTY = gS(0,-59);

const int  ISOLATED_PAWN_PENALTY = gS(6,-39);

const int  PAWN_BLOCKED = gS(-4,58);

const int  PASSER_BLOCKED = gS(-19,108);

const int  PAWN_DISTORTION = gS(-3,4);

const int  BISHOP_RAMMED_PENALTY = gS(-2,-13);

const int  BISHOP_CENTER_CONTROL = gS(4,34);

const int  MINOR_BEHIND_PAWN = gS(4,37);

const int  MINOR_BEHIND_PASSER = gS(23,0);

const int  KING_AHEAD_PASSER = gS(-28,76);

const int  KING_EQUAL_PASSER = gS(-31,47);

const int  KING_BEHIND_PASSER = gS(10,8);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(-34,83), gS(-36,91), gS(-15,44),
           gS(2,9), gS(-3,1), gS(5,-13), gS(-9,-34),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(81,113), gS(27,-44), gS(17,-54),
           gS(2,-59), gS(-5,-62), gS(-17,-47), gS(12,-79),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(8,56), gS(17,42),};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(-1,32), gS(14,25), };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-28,-21), gS(-22,-75), gS(-31,-89), gS(-21,-11),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-10,-102), gS(-16,-81), gS(-20,-3),
           gS(-8,81), gS(-11,224), gS(71,273), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(-17,98), gS(-21,76), gS(-11,23), gS(5,-2),
           gS(4,-10), gS(-20,1), gS(-24,49), gS(0,54),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-94), gS(-3,-44), gS(-2,-29), gS(-1,-13), gS(-3,-29), gS(-4,-24), gS(-5,-22), gS(-12,-86),
           gS(-13,-38), gS(2,50), gS(7,38), gS(1,13), gS(1,12), gS(5,36), gS(2,63), gS(-12,-39),
           gS(-2,0), gS(33,75), gS(40,76), gS(17,48), gS(17,46), gS(43,82), gS(34,85), gS(-7,-6),
           gS(-10,-10), gS(25,41), gS(36,76), gS(5,80), gS(14,76), gS(41,80), gS(34,50), gS(-39,-13),
           gS(-26,-55), gS(21,-5), gS(29,44), gS(-14,76), gS(3,73), gS(12,50), gS(24,5), gS(-60,-50),
           gS(-58,-45), gS(-11,-26), gS(-9,5), gS(-35,39), gS(-22,32), gS(-22,16), gS(-4,-21), gS(-50,-50),
           gS(1,-33), gS(4,-20), gS(-24,-6), gS(-70,4), gS(-49,-2), gS(-50,7), gS(6,-19), gS(0,-43),
           gS(-42,-88), gS(-9,-62), gS(-43,-39), gS(-36,-81), gS(-24,-94), gS(-48,-58), gS(-14,-75), gS(-21,-140),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(59,150), gS(45,138), gS(66,127), gS(72,111), gS(82,103), gS(86,102), gS(-7,120), gS(-18,134),
           gS(-18,127), gS(19,98), gS(36,82), gS(41,42), gS(58,48), gS(98,82), gS(66,79), gS(3,100),
           gS(-10,70), gS(9,48), gS(9,43), gS(25,19), gS(29,36), gS(28,52), gS(12,54), gS(-8,55),
           gS(-17,39), gS(-2,32), gS(-2,23), gS(4,23), gS(8,32), gS(3,38), gS(4,30), gS(-17,24),
           gS(-22,29), gS(-9,12), gS(-13,29), gS(-11,31), gS(-6,43), gS(-7,51), gS(4,15), gS(-17,15),
           gS(-22,43), gS(-8,27), gS(-20,54), gS(-12,56), gS(-17,68), gS(-4,85), gS(4,37), gS(-22,17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(34,83), gS(30,88), gS(6,93), gS(15,83), gS(19,81), gS(35,82), gS(51,78), gS(62,77),
           gS(-15,80), gS(-19,91), gS(6,85), gS(23,87), gS(22,83), gS(44,60), gS(9,77), gS(13,70),
           gS(-15,56), gS(24,45), gS(12,61), gS(37,43), gS(51,34), gS(46,50), gS(72,29), gS(11,47),
           gS(-11,33), gS(5,36), gS(12,44), gS(35,34), gS(28,31), gS(23,32), gS(26,28), gS(8,29),
           gS(-17,-1), gS(-11,26), gS(-14,26), gS(-2,16), gS(-3,15), gS(-10,22), gS(4,20), gS(-12,2),
           gS(-19,-28), gS(-5,-5), gS(-10,-12), gS(0,-20), gS(0,-16), gS(-7,-10), gS(9,-5), gS(-14,-23),
           gS(-48,-34), gS(-11,-30), gS(-3,-25), gS(5,-31), gS(5,-29), gS(0,-27), gS(0,-27), gS(-55,-24),
           gS(-15,-14), gS(-7,-12), gS(-3,-9), gS(2,-17), gS(0,-12), gS(-4,5), gS(0,-11), gS(-10,-26),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-54,11), gS(-43,20), gS(-75,38), gS(-78,52), gS(-65,46), gS(-92,29), gS(-15,7), gS(-50,3),
           gS(-46,-1), gS(-9,6), gS(-7,22), gS(-12,44), gS(-3,40), gS(2,26), gS(-25,10), gS(-18,-3),
           gS(8,10), gS(3,23), gS(24,14), gS(6,31), gS(25,36), gS(10,29), gS(28,25), gS(20,10),
           gS(-7,9), gS(18,32), gS(4,27), gS(17,59), gS(15,54), gS(14,30), gS(18,31), gS(-4,16),
           gS(2,-7), gS(7,15), gS(10,33), gS(15,41), gS(21,43), gS(5,21), gS(13,20), gS(4,-7),
           gS(1,-11), gS(12,2), gS(9,0), gS(13,21), gS(8,23), gS(13,0), gS(13,-1), gS(14,1),
           gS(9,-32), gS(11,-38), gS(12,-10), gS(3,14), gS(7,15), gS(9,-9), gS(17,-24), gS(7,-46),
           gS(8,-45), gS(4,-15), gS(1,14), gS(3,0), gS(3,2), gS(0,3), gS(0,-12), gS(5,-46),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-146,-67), gS(-36,-15), gS(-64,21), gS(-13,7), gS(1,9), gS(-52,22), gS(-29,-9), gS(-89,-70),
           gS(-9,-32), gS(-10,-6), gS(33,2), gS(55,43), gS(43,40), gS(56,-4), gS(-19,-1), gS(-6,-23),
           gS(-19,-21), gS(11,2), gS(30,59), gS(28,66), gS(60,54), gS(48,59), gS(30,2), gS(7,-13),
           gS(2,-11), gS(9,15), gS(23,59), gS(20,87), gS(10,78), gS(33,62), gS(14,32), gS(8,3),
           gS(-1,-12), gS(10,19), gS(12,54), gS(15,71), gS(16,79), gS(16,67), gS(24,33), gS(8,9),
           gS(-15,-48), gS(2,-11), gS(5,10), gS(11,43), gS(13,44), gS(10,19), gS(6,4), gS(-3,-32),
           gS(-19,-22), gS(-26,-4), gS(-4,-22), gS(6,16), gS(0,19), gS(0,-12), gS(-15,-2), gS(-9,-3),
           gS(-38,-54), gS(-9,-36), gS(-8,-26), gS(-6,6), gS(1,6), gS(-6,-15), gS(-8,-26), gS(-38,-51),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-1,11), gS(5,30), gS(6,46), gS(14,62), gS(20,72), gS(38,60), gS(13,58), gS(20,44),
           gS(-9,0), gS(-46,41), gS(-12,38), gS(-34,87), gS(-40,132), gS(7,85), gS(-52,77), gS(-11,44),
           gS(-9,-18), gS(-4,-18), gS(-6,19), gS(-8,53), gS(7,72), gS(17,92), gS(37,57), gS(2,70),
           gS(0,-36), gS(2,3), gS(-6,6), gS(-12,61), gS(-10,85), gS(-6,87), gS(5,70), gS(8,41),
           gS(6,-42), gS(8,-11), gS(1,-5), gS(-5,40), gS(-1,43), gS(-7,40), gS(14,13), gS(2,15),
           gS(4,-57), gS(15,-37), gS(8,-13), gS(7,-17), gS(7,-15), gS(3,4), gS(18,-23), gS(7,-43),
           gS(3,-64), gS(11,-58), gS(19,-68), gS(13,-37), gS(17,-36), gS(19,-84), gS(17,-77), gS(-7,-55),
           gS(8,-67), gS(11,-79), gS(13,-86), gS(16,-61), gS(16,-68), gS(12,-88), gS(14,-82), gS(4,-73),
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
