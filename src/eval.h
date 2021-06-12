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
           gS(-24,-103), gS(-12,-44), gS(-5,-18), gS(-2,-2), gS(1,11), gS(2,22), gS(2,28),
           gS(1,31), gS(2,34), gS(5,32), gS(11,29), gS(27,19), gS(45,28), gS(75,-4),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-6,-70), gS(2,-24), gS(6,0), gS(9,14), gS(11,22),
           gS(14,28), gS(19,27), gS(25,19), gS(34,4),
};

const int KING_MOBILITY[9] = {
           gS(11,-2), gS(15,-7), gS(10,1), gS(6,6), gS(-1,6),
           gS(-8,3), gS(-5,3), gS(-19,1), gS(-14,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-21,-101), gS(-14,-64), gS(-9,-31), gS(-8,-11), gS(-9,6), gS(-11,19), gS(-5,23),
           gS(0,26), gS(3,31), gS(8,36), gS(10,40), gS(16,42), gS(21,41), gS(36,29),
           gS(88,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-42,-107), gS(-21,-172), gS(-9,-132), gS(-5,-76), gS(-3,-43), gS(-1,-21), gS(0,-3),
           gS(2,8), gS(4,17), gS(6,23), gS(8,30), gS(9,34), gS(10,38), gS(9,43),
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
        [PAWN] = gS(70,127),
        [ROOK] = gS(416,753),
        [KNIGHT] = gS(277,414),
        [BISHOP] = gS(305,449),
        [QUEEN] = gS(949,1366),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(15,76);

const int PAWN_SUPPORTED = gS(13,4);

const int DOUBLED_PAWN_PENALTY = gS(-5,-25);

const int ISOLATED_PAWN_PENALTY = gS(-4,-7);

const int PAWN_BLOCKED = gS(1,26);

const int PASSER_BLOCKED = gS(1,45);

const int PAWN_DISTORTION = gS(0,0);

const int PAWN_CONNECTED = gS(4,3);

const int PAWN_PUSH_THREAT = gS(5,6);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(10,7);

const int TRAPPED_ROOK = gS(-14,-13);

const int MINOR_BEHIND_PAWN = gS(7,22);

const int MINOR_BEHIND_PASSER = gS(7,13);

const int KING_AHEAD_PASSER = gS(-18,10);

const int KING_EQUAL_PASSER = gS(6,8);

const int KING_BEHIND_PASSER = gS(7,-5);

const int KING_OPEN_FILE = gS(-50,-1);

const int KING_OWN_SEMI_FILE = gS(-31,18);

const int KING_ENEMY_SEMI_LINE = gS(-17,8);

const int KING_ATTACK_PAWN = gS(22,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,35), gS(-3,26), gS(-6,11),
           gS(-4,0), gS(-3,-4), gS(10,-9), gS(0,-18),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(44,44), gS(-14,18), gS(-3,-1),
           gS(-3,-13), gS(-1,-24), gS(5,-35), gS(14,-41),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(23,10), gS(22,12),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(6,3), gS(14,9),
};



/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn 
 * and OUR pieces attacking enemy pawns - [PAWN] 
 */
const int HANGING_PIECE[5] = {
           gS(-2,11), gS(-28,-15), gS(-31,-50), gS(-36,-77), gS(-22,-13),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-28), gS(-15,-14), gS(-8,21),
           gS(16,55), gS(25,119), gS(73,133), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,17), gS(6,16), gS(0,3), gS(-1,0),
           gS(-4,-2), gS(-15,1), gS(-8,16), gS(9,12),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-97), gS(0,-47), gS(0,-33), gS(0,-16), gS(0,-32), gS(0,-27), gS(0,-25), gS(-12,-89),
           gS(-13,-37), gS(0,29), gS(5,22), gS(0,5), gS(0,2), gS(3,18), gS(0,37), gS(-12,-37),
           gS(-2,-5), gS(27,30), gS(33,29), gS(12,19), gS(12,15), gS(35,26), gS(26,29), gS(-7,-12),
           gS(-9,-10), gS(22,12), gS(30,27), gS(0,32), gS(7,26), gS(32,23), gS(28,11), gS(-38,-13),
           gS(-22,-33), gS(22,0), gS(26,19), gS(-14,36), gS(0,32), gS(7,20), gS(24,2), gS(-53,-23),
           gS(-52,-13), gS(-9,2), gS(-7,14), gS(-28,31), gS(-16,27), gS(-17,18), gS(-9,3), gS(-48,-11),
           gS(0,-9), gS(-2,6), gS(-23,16), gS(-61,26), gS(-44,21), gS(-43,23), gS(0,4), gS(0,-18),
           gS(-27,-53), gS(1,-24), gS(-27,-3), gS(-33,-23), gS(-24,-35), gS(-48,-4), gS(-4,-24), gS(-15,-69),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,72), gS(45,94), gS(64,90), gS(71,76), gS(79,75), gS(83,80), gS(-7,100), gS(-19,84),
           gS(-2,46), gS(22,49), gS(36,41), gS(41,20), gS(50,20), gS(88,29), gS(54,37), gS(10,34),
           gS(-2,19), gS(3,21), gS(4,18), gS(18,3), gS(27,7), gS(26,12), gS(9,19), gS(-1,9),
           gS(-12,3), gS(-8,9), gS(-4,1), gS(2,1), gS(7,2), gS(3,3), gS(0,3), gS(-11,-4),
           gS(-17,-2), gS(-17,0), gS(-14,4), gS(-13,5), gS(-6,8), gS(-6,7), gS(-1,-4), gS(-13,-12),
           gS(-15,0), gS(-12,3), gS(-17,14), gS(-8,14), gS(-14,20), gS(6,16), gS(4,0), gS(-13,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,27), gS(23,33), gS(0,42), gS(8,38), gS(13,38), gS(25,37), gS(41,31), gS(52,27),
           gS(-1,23), gS(-3,27), gS(18,23), gS(30,23), gS(27,24), gS(44,9), gS(10,22), gS(16,18),
           gS(-8,16), gS(31,2), gS(21,13), gS(43,2), gS(53,-3), gS(51,4), gS(70,-10), gS(16,8),
           gS(-8,8), gS(9,7), gS(17,9), gS(38,2), gS(32,3), gS(29,2), gS(29,0), gS(13,2),
           gS(-21,0), gS(-11,11), gS(-13,12), gS(-3,5), gS(-4,5), gS(-6,8), gS(6,5), gS(-9,-3),
           gS(-27,-15), gS(-8,-3), gS(-16,-4), gS(-8,-9), gS(-7,-8), gS(-9,-6), gS(9,-6), gS(-15,-17),
           gS(-55,-9), gS(-19,-16), gS(-12,-10), gS(-6,-15), gS(-5,-15), gS(-3,-19), gS(-3,-20), gS(-52,-8),
           gS(-17,-12), gS(-10,-11), gS(-8,-8), gS(-2,-15), gS(-2,-15), gS(-5,-4), gS(-2,-12), gS(-9,-21),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,5), gS(-43,8), gS(-79,17), gS(-81,23), gS(-69,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-49,0), gS(-12,4), gS(-8,7), gS(-14,15), gS(-3,13), gS(0,7), gS(-27,5), gS(-26,-4),
           gS(1,1), gS(0,7), gS(20,3), gS(7,8), gS(23,7), gS(7,9), gS(20,2), gS(14,0),
           gS(-11,0), gS(19,6), gS(4,6), gS(24,12), gS(22,13), gS(16,4), gS(20,3), gS(-4,0),
           gS(0,-12), gS(5,0), gS(11,7), gS(18,7), gS(25,6), gS(3,7), gS(13,0), gS(3,-13),
           gS(-3,-17), gS(10,-7), gS(5,-4), gS(12,2), gS(7,4), gS(10,-5), gS(11,-8), gS(12,-11),
           gS(7,-30), gS(6,-32), gS(9,-15), gS(1,-2), gS(5,-3), gS(6,-15), gS(16,-27), gS(4,-42),
           gS(4,-37), gS(0,-15), gS(-1,-2), gS(-1,-6), gS(-2,-2), gS(-2,-7), gS(-3,-10), gS(1,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-65), gS(-35,-20), gS(-67,11), gS(-14,0), gS(0,1), gS(-55,13), gS(-28,-12), gS(-92,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-3), gS(50,17), gS(38,17), gS(52,-11), gS(-21,-3), gS(-7,-21),
           gS(-18,-18), gS(11,-2), gS(32,30), gS(36,28), gS(61,19), gS(45,24), gS(26,-8), gS(3,-17),
           gS(2,-11), gS(12,4), gS(31,26), gS(31,38), gS(24,32), gS(45,20), gS(22,4), gS(13,-8),
           gS(-4,-9), gS(11,8), gS(17,30), gS(23,33), gS(26,35), gS(22,33), gS(28,12), gS(7,0),
           gS(-21,-33), gS(0,-5), gS(5,10), gS(13,25), gS(17,23), gS(13,8), gS(7,0), gS(-6,-25),
           gS(-22,-16), gS(-28,0), gS(-8,-10), gS(5,7), gS(0,8), gS(-2,-9), gS(-16,-3), gS(-13,-3),
           gS(-41,-46), gS(-16,-27), gS(-13,-17), gS(-9,3), gS(-2,2), gS(-9,-11), gS(-13,-20), gS(-41,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-3,3), gS(0,20), gS(0,34), gS(3,47), gS(9,56), gS(31,48), gS(8,50), gS(15,38),
           gS(-18,-4), gS(-56,35), gS(-17,26), gS(-41,72), gS(-42,107), gS(0,70), gS(-53,72), gS(-18,39),
           gS(-15,-17), gS(-8,-18), gS(-12,14), gS(-11,41), gS(1,60), gS(11,78), gS(29,45), gS(3,60),
           gS(-4,-33), gS(-2,0), gS(-7,1), gS(-14,44), gS(-12,65), gS(-5,71), gS(6,60), gS(9,31),
           gS(-1,-41), gS(3,-13), gS(-2,-9), gS(-9,28), gS(-6,27), gS(-5,25), gS(12,3), gS(4,8),
           gS(-2,-58), gS(9,-42), gS(3,-20), gS(2,-27), gS(2,-26), gS(0,-9), gS(15,-31), gS(4,-47),
           gS(-3,-66), gS(4,-61), gS(13,-76), gS(7,-46), gS(10,-49), gS(14,-87), gS(10,-83), gS(-11,-57),
           gS(-1,-67), gS(2,-78), gS(4,-84), gS(7,-65), gS(8,-71), gS(3,-87), gS(6,-84), gS(-4,-75),
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
