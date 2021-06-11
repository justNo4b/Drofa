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
const int  KING_HIGH_DANGER  = gS(-12,-27);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-8,-15);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(-1,11);   // слабый пешечный щит
const int  KING_SAFE         = gS(3,18);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-23,-104), gS(-11,-45), gS(-5,-18), gS(-2,-2), gS(1,12), gS(2,22), gS(2,29),
           gS(1,31), gS(2,34), gS(5,32), gS(11,29), gS(27,19), gS(45,29), gS(75,-3),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-6,-72), gS(2,-26), gS(5,0), gS(8,14), gS(10,22),
           gS(13,28), gS(18,27), gS(24,19), gS(34,5),
};

const int KING_MOBILITY[9] = {
           gS(12,-2), gS(16,-5), gS(11,1), gS(7,6), gS(0,5),
           gS(-7,2), gS(-7,2), gS(-23,1), gS(-18,-9),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-103), gS(-15,-67), gS(-10,-32), gS(-8,-11), gS(-9,7), gS(-11,20), gS(-5,24),
           gS(0,27), gS(4,32), gS(8,37), gS(11,41), gS(17,42), gS(22,42), gS(37,29),
           gS(90,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-42,-107), gS(-20,-173), gS(-7,-133), gS(-4,-76), gS(-2,-43), gS(-1,-21), gS(0,-3),
           gS(2,8), gS(4,17), gS(6,24), gS(8,30), gS(9,35), gS(10,39), gS(9,44),
           gS(8,50), gS(6,54), gS(5,56), gS(4,57), gS(4,57), gS(9,53), gS(13,48),
           gS(11,44), gS(10,41), gS(21,29), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
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
        [PAWN] = gS(69,126),
        [ROOK] = gS(414,753),
        [KNIGHT] = gS(277,414),
        [BISHOP] = gS(303,448),
        [QUEEN] = gS(950,1367),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(14,77);

const int PAWN_SUPPORTED = gS(13,5);

const int DOUBLED_PAWN_PENALTY = gS(-5,-24);

const int ISOLATED_PAWN_PENALTY = gS(-4,-8);

const int PAWN_BLOCKED = gS(1,26);

const int PASSER_BLOCKED = gS(1,45);

const int PAWN_DISTORTION = gS(0,0);

const int PAWN_CONNECTED = gS(4,3);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(10,7);

const int MINOR_BEHIND_PAWN = gS(7,12);

const int MINOR_BEHIND_PASSER = gS(8,13);

const int KING_AHEAD_PASSER = gS(-17,12);

const int KING_EQUAL_PASSER = gS(7,8);

const int KING_BEHIND_PASSER = gS(6,-5);

const int KING_OPEN_FILE = gS(-50,0);

const int KING_OWN_SEMI_FILE = gS(-30,18);

const int KING_ENEMY_SEMI_LINE = gS(-17,9);

const int KING_ATTACK_PAWN = gS(18,29);


const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,36), gS(-2,27), gS(-5,12),
           gS(-3,0), gS(-2,-5), gS(10,-12), gS(-1,-23),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(46,55), gS(-18,19), gS(-5,0),
           gS(-3,-14), gS(-1,-27), gS(5,-39), gS(13,-47),
           gS(0,-10),
};

const int KING_PAWN_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(0,20), gS(-25,7), gS(-5,5),
           gS(1,2), gS(3,-4), gS(2,-8), gS(2,-14),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {  gS(22,10), gS(22,11), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,3), gS(14,10), };


/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn 
 * and OUR pieces attacking enemy pawns - [PAWN] 
 */
const int HANGING_PIECE[5] = {
           gS(-3,11), gS(-26,-15), gS(-26,-50), gS(-30,-77), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-12,-26), gS(-16,-15), gS(-9,19),
           gS(17,54), gS(30,120), gS(73,132), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(11,19), gS(6,16), gS(0,2), gS(-1,-1),
           gS(-4,-3), gS(-14,0), gS(-8,16), gS(10,12),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-98), gS(0,-47), gS(0,-33), gS(0,-17), gS(0,-32), gS(0,-27), gS(-1,-25), gS(-12,-90),
           gS(-13,-37), gS(0,30), gS(5,21), gS(0,4), gS(0,2), gS(3,18), gS(0,37), gS(-12,-37),
           gS(-2,-5), gS(27,30), gS(33,27), gS(12,16), gS(12,13), gS(35,24), gS(26,29), gS(-7,-12),
           gS(-9,-7), gS(22,12), gS(30,24), gS(-1,25), gS(6,19), gS(32,19), gS(28,8), gS(-38,-12),
           gS(-22,-30), gS(23,1), gS(25,15), gS(-15,28), gS(0,23), gS(9,14), gS(27,1), gS(-52,-21),
           gS(-51,-9), gS(-7,3), gS(-7,10), gS(-29,23), gS(-16,18), gS(-15,13), gS(-3,1), gS(-45,-8),
           gS(2,-4), gS(0,9), gS(-22,16), gS(-61,24), gS(-44,18), gS(-44,22), gS(0,6), gS(-3,-11),
           gS(-24,-47), gS(0,-16), gS(-29,2), gS(-35,-18), gS(-27,-29), gS(-59,5), gS(-6,-13), gS(-17,-55),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(60,71), gS(46,93), gS(65,90), gS(72,77), gS(80,76), gS(84,80), gS(-6,100), gS(-20,85),
           gS(-6,45), gS(21,47), gS(35,40), gS(37,20), gS(45,21), gS(79,30), gS(48,36), gS(-2,36),
           gS(-3,18), gS(4,21), gS(5,20), gS(16,7), gS(23,10), gS(23,15), gS(7,21), gS(-4,11),
           gS(-11,2), gS(-6,10), gS(-3,4), gS(4,4), gS(9,5), gS(4,6), gS(2,5), gS(-10,-2),
           gS(-16,-4), gS(-15,0), gS(-12,4), gS(-10,6), gS(-4,8), gS(-3,8), gS(0,-4), gS(-11,-12),
           gS(-14,-2), gS(-11,0), gS(-15,12), gS(-6,12), gS(-12,17), gS(7,14), gS(5,-3), gS(-11,-20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,27), gS(23,33), gS(1,42), gS(9,38), gS(13,38), gS(26,37), gS(42,30), gS(53,27),
           gS(-1,24), gS(-4,27), gS(18,23), gS(29,23), gS(27,24), gS(44,9), gS(10,22), gS(16,18),
           gS(-8,17), gS(31,2), gS(21,13), gS(44,2), gS(54,-3), gS(52,4), gS(70,-10), gS(17,8),
           gS(-8,9), gS(8,7), gS(16,9), gS(38,2), gS(30,3), gS(28,1), gS(28,-1), gS(13,2),
           gS(-20,0), gS(-12,11), gS(-14,13), gS(-4,6), gS(-5,5), gS(-7,9), gS(5,5), gS(-9,-3),
           gS(-25,-14), gS(-8,-3), gS(-16,-3), gS(-9,-8), gS(-8,-7), gS(-9,-5), gS(9,-7), gS(-15,-17),
           gS(-55,-8), gS(-19,-15), gS(-12,-10), gS(-5,-14), gS(-4,-15), gS(-3,-18), gS(-3,-20), gS(-54,-9),
           gS(-17,-12), gS(-11,-11), gS(-7,-8), gS(0,-15), gS(-1,-15), gS(-3,-5), gS(-2,-13), gS(-11,-21),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-51,5), gS(-44,8), gS(-80,17), gS(-82,23), gS(-70,19), gS(-93,15), gS(-15,0), gS(-49,0),
           gS(-49,1), gS(-13,4), gS(-9,7), gS(-14,15), gS(-4,13), gS(0,7), gS(-27,6), gS(-26,-3),
           gS(2,1), gS(1,8), gS(21,3), gS(7,8), gS(24,7), gS(8,9), gS(21,2), gS(15,0),
           gS(-11,0), gS(17,6), gS(4,6), gS(23,13), gS(20,13), gS(15,4), gS(18,3), gS(-5,0),
           gS(0,-12), gS(5,-1), gS(10,7), gS(17,7), gS(24,7), gS(2,6), gS(12,0), gS(3,-13),
           gS(-1,-17), gS(10,-7), gS(5,-4), gS(12,2), gS(8,4), gS(10,-5), gS(11,-8), gS(13,-11),
           gS(8,-31), gS(7,-32), gS(10,-15), gS(2,-2), gS(6,-3), gS(8,-15), gS(16,-27), gS(5,-43),
           gS(5,-37), gS(2,-15), gS(1,-2), gS(0,-6), gS(0,-2), gS(0,-7), gS(-2,-10), gS(2,-39),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-146,-65), gS(-36,-20), gS(-68,11), gS(-14,0), gS(0,1), gS(-56,13), gS(-29,-12), gS(-91,-71),
           gS(-11,-24), gS(-12,-5), gS(31,-3), gS(50,18), gS(38,17), gS(52,-11), gS(-21,-3), gS(-7,-21),
           gS(-18,-18), gS(12,-2), gS(32,30), gS(35,28), gS(60,19), gS(45,24), gS(26,-9), gS(4,-17),
           gS(2,-11), gS(10,4), gS(29,26), gS(29,38), gS(22,32), gS(43,20), gS(20,4), gS(12,-8),
           gS(-4,-9), gS(10,9), gS(15,30), gS(21,33), gS(25,35), gS(20,32), gS(26,12), gS(7,0),
           gS(-20,-33), gS(0,-5), gS(5,10), gS(13,25), gS(17,23), gS(13,8), gS(7,0), gS(-5,-25),
           gS(-21,-16), gS(-28,0), gS(-7,-9), gS(5,7), gS(1,9), gS(-1,-9), gS(-16,-3), gS(-12,-4),
           gS(-40,-46), gS(-13,-27), gS(-11,-17), gS(-9,4), gS(0,3), gS(-8,-11), gS(-11,-20), gS(-41,-46),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-3,3), gS(0,21), gS(0,35), gS(3,47), gS(10,56), gS(31,49), gS(8,51), gS(15,38),
           gS(-18,-4), gS(-56,36), gS(-17,26), gS(-41,72), gS(-43,107), gS(0,70), gS(-54,72), gS(-18,39),
           gS(-15,-18), gS(-8,-18), gS(-12,15), gS(-11,42), gS(1,60), gS(12,79), gS(30,45), gS(4,61),
           gS(-5,-33), gS(-3,0), gS(-8,1), gS(-15,45), gS(-14,65), gS(-7,71), gS(4,59), gS(8,30),
           gS(-1,-42), gS(3,-14), gS(-3,-9), gS(-10,28), gS(-6,27), gS(-6,24), gS(11,3), gS(3,9),
           gS(-1,-58), gS(9,-42), gS(3,-21), gS(2,-28), gS(2,-27), gS(0,-10), gS(15,-32), gS(5,-47),
           gS(-1,-67), gS(5,-61), gS(14,-76), gS(7,-46), gS(11,-49), gS(15,-88), gS(11,-83), gS(-10,-57),
           gS(0,-67), gS(3,-79), gS(5,-85), gS(8,-65), gS(9,-72), gS(4,-87), gS(7,-84), gS(-2,-75),
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
