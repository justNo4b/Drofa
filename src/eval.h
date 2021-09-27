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

  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int COUNT_TO_POWER_DIVISOR = 128;
  /**@}*/

const int TEMPO = 10;

const int KING_HIGH_DANGER = gS(-53,-64);

const int KING_MED_DANGER = gS(-22,-39);

const int KING_LOW_DANGER = gS(59,-48);

const int BISHOP_PAIR_BONUS = gS(30,54);

const int PAWN_SUPPORTED = gS(13,8);

const int DOUBLED_PAWN_PENALTY = gS(-7,-21);

const int ISOLATED_PAWN_PENALTY = gS(-6,-5);

const int PAWN_BLOCKED = gS(2,14);

const int PASSER_BLOCKED = gS(10,0);

const int BISHOP_RAMMED_PENALTY = gS(-3,-6);

const int BISHOP_CENTER_CONTROL = gS(10,9);

const int MINOR_BEHIND_PAWN = gS(6,9);

const int MINOR_BEHIND_PASSER = gS(13,0);

const int KING_AHEAD_PASSER = gS(-21,15);

const int KING_EQUAL_PASSER = gS(10,7);

const int KING_BEHIND_PASSER = gS(4,-7);

const int KING_OPEN_FILE = gS(-64,8);

const int KING_OWN_SEMI_FILE = gS(-25,15);

const int KING_ENEMY_SEMI_LINE = gS(-18,8);

const int KING_ATTACK_PAWN = gS(19,50);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PAWN_CONNECTED[7] = {
           gS(0,0), gS(4,0), gS(6,5), gS(8,11),
           gS(19,25), gS(39,57), gS(5,31),
};

const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(0,-7), gS(-2,8), gS(0,45),
           gS(15,55), gS(25,78), gS(57,86),
};

const int PASSED_PAWN_FILES[8] = {
           gS(6,23), gS(2,13), gS(1,3), gS(0,-9),
           gS(-5,-7), gS(-4,2), gS(-1,14), gS(6,16),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-2,9), gS(5,25), gS(49,29),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(12,28), gS(30,58), gS(27,120),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-6,-22), gS(16,3), gS(0,-18), gS(-10,-6),
           gS(-13,-7), gS(-21,-8), gS(-21,-11), gS(-20,-9),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(2,37), gS(-7,27), gS(-4,8),
           gS(-1,0), gS(0,0), gS(15,-2), gS(13,-2),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-42,-1), gS(12,-18), gS(3,0),
           gS(5,11), gS(0,21), gS(1,22), gS(-10,21),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */
const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(31,-4), gS(33,-1),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(15,-3), gS(23,12),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-65,-40), gS(-49,-43), gS(-53,-67), gS(-49,-23),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(14,33), gS(14,36), gS(15,30),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-3,19), gS(63,22), gS(42,41),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(60,26), gS(38,11), gS(42,17),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-3,2), gS(3,18), gS(5,8), gS(-1,21),
           gS(-7,5), gS(-16,-14), gS(-4,5), gS(0,4),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(11,3), gS(11,27), gS(19,6), gS(23,22),
           gS(11,3), gS(-13,-7), gS(-3,4), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-11,-66), gS(-1,-27), gS(5,-14), gS(11,-3), gS(14,6), gS(16,10), gS(16,14),
           gS(17,14), gS(18,15), gS(20,13), gS(23,11), gS(37,2), gS(36,17), gS(67,-8),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-4,-87), gS(8,-35), gS(15,-8), gS(20,2), gS(25,10),
           gS(29,16), gS(33,14), gS(38,8), gS(42,0),
};

const int KING_MOBILITY[9] = {
           gS(19,-13), gS(10,-3), gS(5,3), gS(1,7), gS(-3,6),
           gS(-10,4), gS(-6,4), gS(-11,0), gS(-1,-17),
};

const int ROOK_MOBILITY[15] = {
           gS(-24,-90), gS(-11,-52), gS(-5,-32), gS(1,-17), gS(4,-4), gS(1,7), gS(7,9),
           gS(9,10), gS(12,15), gS(15,19), gS(17,24), gS(21,26), gS(26,25), gS(32,24),
           gS(55,15),
};

const int QUEEN_MOBILITY[28] = {
           gS(-15,-114), gS(-10,-171), gS(-10,-115), gS(-9,-65), gS(-7,-43), gS(-6,-27), gS(-5,-12),
           gS(-2,-1), gS(-1,10), gS(0,20), gS(1,26), gS(2,32), gS(4,35), gS(5,38),
           gS(5,41), gS(6,45), gS(5,47), gS(8,44), gS(8,44), gS(11,41), gS(12,35),
           gS(16,32), gS(10,36), gS(20,25), gS(8,23), gS(16,19), gS(21,24), gS(19,21),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(-7,-100), gS(0,-42), gS(0,-25), gS(0,-11), gS(0,-26), gS(0,-22), gS(0,-18), gS(-6,-94),
           gS(-8,-26), gS(0,32), gS(5,30), gS(1,26), gS(1,14), gS(2,32), gS(0,31), gS(-5,-24),
           gS(0,-3), gS(27,24), gS(28,35), gS(9,38), gS(12,39), gS(33,34), gS(21,28), gS(-1,-3),
           gS(-8,-13), gS(16,13), gS(14,24), gS(-5,36), gS(-4,33), gS(17,23), gS(19,13), gS(-39,-14),
           gS(-21,-24), gS(11,0), gS(0,16), gS(-24,28), gS(-18,25), gS(-18,18), gS(1,2), gS(-55,-16),
           gS(-35,-21), gS(8,-3), gS(-17,10), gS(-40,22), gS(-37,22), gS(-29,12), gS(-5,-2), gS(-28,-18),
           gS(-2,-25), gS(-4,-4), gS(0,1), gS(-22,12), gS(-23,12), gS(-2,3), gS(-9,-2), gS(-4,-27),
           gS(-23,-53), gS(1,-35), gS(-18,-15), gS(-31,-16), gS(13,-46), gS(-12,-25), gS(0,-33), gS(-5,-65),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(57,70), gS(41,81), gS(46,70), gS(67,61), gS(69,64), gS(60,64), gS(-2,88), gS(0,78),
           gS(5,35), gS(12,37), gS(24,24), gS(48,11), gS(49,12), gS(72,17), gS(35,30), gS(28,22),
           gS(-7,16), gS(0,16), gS(9,6), gS(22,7), gS(28,7), gS(25,4), gS(19,8), gS(0,8),
           gS(-19,1), gS(-17,9), gS(-3,0), gS(14,1), gS(14,0), gS(14,-1), gS(0,0), gS(-13,-5),
           gS(-20,-5), gS(-19,0), gS(-6,-2), gS(-6,5), gS(0,5), gS(1,0), gS(5,-9), gS(-13,-11),
           gS(-23,4), gS(-11,5), gS(-15,12), gS(-3,14), gS(-4,17), gS(9,6), gS(7,-1), gS(-12,-5),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-1,2), gS(-1,1), gS(0,1), gS(0,1), gS(0,0), gS(-1,0), gS(0,0), gS(0,1),
           gS(0,3), gS(-1,3), gS(-1,2), gS(0,0), gS(0,1), gS(-1,1), gS(0,4), gS(0,4),
           gS(-1,0), gS(-1,0), gS(-2,-3), gS(0,-12), gS(-1,-9), gS(-2,-3), gS(-4,-1), gS(-1,1),
           gS(-1,-3), gS(-1,-3), gS(0,-7), gS(0,-16), gS(-1,-12), gS(-1,-6), gS(-2,0), gS(0,1),
           gS(0,-10), gS(-1,-1), gS(-2,-4), gS(-2,3), gS(-2,-1), gS(-3,3), gS(0,-4), gS(0,-9),
           gS(0,-11), gS(-1,-7), gS(-2,5), gS(-2,3), gS(-2,5), gS(0,6), gS(0,0), gS(0,-17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-1,1), gS(-1,1), gS(0,0), gS(0,0), gS(0,0), gS(-1,0), gS(0,0), gS(0,0),
           gS(0,3), gS(-1,2), gS(-1,2), gS(0,0), gS(0,0), gS(-1,0), gS(0,3), gS(-1,3),
           gS(0,-1), gS(0,0), gS(-1,-3), gS(0,-12), gS(0,-9), gS(-1,-4), gS(-4,-3), gS(-1,0),
           gS(0,1), gS(0,-2), gS(1,-6), gS(0,-12), gS(0,-10), gS(0,-7), gS(-1,-1), gS(0,-2),
           gS(1,-4), gS(0,2), gS(0,0), gS(-1,4), gS(-1,2), gS(-2,4), gS(1,-4), gS(2,-4),
           gS(2,3), gS(0,1), gS(0,12), gS(0,7), gS(-1,8), gS(1,13), gS(2,6), gS(3,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(25,14), gS(15,22), gS(0,29), gS(1,22), gS(8,21), gS(19,20), gS(30,16), gS(40,14),
           gS(6,17), gS(0,17), gS(18,13), gS(33,6), gS(26,7), gS(36,2), gS(0,14), gS(12,11),
           gS(3,18), gS(21,9), gS(18,11), gS(29,1), gS(44,-3), gS(44,-4), gS(36,-1), gS(1,12),
           gS(0,14), gS(0,14), gS(16,9), gS(21,2), gS(21,1), gS(21,0), gS(9,3), gS(0,5),
           gS(-14,9), gS(-11,14), gS(-3,10), gS(-2,7), gS(0,4), gS(-1,3), gS(-4,6), gS(-19,5),
           gS(-17,0), gS(-10,1), gS(-4,0), gS(-4,-2), gS(-2,-1), gS(0,-8), gS(2,-8), gS(-13,-10),
           gS(-21,-9), gS(-5,-10), gS(0,-7), gS(0,-10), gS(11,-15), gS(9,-22), gS(5,-23), gS(-39,-11),
           gS(-5,-11), gS(0,-13), gS(2,-9), gS(11,-21), gS(14,-24), gS(14,-21), gS(-7,-11), gS(-3,-25),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-35,0), gS(-43,-1), gS(-77,16), gS(-81,18), gS(-64,18), gS(-91,13), gS(-20,-6), gS(-31,-9),
           gS(-38,0), gS(-17,0), gS(-10,0), gS(-12,13), gS(-9,10), gS(-7,2), gS(-27,-3), gS(-22,-5),
           gS(-3,0), gS(0,3), gS(2,4), gS(19,-1), gS(22,0), gS(5,0), gS(9,0), gS(13,-5),
           gS(-9,-1), gS(11,5), gS(2,3), gS(24,7), gS(16,9), gS(16,2), gS(10,0), gS(2,-9),
           gS(7,-6), gS(-1,3), gS(11,2), gS(19,6), gS(23,3), gS(5,3), gS(10,0), gS(9,-17),
           gS(0,-15), gS(19,-7), gS(9,-6), gS(11,5), gS(5,10), gS(19,-9), gS(15,-10), gS(7,-10),
           gS(19,-24), gS(13,-26), gS(18,-12), gS(7,-1), gS(16,-1), gS(16,-7), gS(22,-23), gS(10,-34),
           gS(0,-34), gS(11,-10), gS(0,-2), gS(4,-1), gS(9,-4), gS(-10,4), gS(0,-9), gS(1,-29),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-148,-64), gS(-34,-13), gS(-75,8), gS(-13,-2), gS(0,0), gS(-66,2), gS(-29,-8), gS(-91,-66),
           gS(-24,-12), gS(-11,2), gS(6,0), gS(33,9), gS(20,6), gS(38,-10), gS(-11,0), gS(-9,-17),
           gS(-10,-2), gS(16,1), gS(20,22), gS(31,13), gS(53,4), gS(38,7), gS(15,-4), gS(12,-13),
           gS(9,-2), gS(9,1), gS(18,16), gS(34,20), gS(25,17), gS(37,11), gS(15,-6), gS(25,-12),
           gS(0,1), gS(13,6), gS(16,22), gS(25,19), gS(26,25), gS(30,15), gS(26,2), gS(11,-1),
           gS(-12,-18), gS(7,-4), gS(7,8), gS(20,22), gS(22,18), gS(17,4), gS(22,-6), gS(-2,-8),
           gS(-7,-24), gS(-10,4), gS(3,-6), gS(8,7), gS(13,6), gS(11,-4), gS(1,2), gS(0,-14),
           gS(-40,-40), gS(-13,-16), gS(-2,-7), gS(0,1), gS(8,3), gS(-5,-5), gS(-8,-8), gS(-27,-37),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-13,0), gS(0,13), gS(-4,24), gS(-3,34), gS(-6,41), gS(11,27), gS(-7,30), gS(-9,19),
           gS(-13,-1), gS(-28,25), gS(-10,25), gS(-32,60), gS(-42,82), gS(-19,49), gS(-19,57), gS(3,24),
           gS(-9,-16), gS(0,-5), gS(-3,22), gS(3,41), gS(0,54), gS(3,53), gS(9,25), gS(-5,30),
           gS(-2,-23), gS(-8,5), gS(-3,15), gS(-2,41), gS(-8,59), gS(-6,51), gS(-5,39), gS(4,6),
           gS(0,-33), gS(-5,-3), gS(-2,4), gS(-4,37), gS(2,29), gS(-6,16), gS(8,4), gS(-5,0),
           gS(-2,-44), gS(9,-29), gS(4,-10), gS(3,-8), gS(3,-6), gS(7,-10), gS(12,-14), gS(4,-33),
           gS(3,-58), gS(8,-57), gS(18,-65), gS(11,-39), gS(13,-38), gS(20,-72), gS(22,-81), gS(0,-49),
           gS(2,-63), gS(5,-73), gS(4,-76), gS(4,-62), gS(5,-65), gS(-6,-79), gS(0,-78), gS(8,-70),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(79,112), gS(484,692), gS(340,381), gS(364,408), gS(985,1305),
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
void SetupTuning(int phase, PieceType piece, int value);

};

#endif
