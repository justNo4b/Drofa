#ifndef EVAL_H
#define EVAL_H

#include "defs.h"
#include "movegen.h"
#include "bitutils.h"

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
const int  KING_SAFE         = gS(3,19);   // хороший пешечный щит

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-23,-103), gS(-11,-44), gS(-5,-17), gS(-1,-2), gS(1,11), gS(3,22), gS(3,28),
           gS(2,31), gS(3,34), gS(6,32), gS(11,29), gS(27,19), gS(45,28), gS(75,-4),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-6,-71), gS(2,-25), gS(6,0), gS(8,14), gS(11,22),
           gS(14,28), gS(18,27), gS(25,18), gS(34,4),
};

const int KING_MOBILITY[9] = {
           gS(9,-2), gS(14,-7), gS(9,1), gS(6,6), gS(0,6),
           gS(-7,3), gS(-4,3), gS(-18,1), gS(-13,-10),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-102), gS(-15,-66), gS(-10,-32), gS(-8,-12), gS(-9,6), gS(-10,20), gS(-4,24),
           gS(0,27), gS(4,31), gS(9,36), gS(11,40), gS(17,42), gS(22,42), gS(37,29),
           gS(89,0),
};

const int QUEEN_MOBILITY[28] = {
           gS(-42,-106), gS(-21,-171), gS(-8,-132), gS(-5,-76), gS(-3,-43), gS(-1,-21), gS(0,-3),
           gS(2,8), gS(5,17), gS(7,23), gS(8,30), gS(10,34), gS(10,38), gS(9,43),
           gS(8,49), gS(6,53), gS(5,56), gS(4,57), gS(4,57), gS(9,53), gS(12,48),
           gS(11,43), gS(10,40), gS(21,28), gS(12,24), gS(20,23), gS(26,29), gS(24,26),
};


/**
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
  const int COUNT_TO_POWER[8] = {0, 0, 40, 65, 80, 87, 95, 100};
  const int SAFE_SHIELD_SAFETY = -50;
  /**@}*/


/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(70,127),
        [ROOK] = gS(416,752),
        [KNIGHT] = gS(278,414),
        [BISHOP] = gS(305,448),
        [QUEEN] = gS(949,1365),
        [KING] = gS(0, 0)
};

const int TEMPO = 5;
const int BISHOP_PAIR_BONUS = gS(15,76);

const int PAWN_SUPPORTED = gS(13,4);

const int DOUBLED_PAWN_PENALTY = gS(-5,-25);

const int ISOLATED_PAWN_PENALTY = gS(-4,-7);

const int PAWN_BLOCKED = gS(1,26);

const int PASSER_BLOCKED = gS(1,45);

const int PAWN_CONNECTED = gS(4,3);

const int BISHOP_RAMMED_PENALTY = gS(-2,-9);

const int BISHOP_CENTER_CONTROL = gS(10,7);

const int MINOR_BEHIND_PAWN = gS(6,12);

const int MINOR_BEHIND_PASSER = gS(7,13);

const int KING_AHEAD_PASSER = gS(-17,10);

const int KING_EQUAL_PASSER = gS(6,8);

const int KING_BEHIND_PASSER = gS(7,-5);

const int KING_OPEN_FILE = gS(-50,-1);

const int KING_OWN_SEMI_FILE = gS(-31,18);

const int KING_ENEMY_SEMI_LINE = gS(-17,8);

const int KING_ATTACK_PAWN = gS(21,42);

const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(3,35), gS(-3,26), gS(-6,11),
           gS(-4,0), gS(-4,-4), gS(10,-9), gS(0,-19),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(45,43), gS(-14,18), gS(-3,-1),
           gS(-3,-13), gS(-2,-24), gS(5,-35), gS(14,-41),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(22,10), gS(23,12),
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
           gS(-2,11), gS(-27,-15), gS(-31,-50), gS(-35,-77), gS(-21,-13),
};
/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(-11,-28), gS(-15,-14), gS(-8,21),
           gS(16,55), gS(25,119), gS(73,132), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(10,17), gS(6,16), gS(0,3), gS(-1,0),
           gS(-4,-2), gS(-15,1), gS(-8,16), gS(9,12),
};


const int KING_PSQT_BLACK[64] = {
           gS(-13,-97), gS(0,-47), gS(0,-33), gS(0,-16), gS(0,-32), gS(0,-27), gS(-1,-25), gS(-12,-89),
           gS(-13,-37), gS(0,30), gS(5,22), gS(0,5), gS(0,2), gS(3,19), gS(0,38), gS(-12,-37),
           gS(-2,-5), gS(27,31), gS(33,29), gS(12,19), gS(12,15), gS(35,27), gS(26,30), gS(-7,-12),
           gS(-9,-10), gS(22,13), gS(30,27), gS(0,32), gS(7,26), gS(32,23), gS(28,11), gS(-38,-13),
           gS(-22,-33), gS(22,0), gS(26,19), gS(-14,36), gS(0,32), gS(8,20), gS(24,2), gS(-53,-23),
           gS(-52,-13), gS(-9,2), gS(-7,14), gS(-28,31), gS(-16,27), gS(-17,18), gS(-9,3), gS(-48,-11),
           gS(1,-9), gS(-2,6), gS(-22,16), gS(-61,26), gS(-44,21), gS(-43,22), gS(0,4), gS(-1,-18),
           gS(-26,-53), gS(0,-24), gS(-29,-3), gS(-32,-23), gS(-22,-36), gS(-54,-3), gS(-5,-24), gS(-15,-69),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,72), gS(46,94), gS(65,90), gS(72,76), gS(80,76), gS(84,81), gS(-7,100), gS(-20,84),
           gS(-2,47), gS(23,49), gS(37,41), gS(41,20), gS(51,20), gS(89,29), gS(55,37), gS(10,35),
           gS(-3,19), gS(3,21), gS(4,18), gS(19,3), gS(27,7), gS(26,12), gS(9,19), gS(-1,10),
           gS(-12,4), gS(-8,10), gS(-4,2), gS(2,1), gS(7,2), gS(3,3), gS(0,4), gS(-11,-4),
           gS(-17,-2), gS(-17,0), gS(-14,4), gS(-12,5), gS(-6,8), gS(-6,8), gS(-1,-3), gS(-12,-12),
           gS(-15,1), gS(-12,3), gS(-17,15), gS(-8,15), gS(-14,20), gS(6,17), gS(4,0), gS(-12,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(29,27), gS(23,33), gS(0,42), gS(8,38), gS(13,38), gS(25,37), gS(41,31), gS(52,27),
           gS(-1,23), gS(-4,27), gS(18,23), gS(29,23), gS(26,24), gS(44,9), gS(10,22), gS(16,18),
           gS(-8,16), gS(31,2), gS(21,13), gS(43,2), gS(53,-3), gS(51,4), gS(70,-10), gS(15,8),
           gS(-8,8), gS(9,7), gS(17,9), gS(39,2), gS(32,3), gS(29,2), gS(29,0), gS(13,2),
           gS(-20,0), gS(-11,11), gS(-13,12), gS(-3,5), gS(-4,5), gS(-6,9), gS(6,5), gS(-9,-3),
           gS(-25,-15), gS(-7,-3), gS(-15,-4), gS(-8,-9), gS(-7,-8), gS(-8,-6), gS(10,-6), gS(-15,-17),
           gS(-55,-9), gS(-18,-16), gS(-12,-10), gS(-5,-15), gS(-4,-15), gS(-2,-19), gS(-2,-20), gS(-53,-9),
           gS(-19,-12), gS(-12,-10), gS(-8,-8), gS(-1,-15), gS(-2,-15), gS(-4,-4), gS(-2,-12), gS(-12,-21),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-50,5), gS(-43,8), gS(-79,17), gS(-81,23), gS(-69,18), gS(-92,15), gS(-15,0), gS(-48,0),
           gS(-49,1), gS(-12,4), gS(-8,7), gS(-14,15), gS(-4,13), gS(0,7), gS(-27,5), gS(-26,-4),
           gS(1,1), gS(0,7), gS(21,3), gS(7,9), gS(23,7), gS(8,9), gS(21,2), gS(14,0),
           gS(-11,0), gS(18,6), gS(4,6), gS(23,13), gS(21,13), gS(15,4), gS(20,3), gS(-5,0),
           gS(0,-12), gS(5,-1), gS(11,7), gS(18,7), gS(25,6), gS(3,7), gS(13,0), gS(3,-13),
           gS(-2,-17), gS(11,-7), gS(5,-4), gS(12,2), gS(8,4), gS(10,-5), gS(11,-8), gS(13,-11),
           gS(8,-30), gS(7,-32), gS(9,-15), gS(2,-2), gS(6,-3), gS(7,-15), gS(17,-27), gS(5,-42),
           gS(4,-37), gS(1,-15), gS(0,-2), gS(0,-6), gS(-1,-2), gS(0,-7), gS(-2,-10), gS(2,-38),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-145,-65), gS(-35,-20), gS(-67,11), gS(-14,0), gS(0,1), gS(-55,13), gS(-28,-12), gS(-91,-70),
           gS(-11,-24), gS(-12,-5), gS(31,-3), gS(50,17), gS(38,17), gS(52,-11), gS(-21,-3), gS(-7,-21),
           gS(-18,-18), gS(11,-2), gS(32,30), gS(35,28), gS(60,19), gS(45,24), gS(26,-9), gS(3,-17),
           gS(2,-11), gS(11,4), gS(30,26), gS(30,38), gS(24,32), gS(45,20), gS(22,4), gS(13,-8),
           gS(-5,-9), gS(10,8), gS(16,30), gS(22,33), gS(25,35), gS(22,33), gS(27,12), gS(7,0),
           gS(-21,-33), gS(0,-5), gS(4,10), gS(13,25), gS(17,23), gS(12,8), gS(7,0), gS(-6,-25),
           gS(-22,-16), gS(-28,0), gS(-8,-10), gS(5,7), gS(0,8), gS(-1,-9), gS(-16,-3), gS(-12,-3),
           gS(-40,-46), gS(-15,-27), gS(-12,-17), gS(-9,3), gS(-1,2), gS(-9,-11), gS(-12,-20), gS(-41,-45),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-3,3), gS(0,20), gS(0,34), gS(3,47), gS(9,55), gS(31,48), gS(8,50), gS(15,38),
           gS(-18,-4), gS(-56,36), gS(-17,26), gS(-41,72), gS(-43,108), gS(0,70), gS(-54,72), gS(-19,39),
           gS(-15,-17), gS(-8,-18), gS(-12,14), gS(-11,42), gS(1,60), gS(10,78), gS(29,45), gS(2,60),
           gS(-4,-33), gS(-2,0), gS(-7,2), gS(-15,45), gS(-13,65), gS(-6,71), gS(5,60), gS(9,31),
           gS(0,-41), gS(3,-13), gS(-2,-9), gS(-9,28), gS(-5,27), gS(-5,25), gS(12,3), gS(4,8),
           gS(-1,-58), gS(9,-42), gS(3,-21), gS(2,-28), gS(2,-27), gS(0,-9), gS(16,-31), gS(5,-47),
           gS(-2,-66), gS(4,-61), gS(13,-76), gS(7,-46), gS(10,-49), gS(15,-88), gS(11,-83), gS(-10,-57),
           gS(-1,-67), gS(2,-79), gS(4,-85), gS(7,-65), gS(8,-71), gS(3,-87), gS(7,-84), gS(-3,-75),
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
 * @brief This function analyses king shield safety.
 * it returns simple overall score gS() and
 * adjust base safety value for some types of shields
 */
inline int kingShieldSafety(const Board &, Color, int, evalBits *);

/**
 * @brief This function takes number of each pieceType count for each
 * side and (assuming best play) returns if the position is deadDraw
 *
 * Returns true is position is drawn, returns false if there is some play left.
 * Based on Vice function.
 *
 */
inline bool IsItDeadDraw (int w_N, int w_B, int w_R, int w_Q,
                          int b_N, int b_B, int b_R, int b_Q);


/**
 * @brief Function evaluate piece-pawns interactions for given color
 * Includes:
 * 1. Blocked Pawns
 * 2. Minors shielded by pawns
 * 3. Threats by pawn push
 */
inline int PiecePawnInteraction(const Board &, Color, evalBits &);

/**
 * @brief Set value for a MATERIAL_VALUES_TUNABLE array
 * which is used for optuna tuning
 */
void SetupTuning(int phase, PieceType piece, int value);

};

#endif
