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
    int EnemyKingSquare[2];
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
};

/**
 * @brief Various constants used for Scaling evaluation
 * The basic idea is that in some endgames it is hard to convert even a big advantage
 * We want our engine to avoid such endgames, so we scale down the evaluation if the endgame
 * is hard to win (OCB and its derivates, no pany pawns, etc)
 *
  * @{
  */
  const int EG_SCALE_NORMAL = 64;
  const int EG_SCALE_MAXIMUM = 128;
  const int EG_SCALE_MINIMAL = 32;
  const int EG_SCALE_PAWN    = 8;

  const int BOTH_SCALE_NORMAL = 4;
  const int BOTH_SCALE_OCB = 2;
  const int BOTH_SCALE_ROOK_OCB = 3;
  const int BOTH_SCALE_KNIGHT_OCB = 3;
  /**@}*/

/**
 * @brief Various constants used for KingSafety calculation
  * @{
  */
  const int START_ATTACK_VALUE = -50;
  const int ATTACK_TEMPO = 35;

  const int UNCONTESTED_KING_ATTACK [6] = {
      -70, -20, 0, 100, 150, 200
  };

  const int PIECE_ATTACK_POWER[6] = {
        0, 24, 50, 26, 62, 0
  };

  const int PIECE_CHECK_POWER[6] = {
        0, 22, 16, 92, 34, 0
  };

  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int COUNT_TO_POWER_DIVISOR = 128;
  /**@}*/

const int TEMPO = 10;

const int KING_HIGH_DANGER = gS(-51,-58);

const int KING_MED_DANGER = gS(-23,-38);

const int KING_LOW_DANGER = gS(67,-70);

const int BISHOP_PAIR_BONUS = gS(32,51);

const int PAWN_SUPPORTED = gS(12,8);

const int DOUBLED_PAWN_PENALTY = gS(-7,-22);

const int ISOLATED_PAWN_PENALTY = gS(-8,-4);

const int PAWN_BLOCKED = gS(3,15);

const int PASSER_BLOCKED = gS(6,-3);

const int BISHOP_RAMMED_PENALTY = gS(-3,-5);

const int BISHOP_CENTER_CONTROL = gS(9,7);

const int MINOR_BEHIND_PAWN = gS(4,10);

const int MINOR_BEHIND_PASSER = gS(10,0);

const int MINOR_BLOCK_OWN_PAWN = gS(-7,0);

const int MINOR_BLOCK_OWN_PASSER = gS(0,-5);

const int KING_AHEAD_PASSER = gS(-15,14);

const int KING_EQUAL_PASSER = gS(12,7);

const int KING_BEHIND_PASSER = gS(0,-5);

const int KING_OPEN_FILE = gS(-57,8);

const int KING_OWN_SEMI_FILE = gS(-24,15);

const int KING_ENEMY_SEMI_LINE = gS(-18,8);

const int KING_ATTACK_PAWN = gS(20,49);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(5,0), gS(6,5), gS(8,12),
           gS(19,25), gS(42,57), gS(7,41),
};

const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-9,-8), gS(-5,7), gS(0,44),
           gS(16,54), gS(26,78), gS(54,88),
};

const int PASSED_PAWN_FILES[8] = {
           gS(0,23), gS(-1,14), gS(1,4), gS(2,-8),
           gS(-4,-6), gS(-4,3), gS(-4,14), gS(1,17),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-1,8), gS(6,23), gS(62,24),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(13,27), gS(34,56), gS(45,112),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-5,-24), gS(17,2), gS(1,-19), gS(-7,-7),
           gS(-10,-9), gS(-19,-9), gS(-18,-12), gS(-16,-10),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(4,36), gS(-9,28), gS(-7,10),
           gS(-3,1), gS(0,0), gS(14,-1), gS(13,-1),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-46,-1), gS(8,-17), gS(0,1),
           gS(2,12), gS(-2,22), gS(1,23), gS(-8,22),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-6,-8), gS(1,3), gS(7,8), gS(13,14),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(28,-3), gS(32,0),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(15,-3), gS(22,9),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,20), gS(-66,-41), gS(-49,-47), gS(-53,-66), gS(-50,-27),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(17,33), gS(16,34), gS(15,31),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-2,20), gS(65,21), gS(42,42),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(63,32), gS(41,17), gS(43,31),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-3,2), gS(4,9), gS(4,12), gS(-2,10),
           gS(-8,5), gS(-18,-1), gS(-6,13), gS(0,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(9,0), gS(12,23), gS(19,5), gS(23,13),
           gS(10,0), gS(-17,-2), gS(-4,10), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-6,-67), gS(2,-28), gS(10,-15), gS(14,-4), gS(17,5), gS(18,9), gS(17,12),
           gS(17,12), gS(16,13), gS(16,12), gS(19,10), gS(33,1), gS(34,13), gS(60,-10),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-2,-95), gS(9,-43), gS(16,-11), gS(21,1), gS(26,11),
           gS(30,18), gS(34,16), gS(37,10), gS(43,1),
};

const int KING_MOBILITY[9] = {
           gS(6,-23), gS(2,-3), gS(0,4), gS(-1,9), gS(-3,8),
           gS(-6,6), gS(2,4), gS(2,0), gS(14,-17),
};

const int ROOK_MOBILITY[15] = {
           gS(-25,-87), gS(-11,-49), gS(-4,-32), gS(2,-17), gS(4,-5), gS(1,7), gS(7,8),
           gS(8,9), gS(12,14), gS(14,17), gS(16,22), gS(18,24), gS(23,22), gS(29,21),
           gS(52,12),
};

const int QUEEN_MOBILITY[28] = {
           gS(-3,-114), gS(-2,-164), gS(-3,-112), gS(-3,-72), gS(-4,-53), gS(-3,-36), gS(-3,-19),
           gS(-2,-6), gS(-2,7), gS(-1,19), gS(-1,26), gS(-1,33), gS(0,37), gS(0,40),
           gS(0,43), gS(0,45), gS(0,48), gS(5,43), gS(7,41), gS(11,39), gS(15,33),
           gS(17,28), gS(10,32), gS(19,21), gS(4,19), gS(12,15), gS(17,20), gS(15,16),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(-2,-95), gS(0,-37), gS(0,-20), gS(0,-6), gS(0,-21), gS(0,-21), gS(0,-13), gS(-1,-94),
           gS(-3,-21), gS(0,34), gS(5,33), gS(1,32), gS(1,16), gS(0,34), gS(0,31), gS(-4,-22),
           gS(0,-2), gS(27,24), gS(23,35), gS(4,38), gS(7,39), gS(28,35), gS(16,28), gS(0,-3),
           gS(-3,-13), gS(11,13), gS(6,25), gS(-7,35), gS(-6,33), gS(11,23), gS(14,14), gS(-37,-14),
           gS(-16,-24), gS(6,0), gS(-4,16), gS(-21,26), gS(-16,24), gS(-21,18), gS(-1,2), gS(-50,-17),
           gS(-24,-25), gS(10,-6), gS(-14,9), gS(-34,20), gS(-28,19), gS(-24,10), gS(-1,-4), gS(-15,-23),
           gS(0,-29), gS(-5,-6), gS(-5,1), gS(-24,11), gS(-26,12), gS(-4,3), gS(-7,-4), gS(3,-31),
           gS(-18,-55), gS(2,-38), gS(-19,-15), gS(-34,-14), gS(8,-45), gS(-12,-26), gS(5,-36), gS(0,-67),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(53,69), gS(40,79), gS(41,67), gS(66,59), gS(66,63), gS(54,64), gS(0,85), gS(1,76),
           gS(2,34), gS(11,34), gS(26,21), gS(48,10), gS(49,10), gS(76,14), gS(31,28), gS(28,19),
           gS(-9,14), gS(0,14), gS(14,3), gS(19,7), gS(28,4), gS(28,1), gS(25,3), gS(0,6),
           gS(-22,1), gS(-18,7), gS(-7,0), gS(12,1), gS(11,0), gS(12,-2), gS(0,-2), gS(-16,-6),
           gS(-25,-4), gS(-19,-2), gS(-4,-5), gS(-3,2), gS(3,1), gS(2,-3), gS(-1,-9), gS(-20,-11),
           gS(-30,5), gS(-14,4), gS(-14,9), gS(0,9), gS(0,12), gS(5,6), gS(-2,0), gS(-19,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-3,10), gS(-3,8), gS(0,4), gS(0,3), gS(0,2), gS(-1,2), gS(0,2), gS(0,4),
           gS(-2,9), gS(-4,12), gS(-6,7), gS(-1,1), gS(0,3), gS(-1,4), gS(-2,16), gS(-2,11),
           gS(-3,-5), gS(-4,0), gS(-7,-8), gS(0,-17), gS(0,-12), gS(-5,-4), gS(-7,1), gS(-2,0),
           gS(0,-14), gS(-3,-9), gS(1,-13), gS(0,-24), gS(0,-18), gS(-1,-5), gS(-5,0), gS(0,1),
           gS(0,-24), gS(-4,-6), gS(-4,-11), gS(-4,0), gS(-6,-8), gS(-7,2), gS(1,-5), gS(0,-22),
           gS(0,-35), gS(-2,-18), gS(-5,-3), gS(-9,0), gS(-7,1), gS(0,-1), gS(1,-5), gS(1,-41),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-5,4), gS(-4,4), gS(-1,2), gS(0,0), gS(0,0), gS(-2,0), gS(0,0), gS(-2,0),
           gS(-3,8), gS(-5,10), gS(-6,9), gS(-1,0), gS(0,2), gS(-2,1), gS(-1,14), gS(-3,7),
           gS(0,-4), gS(0,0), gS(-2,-3), gS(0,-16), gS(0,-14), gS(-1,-7), gS(-9,-4), gS(-1,0),
           gS(0,5), gS(0,-1), gS(3,-7), gS(0,-13), gS(0,-9), gS(-1,-8), gS(0,0), gS(0,-4),
           gS(3,0), gS(0,4), gS(0,0), gS(-4,2), gS(-2,4), gS(-3,7), gS(3,-5), gS(8,0),
           gS(5,19), gS(0,11), gS(-1,22), gS(-2,16), gS(-4,15), gS(4,17), gS(4,14), gS(8,8),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[32] = {
           gS(16,15), gS(9,18), gS(5,20), gS(4,15),
           gS(5,10), gS(-1,10), gS(10,9), gS(12,8),
           gS(0,11), gS(14,5), gS(12,7), gS(15,3),
           gS(-2,6), gS(1,4), gS(12,3), gS(11,1),
           gS(-19,4), gS(-8,5), gS(-5,2), gS(-2,1),
           gS(-18,-7), gS(-5,-6), gS(-6,-6), gS(-6,-5),
           gS(-33,-11), gS(-2,-17), gS(0,-16), gS(1,-14),
           gS(-8,-17), gS(-7,-12), gS(6,-17), gS(7,-22),
};

const int BISHOP_PSQT_BLACK[32] = {
           gS(-12,-6), gS(-5,-5), gS(-7,-1), gS(-3,1),
           gS(-17,-7), gS(-13,-3), gS(-9,0), gS(-3,5),
           gS(0,0), gS(2,3), gS(5,5), gS(14,4),
           gS(-7,-3), gS(4,5), gS(5,5), gS(15,11),
           gS(1,-6), gS(0,4), gS(2,7), gS(16,8),
           gS(0,-9), gS(13,-3), gS(10,-1), gS(2,13),
           gS(3,-7), gS(12,-17), gS(10,-3), gS(6,3),
           gS(-11,-13), gS(0,-2), gS(-11,6), gS(0,1),
};

const int KNIGHT_PSQT_BLACK[32] = {
           gS(-46,-29), gS(-2,-2), gS(-9,-5), gS(-2,-4),
           gS(-19,-9), gS(-7,-2), gS(2,-2), gS(4,5),
           gS(-2,-4), gS(7,0), gS(14,11), gS(25,6),
           gS(10,-3), gS(1,-4), gS(17,9), gS(15,13),
           gS(0,2), gS(13,2), gS(16,13), gS(18,16),
           gS(-9,-9), gS(13,-8), gS(8,4), gS(16,15),
           gS(-8,-9), gS(-7,2), gS(1,-5), gS(3,7),
           gS(-10,-7), gS(-17,-7), gS(-9,-4), gS(-1,3),
};

const int QUEEN_PSQT_BLACK[32] = {
           gS(-4,0), gS(1,1), gS(0,0), gS(1,2),
           gS(-3,1), gS(-13,1), gS(-7,3), gS(-2,4),
           gS(-7,-1), gS(3,3), gS(5,8), gS(8,11),
           gS(0,0), gS(-4,7), gS(0,9), gS(1,18),
           gS(-3,-2), gS(3,4), gS(-2,8), gS(2,23),
           gS(-5,-3), gS(7,0), gS(4,3), gS(2,7),
           gS(-6,-5), gS(6,-13), gS(14,-24), gS(10,-11),
           gS(-7,-8), gS(-6,-9), gS(-7,-17), gS(0,-26),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(83,113), gS(498,687), gS(356,380), gS(375,398), gS(993,1278),
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
   inline int probePawnStructure(const Board &, Color, evalBits *);

  /**@}*/

evalBits Setupbits(const Board &);

/**
 * @brief This function analyses king shield safety.
 * it returns simple overall score gS() and
 * adjust base safety value for some types of shields
 */
inline int kingShieldSafety(const Board &, Color, evalBits *);

/**
 * @brief This function takes number of each pieceType count for each
 * side and (assuming best play) returns if the position is deadDraw
 *
 * Returns true is position is drawn, returns false if there is some play left.
 * Based on Vice function.
 *
 */
inline bool IsItDeadDraw (const Board &, Color);


/**
 * @brief Function evaluate piece-pawns interactions for given color
 * Includes:
 * 1. Blocked Pawns
 * 2. Minors shielded by pawns
 * 3. Threats by pawn push
 */
inline int PiecePawnInteraction(const Board &, Color, evalBits *);

/**
 * @brief Taper evaluation between Opening and Endgame and scale it
 * if there is some specific endgame position
 */
inline int TaperAndScale(const Board &, Color, int);

/**
 * @brief Transform danger score accumulated in other functions in
 *        a score used for an evaluation
 */
inline int kingDanger(Color, const evalBits *);

/**
 * @brief Set value for a MATERIAL_VALUES_TUNABLE array
 * which is used for optuna tuning
 */
void SetupTuning(PieceType piece, int value);

};

#endif
