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
const int  KING_MED_DANGER   = gS(-8,-15);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(0,8);   // слабый пешечный щит
const int  KING_SAFE         = gS(4,23);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-15,-105), gS(-3,-47), gS(2,-20), gS(4,-4), gS(7,10), gS(7,21), gS(6,27),
           gS(4,30), gS(4,33), gS(5,31), gS(9,27), gS(24,18), gS(43,28), gS(76,-6),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-6,-75), gS(2,-30), gS(6,-5), gS(9,9), gS(12,17),
           gS(14,24), gS(19,23), gS(26,15), gS(35,0),
};

const int  KING_MOBILITY[9] = {
           gS(12,-2), gS(15,-2), gS(8,5), gS(3,10), gS(-3,9),
           gS(-8,5), gS(-5,3), gS(-14,-3), gS(-9,-15),
};

const int  ROOK_MOBILITY[15] = {
           gS(-18,-102), gS(-11,-66), gS(-7,-33), gS(-5,-13), gS(-6,5), gS(-8,19), gS(-3,22),
           gS(1,25), gS(5,30), gS(10,35), gS(10,39), gS(16,41), gS(19,41), gS(34,27),
           gS(87,0),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-46,-102), gS(-41,-166), gS(-15,-135), gS(-5,-82), gS(0,-49), gS(2,-27), gS(4,-10),
           gS(6,2), gS(7,17), gS(8,26), gS(9,34), gS(11,38), gS(12,42), gS(12,46),
           gS(13,49), gS(10,55), gS(9,57), gS(7,59), gS(6,58), gS(9,55), gS(12,49),
           gS(13,44), gS(11,41), gS(21,29), gS(12,25), gS(20,23), gS(26,29), gS(24,26),
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
        [PAWN] = gS(80,120),
        [ROOK] = gS(427,730),
        [KNIGHT] = gS(289,403),
        [BISHOP] = gS(305,434),
        [QUEEN] = gS(938,1347),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(16,76);

const int  PAWN_SUPPORTED = gS(8,5);

const int  DOUBLED_PAWN_PENALTY = gS(-9,-29);

const int  ISOLATED_PAWN_PENALTY = gS(-3,-5);

const int  PAWN_BLOCKED = gS(2,26);

const int  PASSER_BLOCKED = gS(3,46);

const int  PAWN_DISTORTION = gS(-1,-1);

const int  BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int  BISHOP_CENTER_CONTROL = gS(11,7);

const int  MINOR_BEHIND_PAWN = gS(6,21);

const int  MINOR_BEHIND_PASSER = gS(8,13);

const int  KING_AHEAD_PASSER = gS(-11,8);

const int  KING_EQUAL_PASSER = gS(7,1);

const int  KING_BEHIND_PASSER = gS(4,-5);


const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(7,28), gS(-3,24), gS(-8,10),
           gS(-5,-1), gS(-4,-4), gS(10,-9), gS(0,-18),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(43,80), gS(-18,13), gS(-4,-3),
           gS(-4,-16), gS(-2,-27), gS(5,-39), gS(12,-45),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(24,7), gS(25,9) };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,6), gS(17,9) };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-26,-18), gS(-25,-52), gS(-30,-72), gS(-17,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-14,-27), gS(-17,-13), gS(-8,21),
           gS(15,54), gS(28,114), gS(82,122), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(11,13), gS(8,15), gS(1,4), gS(-1,0),
           gS(-3,0), gS(-16,3), gS(-6,17), gS(12,10),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-95), gS(-3,-48), gS(-2,-34), gS(-1,-18), gS(-3,-32), gS(-4,-27), gS(-5,-25), gS(-12,-87),
           gS(-13,-37), gS(0,33), gS(5,22), gS(0,5), gS(0,4), gS(3,22), gS(0,43), gS(-12,-36),
           gS(-3,-4), gS(28,36), gS(33,31), gS(13,19), gS(13,16), gS(35,33), gS(28,39), gS(-7,-9),
           gS(-9,-10), gS(23,19), gS(30,27), gS(0,30), gS(8,26), gS(34,24), gS(31,19), gS(-37,-10),
           gS(-23,-33), gS(22,3), gS(26,18), gS(-15,32), gS(3,29), gS(13,20), gS(25,6), gS(-54,-21),
           gS(-53,-14), gS(-10,3), gS(-7,13), gS(-27,27), gS(-15,24), gS(-15,17), gS(-8,4), gS(-54,-11),
           gS(-1,-11), gS(-3,7), gS(-25,17), gS(-64,25), gS(-45,20), gS(-45,23), gS(-1,5), gS(-3,-21),
           gS(-28,-55), gS(3,-27), gS(-30,-5), gS(-40,-25), gS(-30,-35), gS(-57,-5), gS(-5,-26), gS(-15,-73),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(63,71), gS(51,96), gS(70,92), gS(76,78), gS(82,77), gS(84,79), gS(-8,100), gS(-23,82),
           gS(-1,45), gS(31,51), gS(43,43), gS(45,25), gS(56,25), gS(98,30), gS(65,36), gS(10,32),
           gS(-4,19), gS(6,25), gS(8,21), gS(22,7), gS(31,10), gS(32,14), gS(13,21), gS(-4,8),
           gS(-15,4), gS(-6,12), gS(-4,5), gS(4,3), gS(9,4), gS(5,5), gS(2,5), gS(-15,-4),
           gS(-20,-2), gS(-16,2), gS(-13,6), gS(-12,7), gS(-5,10), gS(-3,9), gS(1,-3), gS(-16,-12),
           gS(-18,1), gS(-12,5), gS(-19,16), gS(-12,17), gS(-15,22), gS(5,17), gS(6,0), gS(-19,-16),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(30,25), gS(23,32), gS(0,40), gS(7,35), gS(10,35), gS(23,34), gS(40,29), gS(51,26),
           gS(-1,27), gS(-7,32), gS(15,28), gS(24,29), gS(21,30), gS(37,12), gS(5,24), gS(13,20),
           gS(-7,19), gS(33,4), gS(20,16), gS(42,5), gS(52,0), gS(42,7), gS(65,-8), gS(9,11),
           gS(-5,10), gS(11,8), gS(20,11), gS(41,4), gS(34,3), gS(26,3), gS(28,0), gS(10,4),
           gS(-17,-1), gS(-10,11), gS(-12,12), gS(-1,4), gS(-2,3), gS(-10,8), gS(4,4), gS(-12,-3),
           gS(-21,-17), gS(-7,-4), gS(-14,-6), gS(-6,-12), gS(-4,-12), gS(-11,-9), gS(7,-10), gS(-15,-19),
           gS(-50,-12), gS(-15,-18), gS(-9,-14), gS(-2,-18), gS(-1,-19), gS(-2,-24), gS(-2,-24), gS(-54,-11),
           gS(-13,-15), gS(-8,-12), gS(-5,-9), gS(1,-17), gS(0,-17), gS(-1,-8), gS(0,-16), gS(-9,-23),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-51,6), gS(-44,8), gS(-76,17), gS(-80,23), gS(-67,19), gS(-92,16), gS(-16,0), gS(-50,0),
           gS(-47,1), gS(-8,3), gS(-5,8), gS(-13,18), gS(-6,16), gS(-1,8), gS(-29,5), gS(-35,-4),
           gS(4,1), gS(5,7), gS(26,3), gS(10,9), gS(25,8), gS(7,9), gS(18,1), gS(4,0),
           gS(-7,0), gS(22,4), gS(7,4), gS(23,14), gS(20,13), gS(14,2), gS(17,1), gS(-6,0),
           gS(2,-12), gS(9,-2), gS(13,7), gS(19,7), gS(24,6), gS(5,4), gS(16,-1), gS(6,-15),
           gS(1,-17), gS(12,-7), gS(8,-4), gS(14,1), gS(9,2), gS(13,-6), gS(15,-9), gS(17,-12),
           gS(9,-29), gS(10,-33), gS(11,-15), gS(3,-2), gS(8,-4), gS(10,-16), gS(20,-28), gS(6,-41),
           gS(7,-37), gS(1,-14), gS(0,-3), gS(1,-6), gS(2,-3), gS(1,-9), gS(0,-11), gS(5,-38),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-143,-69), gS(-36,-19), gS(-64,12), gS(-14,0), gS(0,1), gS(-53,14), gS(-29,-11), gS(-88,-71),
           gS(-10,-24), gS(-11,-4), gS(33,-4), gS(50,17), gS(38,17), gS(55,-12), gS(-21,-2), gS(-7,-21),
           gS(-18,-18), gS(13,-4), gS(33,29), gS(33,30), gS(60,19), gS(47,24), gS(30,-11), gS(6,-16),
           gS(0,-10), gS(8,5), gS(27,27), gS(26,38), gS(15,36), gS(40,22), gS(17,8), gS(9,-7),
           gS(-4,-9), gS(10,8), gS(14,30), gS(21,31), gS(24,33), gS(20,32), gS(26,11), gS(7,-1),
           gS(-19,-35), gS(0,-7), gS(5,8), gS(13,22), gS(17,20), gS(13,5), gS(7,-2), gS(-4,-27),
           gS(-21,-18), gS(-28,-2), gS(-7,-13), gS(6,4), gS(1,6), gS(0,-12), gS(-17,-4), gS(-11,-5),
           gS(-41,-47), gS(-13,-29), gS(-11,-19), gS(-8,1), gS(0,0), gS(-7,-13), gS(-11,-21), gS(-41,-46),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-4,3), gS(0,20), gS(0,34), gS(3,44), gS(9,54), gS(31,49), gS(8,49), gS(14,35),
           gS(-16,0), gS(-56,41), gS(-16,30), gS(-40,74), gS(-48,112), gS(0,72), gS(-57,73), gS(-18,40),
           gS(-14,-15), gS(-5,-15), gS(-9,17), gS(-11,44), gS(2,61), gS(9,78), gS(26,47), gS(-4,60),
           gS(-2,-31), gS(0,1), gS(-8,6), gS(-16,51), gS(-15,66), gS(-11,73), gS(1,58), gS(4,31),
           gS(3,-41), gS(5,-14), gS(-2,-8), gS(-8,28), gS(-4,27), gS(-11,25), gS(12,2), gS(0,7),
           gS(1,-58), gS(13,-43), gS(5,-23), gS(5,-32), gS(4,-28), gS(1,-14), gS(17,-34), gS(6,-47),
           gS(1,-67), gS(7,-64), gS(16,-80), gS(6,-45), gS(14,-52), gS(16,-93), gS(14,-85), gS(-9,-58),
           gS(-2,-72), gS(0,-83), gS(-1,-85), gS(9,-73), gS(9,-73), gS(2,-90), gS(4,-86), gS(-4,-77),
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
