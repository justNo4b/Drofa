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
const int  KING_SAFE         = gS(3,26);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-21,-109), gS(-9,-50), gS(-3,-22), gS(0,-5), gS(5,9), gS(7,21), gS(8,28),
           gS(7,31), gS(10,35), gS(14,33), gS(21,30), gS(38,22), gS(50,34), gS(80,0),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-8,-76), gS(0,-30), gS(3,-4), gS(6,9), gS(9,18),
           gS(12,25), gS(17,24), gS(24,16), gS(34,1),
};

const int  KING_MOBILITY[9] = {
           gS(13,1), gS(14,1), gS(7,7), gS(2,11), gS(-4,9),
           gS(-8,5), gS(-5,3), gS(-14,-3), gS(-7,-16),
};

const int  ROOK_MOBILITY[15] = {
           gS(-18,-97), gS(-12,-66), gS(-8,-33), gS(-6,-13), gS(-7,5), gS(-8,19), gS(-2,22),
           gS(1,25), gS(6,29), gS(11,34), gS(12,38), gS(17,40), gS(20,41), gS(35,27),
           gS(87,0),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-36,-89), gS(-18,-141), gS(-4,-127), gS(-1,-79), gS(0,-46), gS(1,-23), gS(3,-6),
           gS(5,7), gS(7,16), gS(9,23), gS(10,30), gS(11,35), gS(11,39), gS(10,45),
           gS(9,51), gS(6,55), gS(4,57), gS(3,57), gS(3,57), gS(8,53), gS(10,49),
           gS(11,43), gS(10,38), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
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
        [ROOK] = gS(416,729),
        [KNIGHT] = gS(286,404),
        [BISHOP] = gS(306,438),
        [QUEEN] = gS(917,1352),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(15,76);

const int  PAWN_SUPPORTED = gS(8,6);

const int  DOUBLED_PAWN_PENALTY = gS(-13,-25);

const int  ISOLATED_PAWN_PENALTY = gS(-8,-12);

const int  PAWN_BLOCKED = gS(2,25);

const int  PASSER_BLOCKED = gS(3,44);

const int  BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(8,26), gS(-5,23), gS(-10,9),
           gS(-7,-2), gS(-7,-6), gS(7,-10), gS(-2,-20),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(41,81), gS(-18,14), gS(-4,-3),
           gS(-3,-16), gS(-1,-27), gS(6,-38), gS(14,-45),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(24,9), gS(26,11), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,9), gS(17,12), };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-26,-18), gS(-23,-52), gS(-31,-66), gS(-18,-9),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-13,-25), gS(-17,-13), gS(-8,21),
           gS(16,53), gS(27,113), gS(71,113), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(12,16), gS(8,16), gS(0,4), gS(-2,1),
           gS(-4,0), gS(-20,3), gS(-7,18), gS(13,12),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-82), gS(-8,-43), gS(-7,-30), gS(-6,-18), gS(-8,-29), gS(-9,-25), gS(-10,-25), gS(-12,-75),
           gS(-13,-37), gS(0,34), gS(5,23), gS(0,6), gS(-1,5), gS(3,22), gS(-2,44), gS(-12,-35),
           gS(-3,-3), gS(23,37), gS(25,32), gS(8,19), gS(8,16), gS(26,34), gS(23,40), gS(-7,-8),
           gS(-9,-9), gS(18,20), gS(25,27), gS(-2,29), gS(7,25), gS(29,25), gS(28,19), gS(-32,-11),
           gS(-19,-34), gS(20,3), gS(26,17), gS(-14,31), gS(3,27), gS(13,19), gS(26,5), gS(-49,-23),
           gS(-48,-16), gS(-8,3), gS(-6,12), gS(-26,26), gS(-13,22), gS(-14,16), gS(-6,2), gS(-51,-13),
           gS(0,-11), gS(-3,6), gS(-23,16), gS(-62,24), gS(-44,19), gS(-44,23), gS(-2,4), gS(-3,-21),
           gS(-28,-57), gS(3,-28), gS(-29,-6), gS(-37,-27), gS(-27,-36), gS(-54,-7), gS(-5,-26), gS(-16,-74),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(64,81), gS(50,92), gS(69,90), gS(73,76), gS(81,75), gS(80,81), gS(-6,98), gS(-19,92),
           gS(-3,49), gS(25,44), gS(40,38), gS(41,20), gS(51,20), gS(95,26), gS(60,31), gS(9,36),
           gS(-5,24), gS(1,23), gS(5,20), gS(20,4), gS(27,8), gS(30,13), gS(9,19), gS(-3,13),
           gS(-14,10), gS(-9,13), gS(0,6), gS(4,4), gS(9,5), gS(10,7), gS(0,6), gS(-13,1),
           gS(-17,4), gS(-15,4), gS(-10,10), gS(-5,10), gS(1,13), gS(1,13), gS(3,-1), gS(-12,-4),
           gS(-15,8), gS(-12,8), gS(-14,20), gS(-6,21), gS(-11,27), gS(10,22), gS(9,2), gS(-14,-8),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(28,26), gS(22,32), gS(0,40), gS(7,35), gS(10,35), gS(23,34), gS(38,30), gS(49,27),
           gS(-3,28), gS(-8,32), gS(14,28), gS(23,29), gS(20,30), gS(39,12), gS(5,24), gS(13,20),
           gS(-9,19), gS(33,5), gS(20,16), gS(42,5), gS(52,0), gS(46,7), gS(70,-9), gS(11,11),
           gS(-6,10), gS(10,9), gS(18,11), gS(39,3), gS(33,3), gS(25,3), gS(28,0), gS(10,4),
           gS(-18,0), gS(-10,11), gS(-12,12), gS(-4,5), gS(-5,4), gS(-9,8), gS(4,4), gS(-13,-2),
           gS(-22,-16), gS(-7,-4), gS(-14,-6), gS(-5,-11), gS(-4,-12), gS(-11,-8), gS(7,-9), gS(-16,-18),
           gS(-52,-11), gS(-15,-18), gS(-9,-14), gS(-1,-19), gS(0,-20), gS(-3,-24), gS(-2,-24), gS(-55,-11),
           gS(-14,-14), gS(-7,-13), gS(-4,-11), gS(2,-18), gS(1,-18), gS(-1,-9), gS(0,-17), gS(-10,-23),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-42,12), gS(-37,8), gS(-67,10), gS(-73,16), gS(-59,12), gS(-85,8), gS(-13,0), gS(-41,5),
           gS(-46,2), gS(-2,8), gS(-5,8), gS(-20,12), gS(-14,10), gS(0,9), gS(-20,11), gS(-20,-3),
           gS(-1,-4), gS(4,5), gS(28,7), gS(8,8), gS(24,7), gS(16,11), gS(24,0), gS(11,-7),
           gS(-14,-6), gS(14,-2), gS(2,4), gS(21,11), gS(16,11), gS(13,0), gS(14,-6), gS(-10,-5),
           gS(-2,-17), gS(2,-8), gS(9,6), gS(16,4), gS(23,4), gS(4,2), gS(10,-7), gS(0,-18),
           gS(0,-18), gS(12,-5), gS(12,0), gS(16,1), gS(11,3), gS(16,0), gS(13,-6), gS(14,-10),
           gS(13,-23), gS(15,-21), gS(13,-13), gS(4,-6), gS(9,-4), gS(11,-12), gS(24,-11), gS(11,-32),
           gS(15,-26), gS(5,-10), gS(2,-7), gS(0,-11), gS(0,-7), gS(3,-4), gS(1,-6), gS(12,-25),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-135,-74), gS(-31,-23), gS(-55,7), gS(-14,-1), gS(-3,0), gS(-43,9), gS(-24,-14), gS(-75,-64),
           gS(-12,-25), gS(-12,-6), gS(30,-5), gS(47,16), gS(35,15), gS(53,-13), gS(-21,-5), gS(-8,-22),
           gS(-20,-19), gS(9,-6), gS(29,28), gS(29,29), gS(55,19), gS(45,21), gS(28,-13), gS(1,-16),
           gS(-2,-11), gS(5,4), gS(23,26), gS(22,37), gS(10,35), gS(35,21), gS(14,7), gS(6,-8),
           gS(-6,-10), gS(8,7), gS(12,28), gS(14,30), gS(17,33), gS(17,31), gS(23,11), gS(5,-1),
           gS(-19,-34), gS(0,-7), gS(3,8), gS(13,22), gS(17,20), gS(10,6), gS(5,-1), gS(-7,-25),
           gS(-21,-15), gS(-26,0), gS(-6,-11), gS(7,3), gS(3,7), gS(0,-10), gS(-14,0), gS(-8,-2),
           gS(-38,-42), gS(-11,-28), gS(-10,-19), gS(-8,0), gS(0,0), gS(-5,-7), gS(-9,-19), gS(-39,-40),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-5,3), gS(0,19), gS(1,30), gS(7,39), gS(13,48), gS(33,45), gS(12,44), gS(16,32),
           gS(-18,-1), gS(-57,40), gS(-17,29), gS(-39,68), gS(-49,108), gS(0,67), gS(-53,63), gS(-18,37),
           gS(-15,-15), gS(-7,-15), gS(-11,16), gS(-14,42), gS(0,60), gS(7,77), gS(24,48), gS(-7,61),
           gS(-4,-31), gS(-3,2), gS(-11,6), gS(-17,49), gS(-16,66), gS(-15,73), gS(-2,60), gS(0,33),
           gS(0,-41), gS(2,-13), gS(-3,-7), gS(-10,29), gS(-6,27), gS(-12,26), gS(8,3), gS(-1,7),
           gS(0,-58), gS(11,-42), gS(5,-22), gS(3,-30), gS(3,-28), gS(0,-13), gS(15,-33), gS(3,-45),
           gS(0,-66), gS(8,-63), gS(17,-79), gS(10,-49), gS(15,-53), gS(17,-92), gS(14,-84), gS(-12,-53),
           gS(3,-70), gS(7,-82), gS(8,-88), gS(12,-68), gS(13,-73), gS(7,-89), gS(9,-79), gS(-2,-68),
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
