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
const int  KING_HIGH_DANGER  = gS(-14,-52);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-7,-21);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(-4,43);   // слабый пешечный щит
const int  KING_SAFE         = gS(5,20);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-17,-106), gS(-6,-41), gS(-1,-9), gS(1,8), gS(3,26), gS(4,39), gS(4,47),
           gS(2,51), gS(2,55), gS(4,56), gS(12,47), gS(29,35), gS(47,34), gS(78,0),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-5,-75), gS(3,-23), gS(5,8), gS(8,26), gS(10,39),
           gS(12,48), gS(16,50), gS(21,46), gS(33,29),
};

const int  KING_MOBILITY[9] = {
           gS(12,-2), gS(16,-5), gS(11,0), gS(7,5), gS(-1,7),
           gS(-7,6), gS(-11,12), gS(-26,9), gS(-7,-23),
};

const int  ROOK_MOBILITY[15] = {
           gS(-17,-101), gS(-11,-59), gS(-7,-21), gS(-6,3), gS(-7,25), gS(-9,43), gS(-4,47),
           gS(0,51), gS(3,60), gS(7,67), gS(8,71), gS(15,70), gS(20,67), gS(37,50),
           gS(84,27),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-36,-103), gS(-14,-165), gS(0,-132), gS(2,-78), gS(4,-44), gS(5,-20), gS(6,0),
           gS(8,13), gS(10,25), gS(12,33), gS(13,40), gS(14,46), gS(15,50), gS(14,56),
           gS(12,63), gS(10,68), gS(9,70), gS(9,70), gS(9,70), gS(14,64), gS(16,58),
           gS(14,50), gS(12,44), gS(22,31), gS(12,25), gS(20,23), gS(26,29), gS(24,26),
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
        [PAWN] = gS(61,181),
        [ROOK] = gS(412,869),
        [KNIGHT] = gS(282,486),
        [BISHOP] = gS(305,518),
        [QUEEN] = gS(1013,1461),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(9,96);

const int  PAWN_SUPPORTED = gS(5,13);

const int  DOUBLED_PAWN_PENALTY = gS(-3,-44);

const int  ISOLATED_PAWN_PENALTY = gS(-1,-13);

const int  PAWN_BLOCKED = gS(-1,40);

const int  PASSER_BLOCKED = gS(-5,68);

const int  PAWN_DISTORTION = gS(-2,1);

const int  BISHOP_RAMMED_PENALTY = gS(-2,-10);

const int  BISHOP_CENTER_CONTROL = gS(8,17);

const int  MINOR_BEHIND_PAWN = gS(5,27);

const int  MINOR_BEHIND_PASSER = gS(13,7);

const int  KING_AHEAD_PASSER = gS(-19,47);

const int  KING_EQUAL_PASSER = gS(-12,18);

const int  KING_BEHIND_PASSER = gS(6,-11);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(-17,69), gS(-21,55), gS(-10,19),
           gS(1,-5), gS(0,-12), gS(10,-19), gS(-4,-27),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(66,77), gS(9,-20), gS(4,-23),
           gS(-3,-27), gS(-7,-30), gS(-7,-27), gS(9,-41),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(17,26), gS(21,21) };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(3,15), gS(16,14), };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-28,-19), gS(-24,-59), gS(-32,-77), gS(-19,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-7,-60), gS(-11,-45), gS(-10,9),
           gS(4,66), gS(6,164), gS(70,186), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(-2,53), gS(-4,38), gS(-3,7), gS(4,-11),
           gS(2,-13), gS(-18,-1), gS(-14,24), gS(7,26),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-93), gS(-3,-46), gS(-2,-32), gS(-1,-16), gS(-3,-31), gS(-4,-26), gS(-5,-24), gS(-12,-85),
           gS(-13,-36), gS(0,36), gS(5,25), gS(0,7), gS(0,6), gS(3,25), gS(0,48), gS(-12,-35),
           gS(-2,-1), gS(29,48), gS(34,44), gS(13,27), gS(13,25), gS(36,49), gS(29,55), gS(-7,-5),
           gS(-9,-8), gS(23,28), gS(31,45), gS(1,48), gS(10,45), gS(35,50), gS(31,35), gS(-37,-7),
           gS(-23,-38), gS(21,1), gS(26,28), gS(-15,51), gS(2,49), gS(11,35), gS(22,6), gS(-56,-27),
           gS(-54,-23), gS(-11,-9), gS(-8,8), gS(-30,30), gS(-17,26), gS(-18,16), gS(-6,-9), gS(-51,-25),
           gS(0,-17), gS(0,-7), gS(-23,6), gS(-64,14), gS(-44,8), gS(-43,13), gS(3,-8), gS(-3,-26),
           gS(-32,-65), gS(-1,-38), gS(-35,-16), gS(-37,-46), gS(-26,-60), gS(-51,-26), gS(-8,-45), gS(-18,-95),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(60,100), gS(47,109), gS(67,100), gS(73,84), gS(80,81), gS(84,85), gS(-8,106), gS(-21,98),
           gS(-10,72), gS(23,65), gS(39,49), gS(43,21), gS(55,24), gS(96,41), gS(62,51), gS(6,52),
           gS(-6,29), gS(7,25), gS(8,20), gS(24,-1), gS(32,5), gS(31,16), gS(10,31), gS(-7,22),
           gS(-15,6), gS(-5,11), gS(-2,0), gS(5,0), gS(10,2), gS(5,7), gS(2,8), gS(-14,-4),
           gS(-19,-1), gS(-13,-1), gS(-12,5), gS(-11,5), gS(-4,11), gS(-5,18), gS(2,-2), gS(-14,-13),
           gS(-18,5), gS(-11,7), gS(-18,24), gS(-11,23), gS(-15,30), gS(0,40), gS(5,9), gS(-18,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(30,41), gS(24,46), gS(1,52), gS(9,45), gS(12,42), gS(25,42), gS(42,37), gS(53,36),
           gS(-7,43), gS(-12,51), gS(11,46), gS(23,48), gS(21,46), gS(39,26), gS(6,42), gS(13,37),
           gS(-10,27), gS(30,15), gS(17,28), gS(40,14), gS(51,7), gS(45,19), gS(69,1), gS(11,20),
           gS(-7,13), gS(9,15), gS(17,18), gS(38,10), gS(31,9), gS(25,10), gS(27,7), gS(9,10),
           gS(-18,-3), gS(-10,13), gS(-13,13), gS(-1,5), gS(-2,5), gS(-10,10), gS(4,7), gS(-13,-3),
           gS(-22,-21), gS(-7,-6), gS(-13,-9), gS(-4,-15), gS(-3,-14), gS(-10,-10), gS(7,-9), gS(-16,-21),
           gS(-51,-17), gS(-15,-21), gS(-7,-17), gS(0,-22), gS(0,-22), gS(-2,-24), gS(-2,-25), gS(-55,-15),
           gS(-14,-16), gS(-8,-13), gS(-4,-11), gS(2,-19), gS(0,-17), gS(-3,-3), gS(0,-15), gS(-10,-27),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-51,7), gS(-44,10), gS(-76,20), gS(-79,28), gS(-66,23), gS(-92,17), gS(-15,0), gS(-49,0),
           gS(-47,-1), gS(-9,2), gS(-7,10), gS(-13,23), gS(-5,20), gS(0,10), gS(-26,5), gS(-20,-5),
           gS(5,2), gS(4,10), gS(24,6), gS(7,14), gS(24,14), gS(10,14), gS(26,6), gS(17,-1),
           gS(-8,0), gS(19,12), gS(4,10), gS(20,27), gS(18,24), gS(15,10), gS(19,8), gS(-6,3),
           gS(0,-12), gS(7,2), gS(11,15), gS(17,17), gS(24,17), gS(4,9), gS(14,5), gS(3,-13),
           gS(0,-17), gS(11,-5), gS(7,-3), gS(13,8), gS(8,10), gS(11,-2), gS(12,-6), gS(14,-7),
           gS(8,-30), gS(9,-35), gS(10,-14), gS(3,2), gS(7,2), gS(8,-12), gS(17,-24), gS(5,-43),
           gS(6,-40), gS(2,-16), gS(1,1), gS(1,-6), gS(1,-1), gS(0,-3), gS(-2,-10), gS(3,-40),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-144,-69), gS(-36,-19), gS(-64,12), gS(-14,0), gS(0,2), gS(-52,14), gS(-29,-11), gS(-88,-70),
           gS(-10,-26), gS(-11,-7), gS(32,-3), gS(51,23), gS(39,22), gS(55,-10), gS(-20,-3), gS(-7,-22),
           gS(-19,-20), gS(12,-2), gS(32,36), gS(31,40), gS(59,27), gS(46,33), gS(29,-8), gS(5,-16),
           gS(0,-12), gS(8,7), gS(25,37), gS(23,56), gS(13,49), gS(37,33), gS(15,15), gS(8,-4),
           gS(-3,-10), gS(10,10), gS(14,36), gS(18,44), gS(20,49), gS(18,43), gS(25,18), gS(7,1),
           gS(-17,-39), gS(1,-10), gS(5,6), gS(12,27), gS(15,28), gS(11,9), gS(7,-1), gS(-4,-30),
           gS(-21,-18), gS(-27,-3), gS(-6,-16), gS(6,8), gS(0,11), gS(0,-11), gS(-16,-3), gS(-10,-4),
           gS(-39,-49), gS(-11,-32), gS(-10,-21), gS(-7,3), gS(0,3), gS(-7,-12), gS(-10,-22), gS(-39,-47),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-3,6), gS(1,23), gS(2,37), gS(6,50), gS(13,60), gS(33,52), gS(9,52), gS(16,38),
           gS(-15,0), gS(-52,41), gS(-15,32), gS(-39,78), gS(-46,118), gS(1,76), gS(-55,73), gS(-16,41),
           gS(-13,-16), gS(-6,-16), gS(-9,17), gS(-11,46), gS(2,64), gS(10,82), gS(28,50), gS(-3,63),
           gS(-2,-33), gS(0,1), gS(-8,6), gS(-15,54), gS(-14,72), gS(-10,77), gS(1,61), gS(3,34),
           gS(3,-41), gS(5,-13), gS(-1,-7), gS(-8,32), gS(-3,32), gS(-10,30), gS(11,6), gS(0,9),
           gS(1,-58), gS(13,-41), gS(6,-19), gS(5,-26), gS(5,-23), gS(1,-6), gS(16,-30), gS(5,-46),
           gS(1,-65), gS(9,-61), gS(18,-75), gS(11,-44), gS(16,-46), gS(18,-88), gS(16,-82), gS(-9,-56),
           gS(5,-69), gS(8,-81), gS(10,-87), gS(14,-66), gS(15,-71), gS(9,-88), gS(11,-83), gS(1,-74),
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
