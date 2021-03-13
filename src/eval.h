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
const int  KING_LOW_DANGER   = gS(1,7);   // слабый пешечный щит
const int  KING_SAFE         = gS(3,26);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-21,-109), gS(-9,-50), gS(-3,-22), gS(0,-5), gS(4,9), gS(7,21), gS(8,28),
           gS(7,32), gS(9,36), gS(14,34), gS(20,31), gS(37,22), gS(49,34), gS(79,1),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-9,-78), gS(0,-30), gS(3,-4), gS(6,10), gS(9,20),
           gS(12,27), gS(17,25), gS(24,17), gS(34,1),
};

const int  KING_MOBILITY[9] = {
           gS(13,1), gS(14,2), gS(7,8), gS(2,11), gS(-4,9),
           gS(-8,6), gS(-5,3), gS(-14,-2), gS(-7,-15),
};

const int  ROOK_MOBILITY[15] = {
           gS(-18,-94), gS(-12,-62), gS(-9,-29), gS(-6,-9), gS(-7,8), gS(-9,20), gS(-3,23),
           gS(1,24), gS(6,28), gS(11,32), gS(12,35), gS(17,37), gS(21,38), gS(36,24),
           gS(87,-2),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-35,-83), gS(-17,-128), gS(-3,-124), gS(0,-80), gS(1,-45), gS(2,-22), gS(4,-4),
           gS(5,8), gS(8,17), gS(10,24), gS(11,30), gS(12,35), gS(12,39), gS(11,44),
           gS(9,49), gS(6,53), gS(4,56), gS(3,56), gS(2,56), gS(6,53), gS(8,48),
           gS(10,41), gS(10,37), gS(21,28), gS(14,24), gS(20,23), gS(26,29), gS(24,26),
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
        [PAWN] = gS(75,111),
        [ROOK] = gS(415,729),
        [KNIGHT] = gS(284,405),
        [BISHOP] = gS(304,438),
        [QUEEN] = gS(907,1355),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(15,75);

const int  PAWN_SUPPORTED = gS(7,5);

const int  DOUBLED_PAWN_PENALTY = gS(-13,-25);

const int  ISOLATED_PAWN_PENALTY = gS(-8,-12);

const int  BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(6,26), gS(-5,22), gS(-10,7),
           gS(-7,-4), gS(-7,-7), gS(8,-12), gS(-3,-20),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(37,80), gS(-16,14), gS(-3,-2),
           gS(-3,-15), gS(-1,-26), gS(6,-37), gS(14,-44),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {  gS(23,8), gS(27,15), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(5,13), gS(18,17), };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-26,-19), gS(-24,-48), gS(-34,-51), gS(-19,-8),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-25), gS(-16,-12), gS(-7,21),
           gS(17,52), gS(27,112), gS(67,108), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(13,16), gS(7,16), gS(-1,4), gS(-3,2),
           gS(-5,0), gS(-19,2), gS(-8,18), gS(12,12),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-76), gS(-10,-41), gS(-9,-29), gS(-8,-17), gS(-10,-28), gS(-11,-24), gS(-12,-25), gS(-12,-69),
           gS(-13,-37), gS(0,34), gS(4,24), gS(-2,7), gS(-3,5), gS(2,23), gS(-4,45), gS(-12,-33),
           gS(-3,-3), gS(20,39), gS(22,33), gS(7,20), gS(7,17), gS(23,35), gS(20,41), gS(-7,-8),
           gS(-9,-9), gS(15,21), gS(23,28), gS(-4,29), gS(7,25), gS(26,25), gS(27,19), gS(-30,-11),
           gS(-18,-35), gS(19,3), gS(25,17), gS(-13,30), gS(3,26), gS(13,18), gS(27,4), gS(-46,-24),
           gS(-45,-17), gS(-8,2), gS(-5,11), gS(-26,25), gS(-13,21), gS(-13,15), gS(-5,1), gS(-51,-13),
           gS(0,-12), gS(-2,6), gS(-23,15), gS(-61,23), gS(-43,19), gS(-44,22), gS(-2,3), gS(-3,-22),
           gS(-27,-58), gS(3,-28), gS(-29,-7), gS(-36,-28), gS(-27,-38), gS(-54,-7), gS(-5,-27), gS(-16,-75),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(63,82), gS(51,92), gS(69,89), gS(71,75), gS(81,75), gS(78,83), gS(-4,99), gS(-18,94),
           gS(-3,51), gS(25,45), gS(40,38), gS(40,19), gS(51,20), gS(96,27), gS(60,32), gS(10,38),
           gS(-5,26), gS(1,24), gS(5,21), gS(20,5), gS(27,9), gS(29,14), gS(9,20), gS(-3,15),
           gS(-14,11), gS(-9,14), gS(0,6), gS(4,4), gS(9,5), gS(10,9), gS(0,7), gS(-13,2),
           gS(-17,5), gS(-15,4), gS(-9,11), gS(-5,10), gS(1,13), gS(1,14), gS(3,0), gS(-12,-3),
           gS(-14,10), gS(-11,9), gS(-14,21), gS(-6,22), gS(-10,28), gS(10,23), gS(9,3), gS(-14,-7),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(28,26), gS(22,31), gS(0,40), gS(7,35), gS(10,35), gS(23,34), gS(37,30), gS(48,27),
           gS(-3,27), gS(-8,32), gS(14,28), gS(23,29), gS(20,30), gS(39,11), gS(5,24), gS(13,20),
           gS(-6,20), gS(35,5), gS(21,17), gS(42,5), gS(52,0), gS(49,9), gS(72,-8), gS(14,11),
           gS(-5,11), gS(11,10), gS(19,12), gS(41,4), gS(34,4), gS(25,5), gS(29,1), gS(11,6),
           gS(-18,1), gS(-11,12), gS(-12,13), gS(-3,6), gS(-5,5), gS(-9,8), gS(3,4), gS(-13,0),
           gS(-23,-15), gS(-8,-5), gS(-15,-5), gS(-6,-10), gS(-5,-11), gS(-12,-9), gS(6,-11), gS(-17,-18),
           gS(-53,-11), gS(-16,-19), gS(-10,-15), gS(-2,-19), gS(-1,-21), gS(-4,-26), gS(-3,-26), gS(-56,-11),
           gS(-15,-15), gS(-8,-14), gS(-5,-11), gS(1,-18), gS(0,-18), gS(-2,-11), gS(0,-18), gS(-11,-24),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-41,10), gS(-35,6), gS(-62,6), gS(-68,12), gS(-54,8), gS(-79,4), gS(-12,-1), gS(-40,3),
           gS(-47,1), gS(-3,6), gS(-5,6), gS(-21,9), gS(-15,7), gS(0,6), gS(-21,9), gS(-21,-4),
           gS(-2,-2), gS(5,6), gS(29,6), gS(8,6), gS(24,6), gS(19,13), gS(26,1), gS(8,-3),
           gS(-14,-5), gS(14,0), gS(3,5), gS(22,11), gS(18,13), gS(13,2), gS(15,0), gS(-10,-2),
           gS(-3,-13), gS(2,-6), gS(9,7), gS(17,6), gS(23,5), gS(3,4), gS(9,-5), gS(0,-15),
           gS(-1,-16), gS(11,-4), gS(11,0), gS(16,2), gS(11,4), gS(15,0), gS(13,-6), gS(13,-9),
           gS(11,-19), gS(14,-22), gS(12,-13), gS(3,-7), gS(8,-6), gS(10,-13), gS(23,-13), gS(10,-31),
           gS(13,-26), gS(4,-10), gS(1,-9), gS(0,-12), gS(0,-9), gS(2,-5), gS(0,-7), gS(11,-25),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-131,-76), gS(-28,-25), gS(-51,4), gS(-14,-3), gS(-5,0), gS(-39,6), gS(-21,-14), gS(-69,-61),
           gS(-13,-26), gS(-13,-8), gS(29,-8), gS(45,13), gS(34,13), gS(52,-15), gS(-21,-7), gS(-8,-23),
           gS(-18,-18), gS(10,-5), gS(29,28), gS(29,28), gS(55,18), gS(47,23), gS(30,-10), gS(1,-15),
           gS(-2,-10), gS(6,7), gS(23,27), gS(23,37), gS(11,38), gS(35,23), gS(14,15), gS(6,-4),
           gS(-6,-6), gS(8,9), gS(12,30), gS(15,32), gS(17,34), gS(17,33), gS(22,13), gS(4,1),
           gS(-20,-32), gS(-1,-6), gS(2,7), gS(12,22), gS(16,20), gS(9,4), gS(4,-2), gS(-8,-24),
           gS(-23,-13), gS(-27,1), gS(-8,-13), gS(6,1), gS(2,5), gS(-2,-12), gS(-15,0), gS(-10,-2),
           gS(-39,-40), gS(-12,-29), gS(-11,-20), gS(-9,-1), gS(-1,-3), gS(-6,-10), gS(-10,-20), gS(-40,-39),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-7,3), gS(0,18), gS(2,28), gS(8,37), gS(14,45), gS(33,44), gS(14,42), gS(16,31),
           gS(-18,-2), gS(-57,38), gS(-17,27), gS(-36,64), gS(-48,104), gS(1,63), gS(-51,57), gS(-18,35),
           gS(-14,-15), gS(-6,-14), gS(-9,16), gS(-12,41), gS(0,59), gS(13,79), gS(27,51), gS(-5,62),
           gS(-4,-31), gS(-2,5), gS(-10,7), gS(-15,50), gS(-14,68), gS(-15,75), gS(-1,65), gS(0,35),
           gS(0,-40), gS(2,-13), gS(-3,-5), gS(-9,31), gS(-6,28), gS(-13,28), gS(7,4), gS(-3,9),
           gS(-1,-57), gS(9,-42), gS(4,-22), gS(2,-28), gS(2,-26), gS(0,-13), gS(13,-34), gS(1,-44),
           gS(0,-65), gS(6,-64), gS(15,-80), gS(8,-50), gS(13,-54), gS(15,-92), gS(13,-85), gS(-14,-50),
           gS(1,-70), gS(5,-82), gS(7,-89), gS(11,-69), gS(12,-74), gS(5,-88), gS(6,-74), gS(-5,-63),
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
