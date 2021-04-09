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
const int  KING_LOW_DANGER   = gS(1,8);   // слабый пешечный щит
const int  KING_SAFE         = gS(4,25);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int  BISHOP_MOBILITY[14] = {
           gS(-17,-105), gS(-5,-46), gS(0,-20), gS(3,-3), gS(6,10), gS(7,21), gS(7,28),
           gS(5,30), gS(6,32), gS(9,30), gS(15,26), gS(31,17), gS(47,30), gS(79,-4),
};

const int  KNIGHT_MOBILITY[9] = {
           gS(-8,-76), gS(1,-30), gS(4,-5), gS(7,9), gS(10,17),
           gS(13,24), gS(17,23), gS(24,16), gS(34,1),
};

const int  KING_MOBILITY[9] = {
           gS(13,0), gS(14,0), gS(7,6), gS(2,10), gS(-4,8),
           gS(-8,4), gS(-5,2), gS(-14,-3), gS(-8,-16),
};

const int  ROOK_MOBILITY[15] = {
           gS(-19,-98), gS(-12,-66), gS(-8,-33), gS(-6,-13), gS(-7,5), gS(-8,19), gS(-2,21),
           gS(1,25), gS(6,29), gS(11,34), gS(12,38), gS(17,40), gS(21,41), gS(35,27),
           gS(88,0),
};

const int  QUEEN_MOBILITY[28] = {
           gS(-37,-98), gS(-18,-157), gS(-5,-131), gS(-2,-79), gS(0,-45), gS(0,-23), gS(2,-5),
           gS(4,7), gS(6,16), gS(8,23), gS(9,30), gS(11,35), gS(11,39), gS(10,45),
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
        [PAWN] = gS(80,119),
        [ROOK] = gS(422,729),
        [KNIGHT] = gS(288,404),
        [BISHOP] = gS(305,434),
        [QUEEN] = gS(930,1349),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int  BISHOP_PAIR_BONUS = gS(15,76);

const int  PAWN_SUPPORTED = gS(7,5);

const int  DOUBLED_PAWN_PENALTY = gS(-10,-28);

const int  ISOLATED_PAWN_PENALTY = gS(-3,-6);

const int  PAWN_BLOCKED = gS(1,25);

const int  PASSER_BLOCKED = gS(2,44);

const int  PAWN_DISTORTION = gS(-1,-1);

const int  BISHOP_RAMMED_PENALTY = gS(-1,-9);

const int  BISHOP_CENTER_CONTROL = gS(10,6);

const int  MINOR_BEHIND_PAWN = gS(0,0);

const int  MINOR_BEHIND_PASSER = gS(0,0);

const int  KING_AHEAD_PASSER = gS(-8,8);

const int  KING_EQUAL_PASSER = gS(5,1);

const int  KING_BEHIND_PASSER = gS(4,-5);

const int  KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(7,28), gS(-3,24), gS(-8,9),
           gS(-6,-1), gS(-5,-4), gS(9,-9), gS(-1,-18),
           gS(0,-10),
};

const int  KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(41,80), gS(-19,14), gS(-5,-3),
           gS(-4,-16), gS(-2,-27), gS(5,-38), gS(14,-46),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = { gS(24,7), gS(25,9) };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,7), gS(17,10) };


const int  HANGING_PIECE[5] = {
           gS(0,0), gS(-27,-18), gS(-25,-52), gS(-32,-72), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int  PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-14,-27), gS(-17,-14), gS(-8,21),
           gS(16,54), gS(28,115), gS(79,121), gS(0,0),
};

const int  PASSED_PAWN_FILES[8] = {
           gS(12,14), gS(8,15), gS(1,4), gS(0,1),
           gS(-3,0), gS(-16,3), gS(-7,18), gS(12,11),
};


const int  KING_PSQT_BLACK[64] = {
           gS(-13,-91), gS(-5,-46), gS(-4,-33), gS(-3,-18), gS(-5,-31), gS(-6,-27), gS(-7,-25), gS(-12,-83),
           gS(-13,-37), gS(0,33), gS(5,22), gS(0,5), gS(0,4), gS(3,22), gS(0,43), gS(-12,-36),
           gS(-3,-4), gS(26,37), gS(31,32), gS(11,19), gS(11,16), gS(32,33), gS(26,39), gS(-7,-9),
           gS(-9,-10), gS(21,19), gS(28,27), gS(0,29), gS(8,26), gS(32,24), gS(30,19), gS(-35,-10),
           gS(-21,-33), gS(22,3), gS(26,17), gS(-15,32), gS(3,28), gS(13,19), gS(25,5), gS(-52,-21),
           gS(-51,-14), gS(-9,3), gS(-7,12), gS(-27,26), gS(-15,23), gS(-15,17), gS(-8,3), gS(-52,-12),
           gS(-1,-11), gS(-3,7), gS(-24,17), gS(-63,25), gS(-44,20), gS(-45,23), gS(-2,5), gS(-4,-20),
           gS(-28,-56), gS(3,-27), gS(-29,-5), gS(-37,-25), gS(-27,-35), gS(-55,-6), gS(-5,-25), gS(-16,-73),
};

const int  PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(63,74), gS(51,95), gS(70,92), gS(75,78), gS(82,77), gS(83,80), gS(-8,99), gS(-21,86),
           gS(-2,46), gS(30,50), gS(42,42), gS(44,24), gS(55,24), gS(98,30), gS(64,36), gS(10,33),
           gS(-5,20), gS(5,25), gS(7,22), gS(21,8), gS(30,11), gS(31,14), gS(13,21), gS(-4,9),
           gS(-15,5), gS(-7,13), gS(-2,5), gS(4,4), gS(9,5), gS(7,6), gS(2,6), gS(-15,-3),
           gS(-20,-1), gS(-14,3), gS(-13,7), gS(-10,8), gS(-3,11), gS(-1,9), gS(4,-2), gS(-15,-11),
           gS(-18,2), gS(-11,5), gS(-17,16), gS(-10,17), gS(-14,22), gS(7,17), gS(8,0), gS(-18,-15),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int  ROOK_PSQT_BLACK[64] = {
           gS(28,25), gS(22,31), gS(0,40), gS(7,35), gS(10,35), gS(23,34), gS(38,29), gS(49,26),
           gS(-3,27), gS(-8,32), gS(14,28), gS(24,29), gS(20,30), gS(39,12), gS(5,24), gS(13,20),
           gS(-9,19), gS(33,4), gS(20,16), gS(42,5), gS(52,0), gS(45,7), gS(69,-9), gS(11,11),
           gS(-6,10), gS(11,9), gS(19,11), gS(41,4), gS(33,4), gS(27,3), gS(29,0), gS(10,4),
           gS(-18,0), gS(-10,11), gS(-12,12), gS(-1,5), gS(-2,4), gS(-9,8), gS(4,5), gS(-13,-2),
           gS(-22,-16), gS(-7,-4), gS(-14,-6), gS(-5,-11), gS(-4,-12), gS(-11,-8), gS(7,-9), gS(-16,-18),
           gS(-52,-11), gS(-15,-18), gS(-8,-14), gS(-1,-19), gS(0,-20), gS(-3,-24), gS(-2,-24), gS(-55,-11),
           gS(-14,-14), gS(-8,-13), gS(-4,-10), gS(2,-18), gS(1,-18), gS(-1,-9), gS(0,-17), gS(-11,-23),
};

const int  BISHOP_PSQT_BLACK[64] = {
           gS(-48,7), gS(-42,7), gS(-74,14), gS(-78,20), gS(-65,16), gS(-90,13), gS(-15,0), gS(-46,1),
           gS(-49,0), gS(-11,2), gS(-7,7), gS(-16,16), gS(-9,14), gS(0,7), gS(-27,5), gS(-23,-5),
           gS(2,-1), gS(4,5), gS(24,2), gS(7,7), gS(24,7), gS(11,8), gS(25,0), gS(16,-4),
           gS(-11,-2), gS(19,2), gS(4,3), gS(21,12), gS(18,11), gS(15,1), gS(19,-1), gS(-8,-1),
           gS(0,-13), gS(6,-3), gS(10,6), gS(17,5), gS(25,5), gS(3,3), gS(14,-2), gS(2,-16),
           gS(1,-15), gS(11,-6), gS(9,-3), gS(17,1), gS(11,2), gS(13,-4), gS(12,-7), gS(15,-8),
           gS(11,-26), gS(12,-26), gS(13,-14), gS(6,-3), gS(11,-1), gS(11,-13), gS(22,-18), gS(9,-34),
           gS(10,-32), gS(4,-13), gS(3,-5), gS(2,-9), gS(2,-5), gS(4,-2), gS(0,-9), gS(8,-31),
};

const int  KNIGHT_PSQT_BLACK[64] = {
           gS(-141,-71), gS(-34,-21), gS(-61,10), gS(-14,-1), gS(-1,0), gS(-49,12), gS(-27,-13), gS(-84,-69),
           gS(-12,-25), gS(-13,-6), gS(30,-5), gS(47,16), gS(36,15), gS(53,-13), gS(-22,-4), gS(-8,-22),
           gS(-20,-19), gS(11,-5), gS(30,28), gS(30,29), gS(57,18), gS(45,22), gS(28,-13), gS(3,-17),
           gS(-1,-11), gS(6,4), gS(24,26), gS(24,37), gS(13,34), gS(37,21), gS(14,7), gS(7,-8),
           gS(-6,-9), gS(9,7), gS(12,28), gS(19,30), gS(22,33), gS(18,31), gS(24,11), gS(5,-1),
           gS(-18,-33), gS(0,-7), gS(5,8), gS(14,22), gS(18,20), gS(12,5), gS(6,-1), gS(-6,-25),
           gS(-20,-16), gS(-25,0), gS(-6,-12), gS(7,3), gS(3,8), gS(0,-10), gS(-13,0), gS(-8,-2),
           gS(-37,-44), gS(-11,-28), gS(-10,-19), gS(-7,0), gS(0,0), gS(-5,-8), gS(-9,-19), gS(-37,-43),
};

const int  QUEEN_PSQT_BLACK[64] = {
           gS(-5,3), gS(0,19), gS(0,33), gS(4,42), gS(10,51), gS(31,47), gS(9,47), gS(15,34),
           gS(-18,-1), gS(-58,41), gS(-17,29), gS(-41,72), gS(-50,110), gS(-1,70), gS(-56,69), gS(-19,39),
           gS(-15,-15), gS(-7,-15), gS(-11,16), gS(-13,43), gS(0,60), gS(7,77), gS(25,48), gS(-7,60),
           gS(-4,-31), gS(-2,1), gS(-10,6), gS(-17,50), gS(-16,66), gS(-13,73), gS(0,59), gS(1,32),
           gS(1,-41), gS(3,-13), gS(-2,-8), gS(-9,28), gS(-6,27), gS(-12,26), gS(9,3), gS(-1,7),
           gS(0,-58), gS(11,-42), gS(5,-22), gS(4,-31), gS(4,-28), gS(0,-13), gS(15,-33), gS(4,-47),
           gS(1,-66), gS(8,-63), gS(17,-80), gS(10,-50), gS(16,-54), gS(18,-92), gS(15,-84), gS(-10,-56),
           gS(3,-70), gS(7,-82), gS(9,-88), gS(13,-69), gS(14,-74), gS(7,-89), gS(10,-83), gS(0,-73),
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
