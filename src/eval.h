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
    U64 AttackedSquares[2];
    U64 AttackedByKing[2];
    int RammedCount;
    int KingAttackers[2];
    int KingAttackPower[2];
};

enum CastleSide {
    KingSide,
    QueenSide,
    NoCastle
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
extern U64 KING_PAWN_MASKS[2][2][8];

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
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-15,-68), gS(-4,-27), gS(2,-14), gS(8,-3), gS(11,6), gS(13,11), gS(13,15),
           gS(13,15), gS(14,18), gS(16,17), gS(17,15), gS(28,10), gS(39,26), gS(72,-2),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-6,-73), gS(6,-30), gS(13,-5), gS(18,3), gS(23,10),
           gS(27,16), gS(31,14), gS(36,9), gS(39,0),
};

const int KING_MOBILITY[9] = {
           gS(21,-2), gS(12,-3), gS(7,2), gS(3,6), gS(-2,4),
           gS(-9,2), gS(-5,2), gS(-15,-1), gS(-12,-18),
};

const int ROOK_MOBILITY[15] = {
           gS(-23,-85), gS(-10,-52), gS(-5,-31), gS(0,-16), gS(2,-3), gS(0,8), gS(6,10),
           gS(7,11), gS(11,17), gS(14,21), gS(16,26), gS(19,29), gS(24,28), gS(31,27),
           gS(60,15),
};

const int QUEEN_MOBILITY[28] = {
           gS(-18,-114), gS(-13,-183), gS(-11,-125), gS(-10,-69), gS(-8,-40), gS(-7,-23), gS(-6,-8),
           gS(-4,1), gS(-3,12), gS(-1,19), gS(-1,25), gS(0,30), gS(1,34), gS(1,38),
           gS(1,42), gS(2,46), gS(1,51), gS(2,51), gS(4,52), gS(10,50), gS(13,46),
           gS(18,45), gS(13,44), gS(24,31), gS(12,27), gS(20,23), gS(25,28), gS(23,25),
};

/**
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int UNCONTESTED_KING_ATTACK = 7;
  const int PIECE_ATTACK_POWER[6] = {0, 24, 50, 26, 62, 0};
  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int SAFE_SHIELD_SAFETY[2][9] = {
      {-50, -50, -50, 0, 0, 0, 0, 0, 0},
      {-50, -50, -50, 0, 0, 0, 0, 0, 0},
  };
  /**@}*/


/**
 * @brief Array indexed by [Phase][PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[6] = {
        [PAWN] = gS(75,110),
        [ROOK] = gS(459,709),
        [KNIGHT] = gS(324,391),
        [BISHOP] = gS(344,424),
        [QUEEN] = gS(937,1346),
        [KING] = gS(0, 0)
};

const int TEMPO = 10;
const int KING_HIGH_DANGER = gS(-45,-69);

const int KING_MED_DANGER = gS(-19,-42);

const int KING_LOW_DANGER = gS(10,-11);

const int BISHOP_PAIR_BONUS = gS(34,52);

const int PAWN_SUPPORTED = gS(13,7);

const int DOUBLED_PAWN_PENALTY = gS(-7,-19);

const int ISOLATED_PAWN_PENALTY = gS(-6,-6);

const int PAWN_BLOCKED = gS(2,15);

const int PASSER_BLOCKED = gS(12,0);

const int BISHOP_RAMMED_PENALTY = gS(-2,-7);

const int BISHOP_CENTER_CONTROL = gS(10,8);

const int MINOR_BEHIND_PAWN = gS(9,12);

const int MINOR_BEHIND_PASSER = gS(12,4);

const int KING_AHEAD_PASSER = gS(-24,16);

const int KING_EQUAL_PASSER = gS(9,7);

const int KING_BEHIND_PASSER = gS(8,-9);

const int KING_OPEN_FILE = gS(-61,6);

const int KING_OWN_SEMI_FILE = gS(-24,13);

const int KING_ENEMY_SEMI_LINE = gS(-17,9);

const int KING_ATTACK_PAWN = gS(18,47);



const int KING_PASSER_DISTANCE_FRIENDLY[9] = {
           gS(0,0), gS(1,36), gS(-5,25), gS(-1,6),
           gS(0,-2), gS(0,-1), gS(18,-5), gS(12,-6),
           gS(0,-10),
};

const int KING_PASSER_DISTANCE_ENEMY[9] = {
           gS(0,0), gS(29,15), gS(-17,19), gS(-4,0),
           gS(-5,-11), gS(1,-22), gS(0,-24), gS(13,-24),
           gS(0,-10),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(32,-5), gS(29,2),
};

/**
 * @brief Bonuses given to a player for each rook on an open file (opening/endgame)
 */
const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(15,-5), gS(20,11),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-57,-31), gS(-49,-39), gS(-50,-67), gS(-43,-18),
};

const int MINOR_ATTACKED_BY[5] = {
           gS(0,0), gS(13,33), gS(14,35), gS(14,30), gS(0,25),
};

const int ROOK_ATTACKED_BY[5] = {
           gS(0,0), gS(-5,13), gS(60,21), gS(43,31), gS(-1,-3),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(0,-6), gS(7,20), gS(7,10), gS(4,22),
           gS(-2,5), gS(-12,-29), gS(0,0), gS(0,5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(12,6), gS(12,32), gS(21,7), gS(23,26),
           gS(11,5), gS(-10,-11), gS(-2,4), gS(10,0),
};

/**
 * @brief Bonuses given to a player for having a passed pawn (opening/endgame)
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,0), gS(6,5), gS(9,11),
           gS(18,27), gS(31,53), gS(2,18),
};

const int PASSED_PAWN_RANKS[8] = {
           gS(0,0), gS(0,-8), gS(-2,8), gS(-2,47),
           gS(16,55), gS(30,71), gS(62,83), gS(0,0),
};

const int PASSED_PAWN_FILES[8] = {
           gS(9,21), gS(5,11), gS(3,3), gS(0,-8),
           gS(-4,-6), gS(-2,3), gS(0,14), gS(8,15),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-5,10), gS(1,27), gS(27,32),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(8,31), gS(11,70), gS(6,128),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-9,-21), gS(12,8), gS(-4,-14), gS(-13,-2),
           gS(-17,0), gS(-19,-4), gS(-21,-6), gS(-19,-6),
};

const int KING_PSQT_BLACK[64] = {
           gS(-11,-104), gS(0,-46), gS(0,-31), gS(0,-15), gS(0,-30), gS(0,-25), gS(0,-22), gS(-10,-94),
           gS(-12,-33), gS(0,24), gS(5,22), gS(1,14), gS(1,9), gS(4,25), gS(0,31), gS(-9,-28),
           gS(-3,-8), gS(27,23), gS(32,30), gS(12,31), gS(14,32), gS(36,32), gS(25,28), gS(-5,-6),
           gS(-10,-15), gS(19,11), gS(23,23), gS(-3,36), gS(0,32), gS(25,23), gS(23,13), gS(-40,-12),
           gS(-23,-29), gS(18,1), gS(13,16), gS(-24,32), gS(-14,28), gS(-9,18), gS(13,2), gS(-54,-18),
           gS(-44,-20), gS(3,1), gS(-13,12), gS(-39,25), gS(-34,25), gS(-31,16), gS(-9,2), gS(-41,-16),
           gS(-11,-20), gS(-7,3), gS(-3,7), gS(-24,17), gS(-25,17), gS(-5,9), gS(-9,2), gS(-16,-22),
           gS(-30,-58), gS(0,-31), gS(-19,-10), gS(-35,-9), gS(8,-37), gS(-17,-18), gS(-1,-28), gS(-13,-61),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(61,68), gS(41,81), gS(52,74), gS(67,66), gS(73,64), gS(71,65), gS(-3,87), gS(-10,80),
           gS(8,36), gS(15,37), gS(28,25), gS(47,17), gS(50,16), gS(77,16), gS(40,29), gS(29,24),
           gS(-7,16), gS(1,16), gS(8,6), gS(21,6), gS(26,6), gS(22,4), gS(12,10), gS(0,9),
           gS(-18,3), gS(-16,8), gS(0,0), gS(15,0), gS(13,0), gS(13,-1), gS(-2,0), gS(-14,-2),
           gS(-18,-4), gS(-18,0), gS(-7,-1), gS(-7,7), gS(-1,7), gS(-1,2), gS(7,-10), gS(-12,-11),
           gS(-21,5), gS(-10,5), gS(-14,14), gS(-4,17), gS(-6,20), gS(13,8), gS(11,-1), gS(-11,-6),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(19,17), gS(14,23), gS(-2,31), gS(1,23), gS(8,22), gS(21,22), gS(35,16), gS(43,14),
           gS(0,20), gS(-3,20), gS(12,16), gS(26,9), gS(21,11), gS(41,3), gS(8,12), gS(15,10),
           gS(-2,20), gS(23,7), gS(16,12), gS(28,3), gS(44,-3), gS(48,-4), gS(56,-9), gS(12,7),
           gS(-3,16), gS(2,12), gS(12,12), gS(21,3), gS(21,2), gS(23,0), gS(15,0), gS(3,3),
           gS(-15,10), gS(-13,14), gS(-7,14), gS(-3,8), gS(0,6), gS(-4,7), gS(0,4), gS(-17,3),
           gS(-17,0), gS(-10,0), gS(-6,2), gS(-5,0), gS(-3,-1), gS(0,-6), gS(3,-9), gS(-11,-11),
           gS(-22,-8), gS(-8,-8), gS(-2,-5), gS(-1,-8), gS(8,-12), gS(8,-18), gS(6,-22), gS(-38,-11),
           gS(-6,-11), gS(0,-14), gS(1,-9), gS(10,-21), gS(13,-23), gS(14,-21), gS(-6,-11), gS(-2,-26),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-42,5), gS(-43,2), gS(-81,15), gS(-82,19), gS(-69,17), gS(-91,12), gS(-16,-3), gS(-43,-4),
           gS(-42,1), gS(-20,1), gS(-11,0), gS(-15,14), gS(-10,10), gS(-6,0), gS(-33,0), gS(-27,-5),
           gS(-1,1), gS(0,3), gS(4,2), gS(13,3), gS(18,3), gS(4,3), gS(11,-3), gS(14,-4),
           gS(-10,0), gS(12,3), gS(1,4), gS(22,9), gS(17,8), gS(14,1), gS(12,-2), gS(0,-5),
           gS(7,-5), gS(0,2), gS(10,2), gS(21,5), gS(25,1), gS(6,3), gS(10,0), gS(10,-14),
           gS(0,-15), gS(18,-6), gS(10,-6), gS(12,3), gS(6,9), gS(19,-7), gS(15,-9), gS(8,-11),
           gS(16,-27), gS(13,-25), gS(20,-15), gS(8,-2), gS(17,-3), gS(17,-8), gS(23,-23), gS(10,-38),
           gS(0,-35), gS(13,-14), gS(0,-2), gS(4,-1), gS(9,-4), gS(-9,3), gS(1,-11), gS(4,-34),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-142,-63), gS(-38,-20), gS(-71,8), gS(-14,-3), gS(0,0), gS(-60,6), gS(-31,-12), gS(-97,-70),
           gS(-19,-17), gS(-8,0), gS(11,0), gS(37,11), gS(26,7), gS(44,-11), gS(-15,0), gS(-10,-20),
           gS(-12,-7), gS(13,2), gS(25,22), gS(29,20), gS(53,10), gS(39,11), gS(15,-8), gS(9,-13),
           gS(9,-3), gS(10,1), gS(18,18), gS(34,23), gS(25,20), gS(36,14), gS(14,-4), gS(23,-10),
           gS(1,0), gS(12,7), gS(16,25), gS(25,22), gS(25,27), gS(29,19), gS(25,5), gS(11,-2),
           gS(-12,-19), gS(7,-2), gS(7,9), gS(20,25), gS(23,21), gS(17,6), gS(21,-3), gS(-1,-10),
           gS(-12,-21), gS(-15,2), gS(3,-4), gS(8,9), gS(12,7), gS(10,-2), gS(0,0), gS(0,-8),
           gS(-41,-46), gS(-13,-17), gS(-7,-10), gS(-1,3), gS(6,4), gS(-5,-5), gS(-8,-9), gS(-35,-44),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-14,-4), gS(-6,13), gS(-5,30), gS(-2,44), gS(-1,52), gS(22,39), gS(-4,38), gS(-4,21),
           gS(-14,-8), gS(-33,25), gS(-14,22), gS(-39,64), gS(-43,92), gS(-12,58), gS(-32,64), gS(0,30),
           gS(-9,-18), gS(-1,-14), gS(-6,16), gS(-2,42), gS(0,57), gS(6,65), gS(12,29), gS(-2,41),
           gS(-3,-28), gS(-11,0), gS(-4,7), gS(-9,43), gS(-14,62), gS(-11,60), gS(-9,45), gS(1,17),
           gS(0,-38), gS(-5,-10), gS(-3,-1), gS(-6,32), gS(0,28), gS(-9,22), gS(5,2), gS(-6,4),
           gS(-1,-52), gS(9,-34), gS(4,-14), gS(3,-14), gS(2,-13), gS(6,-9), gS(13,-23), gS(6,-38),
           gS(3,-62), gS(8,-56), gS(19,-62), gS(11,-39), gS(14,-39), gS(20,-74), gS(22,-77), gS(0,-53),
           gS(1,-64), gS(6,-73), gS(5,-75), gS(4,-59), gS(6,-64), gS(-4,-80), gS(3,-82), gS(2,-72),
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
   inline int evaluateKING(const Board &, Color, evalBits *);

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
