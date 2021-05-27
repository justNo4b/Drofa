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
const int  KING_HIGH_DANGER  = gS(-15,-27);   // применять когда жив ферзь и мы не рокированы
const int  KING_MED_DANGER   = gS(-6,-16);   // мы рокированы, но пешечный щит кривой
const int  KING_LOW_DANGER   = gS(0,10);   // слабый пешечный щит
const int  KING_SAFE         = gS(2,21);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-20,-104), gS(-8,-44), gS(-2,-17), gS(0,-2), gS(3,11), gS(5,21), gS(5,28),
           gS(3,31), gS(4,33), gS(7,32), gS(13,28), gS(29,18), gS(45,28), gS(77,-4),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-7,-74), gS(1,-26), gS(4,-1), gS(7,12), gS(10,20),
           gS(13,26), gS(18,24), gS(25,16), gS(34,2),
};

const int KING_MOBILITY[9] = {
           gS(10,-2), gS(14,-5), gS(8,2), gS(5,7), gS(-1,6),
           gS(-7,4), gS(-4,4), gS(-17,1), gS(-10,-11),
};

const int ROOK_MOBILITY[15] = {
           gS(-21,-102), gS(-13,-66), gS(-9,-33), gS(-7,-12), gS(-8,6), gS(-9,20), gS(-4,23),
           gS(0,27), gS(4,31), gS(10,36), gS(12,40), gS(18,41), gS(22,41), gS(37,28),
           gS(89,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-40,-103), gS(-19,-166), gS(-7,-132), gS(-4,-77), gS(-2,-44), gS(0,-22), gS(1,-4),
           gS(3,8), gS(6,17), gS(8,23), gS(9,30), gS(11,35), gS(11,39), gS(10,44),
           gS(8,50), gS(6,54), gS(5,57), gS(4,57), gS(4,57), gS(9,53), gS(12,48),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
};

const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
const int COUNT_TO_POWER[8] = {0, 0, 50, 75, 80, 88, 95, 100};
const int SAFE_SHIELD_SAFETY = -50;

/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 * 'vBishopEG': 288, 'vBishopOP': 336, 'vKnightEG': 310, 'vKnightOP': 316, 'vPawnEG': 112, 'vQueenEG': 1005, 'vQueenOP': 1100, 'vRookEG': 500, 'vRookOP': 470}
    Finished game 201 (Drofa_dev vs Drofa_1.2.9): 1-0 {White wins by adjudication}
    Score of Drofa_dev vs Drofa_1.2.9: 67 - 63 - 70  [0.510] 200
    {'vBishopEG': 357, 'vBishopOP': 336, 'vKnightEG': 328, 'vKnightOP': 304, 'vPawnEG': 86, 'vQueenEG': 995, 'vQueenOP': 1190, 'vRookEG': 565, 'vRookOP': 465}
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(75,127),
        [ROOK] = gS(421,745),
        [KNIGHT] = gS(283,410),
        [BISHOP] = gS(305,442),
        [QUEEN] = gS(945,1358),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(15,77);

const int PAWN_SUPPORTED = gS(8,2);

const int DOUBLED_PAWN_PENALTY = gS(-6,-26);

const int ISOLATED_PAWN_PENALTY = gS(-4,-8);

const int PAWN_BLOCKED = gS(2,26);

const int PASSER_BLOCKED = gS(2,46);

const int PAWN_DISTORTION = gS(-1,-1);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(11,6);

const int MINOR_BEHIND_PAWN = gS(7,22);

const int MINOR_BEHIND_PASSER = gS(9,13);

const int KING_AHEAD_PASSER = gS(-14,7);

const int KING_EQUAL_PASSER = gS(6,7);

const int KING_BEHIND_PASSER = gS(6,-4);

const int KING_OPEN_FILE = gS(-48,-2);

const int KING_OWN_SEMI_FILE = gS(-29,17);

const int KING_ENEMY_SEMI_LINE = gS(-16,8);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(4,32), gS(-4,25), gS(-8,10),
           gS(-5,0), gS(-4,-4), gS(9,-9), gS(-1,-19),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(55,62), gS(-15,14), gS(-5,-2),
           gS(-5,-15), gS(-3,-26), gS(3,-37), gS(12,-44),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS [2] = {  gS(22,10), gS(22,11), };

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS [2] = {  gS(6,2), gS(16,9), };


/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn 
 * and OUR pieces attacking enemy pawns - [PAWN] 
 */
const int HANGING_PIECE[5] = {
           gS(-3,13), gS(-26,-16), gS(-26,-51), gS(-31,-74), gS(-18,-12),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-12,-28), gS(-16,-15), gS(-8,20),
           gS(15,55), gS(25,118), gS(75,130), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,16), gS(6,16), gS(0,4), gS(0,0),
           gS(-3,-1), gS(-15,2), gS(-9,17), gS(9,12),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-95), gS(-2,-47), gS(-1,-33), gS(0,-17), gS(-2,-32), gS(-3,-27), gS(-4,-25), gS(-12,-87),
           gS(-13,-37), gS(0,33), gS(5,23), gS(0,5), gS(0,4), gS(3,23), gS(0,43), gS(-12,-36),
           gS(-2,-3), gS(27,37), gS(33,34), gS(12,21), gS(12,19), gS(35,34), gS(27,39), gS(-7,-8),
           gS(-9,-9), gS(22,19), gS(30,30), gS(0,34), gS(8,29), gS(33,26), gS(29,18), gS(-36,-10),
           gS(-22,-33), gS(22,1), gS(26,19), gS(-14,36), gS(2,31), gS(11,19), gS(24,2), gS(-53,-22),
           gS(-52,-15), gS(-10,0), gS(-7,12), gS(-27,28), gS(-15,24), gS(-16,15), gS(-9,1), gS(-51,-13),
           gS(0,-13), gS(-3,3), gS(-23,14), gS(-62,24), gS(-44,18), gS(-44,19), gS(-2,0), gS(-4,-22),
           gS(-28,-55), gS(0,-28), gS(-30,-6), gS(-34,-25), gS(-24,-38), gS(-56,-7), gS(-6,-28), gS(-18,-74),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,72), gS(49,97), gS(68,92), gS(74,78), gS(81,77), gS(84,81), gS(-8,101), gS(-23,83),
           gS(-3,47), gS(27,52), gS(40,43), gS(44,23), gS(55,23), gS(94,30), gS(61,38), gS(9,34),
           gS(-5,20), gS(5,24), gS(6,21), gS(20,6), gS(28,10), gS(29,14), gS(10,21), gS(-3,9),
           gS(-14,4), gS(-7,12), gS(-4,4), gS(4,3), gS(8,4), gS(4,6), gS(0,6), gS(-14,-4),
           gS(-19,-1), gS(-15,2), gS(-13,6), gS(-11,7), gS(-5,10), gS(-3,10), gS(0,-2), gS(-14,-11),
           gS(-16,1), gS(-11,4), gS(-16,16), gS(-9,16), gS(-13,22), gS(6,18), gS(7,1), gS(-15,-16),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,27), gS(23,33), gS(0,42), gS(8,38), gS(12,38), gS(25,37), gS(41,31), gS(52,27),
           gS(-2,23), gS(-6,27), gS(15,23), gS(25,24), gS(23,25), gS(41,10), gS(7,22), gS(14,18),
           gS(-8,16), gS(32,2), gS(20,13), gS(42,2), gS(53,-2), gS(48,5), gS(70,-10), gS(13,9),
           gS(-7,8), gS(9,7), gS(17,9), gS(39,2), gS(31,2), gS(28,1), gS(28,-1), gS(12,3),
           gS(-19,0), gS(-11,10), gS(-13,12), gS(-2,5), gS(-4,4), gS(-8,8), gS(5,5), gS(-10,-3),
           gS(-24,-15), gS(-7,-3), gS(-15,-4), gS(-6,-10), gS(-6,-9), gS(-9,-7), gS(9,-7), gS(-14,-17),
           gS(-53,-9), gS(-16,-16), gS(-9,-11), gS(-3,-16), gS(-2,-16), gS(-1,-20), gS(-1,-21), gS(-53,-10),
           gS(-17,-11), gS(-10,-10), gS(-6,-7), gS(0,-15), gS(0,-14), gS(-2,-4), gS(-1,-13), gS(-11,-21),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,5), gS(-43,8), gS(-77,17), gS(-80,23), gS(-67,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-48,1), gS(-11,4), gS(-7,8), gS(-14,16), gS(-6,14), gS(0,7), gS(-26,5), gS(-24,-4),
           gS(3,1), gS(3,7), gS(23,4), gS(8,9), gS(24,7), gS(10,9), gS(24,2), gS(15,-1),
           gS(-10,0), gS(18,6), gS(4,6), gS(22,13), gS(19,13), gS(15,3), gS(19,2), gS(-6,0),
           gS(0,-12), gS(6,-1), gS(11,7), gS(18,7), gS(25,6), gS(3,6), gS(13,0), gS(3,-13),
           gS(0,-17), gS(11,-7), gS(7,-4), gS(14,2), gS(9,3), gS(12,-5), gS(12,-8), gS(14,-11),
           gS(8,-30), gS(8,-32), gS(11,-15), gS(4,-2), gS(8,-3), gS(9,-15), gS(18,-26), gS(5,-41),
           gS(6,-37), gS(2,-15), gS(1,-2), gS(1,-6), gS(1,-2), gS(0,-7), gS(-1,-10), gS(3,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-143,-68), gS(-35,-20), gS(-65,11), gS(-14,0), gS(0,1), gS(-53,13), gS(-28,-12), gS(-88,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-4), gS(49,17), gS(37,16), gS(52,-12), gS(-21,-3), gS(-7,-21),
           gS(-19,-18), gS(12,-3), gS(32,29), gS(33,28), gS(58,18), gS(45,23), gS(27,-11), gS(3,-17),
           gS(0,-11), gS(9,4), gS(27,26), gS(26,38), gS(19,32), gS(41,19), gS(17,4), gS(9,-8),
           gS(-4,-9), gS(10,8), gS(15,30), gS(20,32), gS(23,34), gS(20,31), gS(25,11), gS(6,0),
           gS(-19,-34), gS(1,-5), gS(5,10), gS(13,25), gS(17,23), gS(13,8), gS(7,0), gS(-5,-26),
           gS(-21,-16), gS(-27,-1), gS(-7,-10), gS(6,7), gS(1,9), gS(0,-9), gS(-16,-3), gS(-10,-4),
           gS(-40,-46), gS(-13,-28), gS(-11,-18), gS(-8,3), gS(0,2), gS(-7,-11), gS(-10,-20), gS(-40,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-4,3), gS(0,19), gS(0,34), gS(3,45), gS(9,54), gS(31,48), gS(8,49), gS(14,36),
           gS(-19,-2), gS(-57,39), gS(-18,28), gS(-42,72), gS(-47,110), gS(0,71), gS(-55,72), gS(-18,39),
           gS(-15,-16), gS(-8,-17), gS(-12,15), gS(-12,42), gS(0,60), gS(9,78), gS(27,47), gS(0,61),
           gS(-5,-32), gS(-3,0), gS(-9,4), gS(-17,48), gS(-15,65), gS(-10,73), gS(2,59), gS(6,31),
           gS(0,-41), gS(3,-13), gS(-3,-9), gS(-10,28), gS(-6,27), gS(-8,25), gS(11,3), gS(2,8),
           gS(0,-58), gS(11,-42), gS(4,-21), gS(3,-30), gS(3,-28), gS(1,-11), gS(16,-32), gS(5,-47),
           gS(0,-66), gS(7,-62), gS(16,-77), gS(9,-48), gS(13,-51), gS(17,-90), gS(14,-84), gS(-10,-57),
           gS(1,-69), gS(5,-81), gS(7,-87), gS(10,-66), gS(11,-73), gS(6,-88), gS(9,-84), gS(-1,-75),
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
