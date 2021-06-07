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
const int  KING_HIGH_DANGER  = gS(-16,-27);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-6,-16);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(-1,11);   // слабый пешечный щит
const int  KING_SAFE         = gS(2,19);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-21,-103), gS(-10,-44), gS(-4,-17), gS(0,-2), gS(2,12), gS(4,22), gS(3,28),
           gS(2,31), gS(3,34), gS(6,32), gS(11,29), gS(27,18), gS(45,28), gS(75,-4),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-6,-72), gS(2,-25), gS(5,0), gS(8,13), gS(11,21),
           gS(14,28), gS(18,26), gS(25,18), gS(34,3),
};

const int KING_MOBILITY[9] = {
           gS(9,-2), gS(13,-7), gS(8,2), gS(5,6), gS(0,6),
           gS(-7,4), gS(-3,3), gS(-17,1), gS(-12,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-22,-102), gS(-14,-66), gS(-9,-32), gS(-7,-11), gS(-8,7), gS(-10,20), gS(-4,24),
           gS(0,27), gS(4,31), gS(9,36), gS(11,40), gS(17,42), gS(21,41), gS(36,29),
           gS(89,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-41,-105), gS(-20,-170), gS(-7,-132), gS(-4,-76), gS(-2,-43), gS(0,-21), gS(1,-3),
           gS(3,8), gS(5,17), gS(7,23), gS(9,30), gS(10,35), gS(10,39), gS(9,44),
           gS(8,50), gS(6,54), gS(4,56), gS(4,57), gS(4,57), gS(9,53), gS(12,48),
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
        [PAWN] = gS(71,127),
        [ROOK] = gS(419,750),
        [KNIGHT] = gS(280,413),
        [BISHOP] = gS(305,446),
        [QUEEN] = gS(950,1365),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(15,76);

const int PAWN_SUPPORTED = gS(13,4);

const int DOUBLED_PAWN_PENALTY = gS(-5,-25);

const int ISOLATED_PAWN_PENALTY = gS(-4,-7);

const int PAWN_BLOCKED = gS(1,26);

const int PASSER_BLOCKED = gS(2,45);

const int PAWN_DISTORTION = gS(0,0);

const int PAWN_CONNECTED = gS(4,3);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(11,6);

const int MINOR_BEHIND_PAWN = gS(7,22);

const int MINOR_BEHIND_PASSER = gS(8,13);

const int KING_AHEAD_PASSER = gS(-16,9);

const int KING_EQUAL_PASSER = gS(6,7);

const int KING_BEHIND_PASSER = gS(6,-4);

const int KING_OPEN_FILE = gS(-49,-1);

const int KING_OWN_SEMI_FILE = gS(-29,18);

const int KING_ENEMY_SEMI_LINE = gS(-16,8);

const int KING_ATTACK_PAWN = gS(20,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,34), gS(-3,26), gS(-6,11),
           gS(-4,0), gS(-4,-4), gS(10,-9), gS(0,-19),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(47,43), gS(-15,18), gS(-3,-1),
           gS(-4,-13), gS(-2,-24), gS(4,-35), gS(13,-42),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {  gS(22,10), gS(22,11), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,3), gS(14,9), };


/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn 
 * and OUR pieces attacking enemy pawns - [PAWN] 
 */
const int HANGING_PIECE[5] = {
           gS(-3,11), gS(-26,-15), gS(-27,-50), gS(-31,-76), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-12,-27), gS(-15,-14), gS(-8,21),
           gS(16,55), gS(26,119), gS(74,131), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,17), gS(6,16), gS(0,3), gS(-1,0),
           gS(-4,-2), gS(-14,1), gS(-8,17), gS(9,13),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-97), gS(0,-47), gS(0,-33), gS(0,-17), gS(0,-32), gS(-1,-27), gS(-2,-25), gS(-12,-89),
           gS(-13,-37), gS(0,31), gS(5,22), gS(0,5), gS(0,3), gS(3,20), gS(0,39), gS(-12,-37),
           gS(-2,-5), gS(27,32), gS(33,30), gS(12,19), gS(12,16), gS(35,28), gS(26,31), gS(-7,-12),
           gS(-9,-10), gS(22,14), gS(30,27), gS(0,32), gS(7,26), gS(32,23), gS(28,11), gS(-38,-13),
           gS(-22,-33), gS(22,0), gS(26,19), gS(-14,36), gS(0,32), gS(9,20), gS(24,2), gS(-53,-24),
           gS(-52,-14), gS(-9,2), gS(-7,14), gS(-27,31), gS(-15,27), gS(-16,18), gS(-9,3), gS(-49,-11),
           gS(0,-10), gS(-2,5), gS(-22,16), gS(-60,26), gS(-43,21), gS(-42,22), gS(0,4), gS(-2,-18),
           gS(-27,-54), gS(0,-25), gS(-30,-4), gS(-33,-23), gS(-23,-36), gS(-54,-3), gS(-6,-24), gS(-17,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,72), gS(47,95), gS(66,90), gS(73,77), gS(80,76), gS(84,81), gS(-7,100), gS(-21,84),
           gS(-2,47), gS(24,49), gS(38,41), gS(42,21), gS(53,21), gS(91,29), gS(57,37), gS(10,35),
           gS(-3,20), gS(3,21), gS(4,19), gS(19,3), gS(28,7), gS(27,12), gS(9,19), gS(-2,9),
           gS(-12,4), gS(-8,10), gS(-4,2), gS(2,1), gS(8,2), gS(3,4), gS(0,4), gS(-12,-4),
           gS(-18,-1), gS(-17,1), gS(-14,4), gS(-13,6), gS(-6,8), gS(-6,8), gS(-1,-3), gS(-13,-11),
           gS(-16,1), gS(-13,4), gS(-17,15), gS(-8,15), gS(-14,20), gS(6,17), gS(4,0), gS(-13,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,27), gS(23,33), gS(0,42), gS(8,38), gS(12,37), gS(25,37), gS(41,31), gS(52,27),
           gS(-1,23), gS(-5,27), gS(17,23), gS(28,23), gS(25,24), gS(43,9), gS(9,22), gS(15,18),
           gS(-8,16), gS(32,2), gS(21,13), gS(43,2), gS(53,-3), gS(50,4), gS(70,-10), gS(15,8),
           gS(-7,9), gS(9,7), gS(17,9), gS(39,2), gS(31,2), gS(28,1), gS(28,-1), gS(12,2),
           gS(-20,0), gS(-11,11), gS(-13,12), gS(-3,5), gS(-4,5), gS(-8,8), gS(5,5), gS(-10,-3),
           gS(-24,-14), gS(-8,-3), gS(-15,-4), gS(-8,-9), gS(-7,-8), gS(-9,-6), gS(9,-7), gS(-15,-17),
           gS(-54,-8), gS(-17,-15), gS(-11,-10), gS(-4,-15), gS(-3,-15), gS(-2,-19), gS(-2,-20), gS(-53,-9),
           gS(-17,-11), gS(-10,-10), gS(-7,-7), gS(0,-15), gS(-1,-14), gS(-3,-4), gS(-2,-12), gS(-11,-20),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,5), gS(-43,8), gS(-79,17), gS(-81,23), gS(-69,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-49,1), gS(-12,4), gS(-8,7), gS(-14,15), gS(-5,13), gS(0,7), gS(-27,5), gS(-26,-4),
           gS(2,1), gS(2,8), gS(22,4), gS(7,9), gS(23,7), gS(8,9), gS(22,2), gS(14,-1),
           gS(-11,0), gS(18,6), gS(4,6), gS(22,13), gS(19,13), gS(14,4), gS(18,2), gS(-6,0),
           gS(0,-12), gS(5,-1), gS(11,7), gS(18,7), gS(25,6), gS(2,6), gS(13,0), gS(3,-13),
           gS(-1,-17), gS(11,-7), gS(6,-4), gS(13,2), gS(8,4), gS(11,-5), gS(12,-8), gS(14,-11),
           gS(8,-30), gS(8,-32), gS(10,-15), gS(3,-2), gS(7,-3), gS(9,-15), gS(18,-26), gS(6,-42),
           gS(5,-37), gS(2,-15), gS(1,-2), gS(0,-6), gS(0,-2), gS(0,-7), gS(-1,-10), gS(3,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-66), gS(-35,-20), gS(-67,11), gS(-14,0), gS(0,1), gS(-55,13), gS(-28,-12), gS(-90,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-3), gS(49,17), gS(37,16), gS(52,-11), gS(-21,-3), gS(-7,-21),
           gS(-19,-18), gS(12,-2), gS(32,30), gS(34,28), gS(58,18), gS(45,24), gS(26,-10), gS(3,-17),
           gS(1,-11), gS(10,4), gS(28,26), gS(28,38), gS(20,31), gS(42,19), gS(18,3), gS(10,-9),
           gS(-4,-9), gS(10,8), gS(15,30), gS(21,32), gS(24,34), gS(21,32), gS(25,11), gS(7,0),
           gS(-19,-33), gS(1,-5), gS(5,10), gS(13,25), gS(17,23), gS(13,8), gS(7,0), gS(-5,-25),
           gS(-21,-16), gS(-27,0), gS(-7,-9), gS(6,7), gS(1,9), gS(0,-8), gS(-15,-3), gS(-10,-3),
           gS(-40,-46), gS(-13,-27), gS(-11,-17), gS(-8,4), gS(0,2), gS(-7,-10), gS(-10,-20), gS(-40,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-4,3), gS(0,20), gS(0,34), gS(3,46), gS(9,54), gS(31,48), gS(8,50), gS(14,37),
           gS(-18,-3), gS(-56,37), gS(-17,27), gS(-42,72), gS(-45,108), gS(0,70), gS(-55,72), gS(-19,39),
           gS(-15,-17), gS(-8,-17), gS(-12,15), gS(-12,42), gS(0,60), gS(9,78), gS(28,45), gS(0,60),
           gS(-4,-32), gS(-2,0), gS(-8,2), gS(-16,46), gS(-15,65), gS(-9,71), gS(2,59), gS(6,30),
           gS(0,-41), gS(3,-13), gS(-2,-9), gS(-9,28), gS(-5,27), gS(-7,24), gS(11,3), gS(2,8),
           gS(0,-58), gS(10,-42), gS(4,-21), gS(3,-28), gS(3,-27), gS(0,-10), gS(16,-32), gS(5,-47),
           gS(0,-66), gS(6,-61), gS(15,-76), gS(9,-46), gS(12,-49), gS(16,-88), gS(13,-83), gS(-10,-57),
           gS(0,-67), gS(4,-79), gS(6,-85), gS(9,-64), gS(11,-71), gS(5,-87), gS(9,-84), gS(-1,-75),
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
