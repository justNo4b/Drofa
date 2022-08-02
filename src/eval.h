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
    U64 PossibleProtOutposts[2];
    U64 PossibleGenOutposts[2];
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
extern U64 TWO_PLUS_FILES[8];
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
  const int QUEEN_FACE_CHECK = 80;

  const int UNCONTESTED_KING_ATTACK [6] = {
      -70, -20, 0, 100, 150, 200
  };

  const int PIECE_ATTACK_POWER[6] = {
        0, 24, 50, 26, 47, 0
  };

  const int PIECE_CHECK_POWER[6] = {
        0, 22, 16, 92, 26, 0
  };

  const int COUNT_TO_POWER[8] = {0, 0, 51, 83, 102, 111, 122, 128};
  const int COUNT_TO_POWER_DIVISOR = 128;
  /**@}*/

const int TEMPO = 10;

const int KING_HIGH_DANGER = gS(-42,-49);

const int KING_MED_DANGER = gS(-30,-20);

const int KING_LOW_DANGER = gS(43,-71);

const int BISHOP_PAIR_BONUS = gS(21,64);

const int DOUBLED_PAWN_PENALTY = gS(-4,-23);

const int ISOLATED_PAWN_PENALTY = gS(-2,-6);

const int PAWN_BLOCKED = gS(3,16);

const int PASSER_BLOCKED = gS(9,-3);

const int PAWN_PUSH_THREAT = gS(17,15);

const int BISHOP_RAMMED_PENALTY = gS(-4,-3);

const int BISHOP_CENTER_CONTROL = gS(9,8);

const int BISHOP_POS_PROUTPOST_JUMP = gS(8,0);

const int BISHOP_POS_GENOUTPOST_JUMP = gS(3,6);

const int KNIGHT_POS_PROUTPOST_JUMP = gS(10,9);

const int KNIGHT_POS_GENOUTPOST_JUMP = gS(8,12);

const int MINOR_BEHIND_PAWN = gS(6,10);

const int MINOR_BEHIND_PASSER = gS(8,-4);

const int MINOR_BLOCK_OWN_PAWN = gS(-4,0);

const int MINOR_BLOCK_OWN_PASSER = gS(-5,6);

const int ROOK_LINE_TENSION = gS(15,-5);

const int ROOK_RAMMED_LINE = gS(-8,-12);

const int KING_AHEAD_PASSER = gS(-13,18);

const int KING_EQUAL_PASSER = gS(15,9);

const int KING_BEHIND_PASSER = gS(-1,-5);

const int KING_OPEN_FILE = gS(-78,14);

const int KING_OWN_SEMI_FILE = gS(-28,17);

const int KING_ENEMY_SEMI_LINE = gS(-20,10);

const int KING_ATTACK_PAWN = gS(5,39);

const int KING_PAWNLESS_FLANG = gS(-8,-66);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-8,-8), gS(-6,5), gS(-2,40),
           gS(29,49), gS(23,71), gS(32,98),
};

const int PASSED_PAWN_FILES[4] = {
           gS(-3,20), gS(-3,17), gS(1,6), gS(3,-4),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-4,12), gS(-8,36), gS(48,27),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(-4,36), gS(0,94), gS(53,118),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-3,-27), gS(15,-2), gS(-2,-17), gS(-6,-9),
           gS(-9,-6), gS(-19,-10), gS(-19,-15), gS(-17,-21),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(4,34), gS(-10,28), gS(-8,11),
           gS(-4,1), gS(2,-2), gS(16,-3), gS(10,-2),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-50,2), gS(23,-20), gS(0,1),
           gS(1,13), gS(-2,22), gS(-4,26), gS(-16,24),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-7,-13), gS(2,1), gS(4,12), gS(8,24),
};

const int CANDIDATE_PASSED_PAWN[7] = {
           gS(0,0), gS(-28,-6), gS(-19,6), gS(-6,19),
           gS(17,42), gS(34,66), gS(0,0),
};

const int CANDIDATE_PASSED_PAWN_FILES[4] = {
           gS(6,26), gS(3,17), gS(0,8), gS(6,-2),
};

const int BACKWARD_PAWN[7] = {
           gS(0,0), gS(-8,0), gS(3,-5), gS(-1,-8),
           gS(2,-2), gS(0,0), gS(0,0),
};

const int BACKWARD_OPEN_PAWN[7] = {
           gS(0,0), gS(-19,-16), gS(-11,-15), gS(-15,-4),
           gS(4,-1), gS(0,0), gS(0,0),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */

const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(23,4), gS(21,-2),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(9,0), gS(13,-4),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(-3,12), gS(-64,-21), gS(-53,-20), gS(-55,-49), gS(-48,-24),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(15,27), gS(16,29), gS(14,23),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-4,20), gS(56,3), gS(39,17),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(55,24), gS(33,8), gS(52,46),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-9,-7), gS(5,4), gS(0,16), gS(-7,0),
           gS(-7,0), gS(-17,-11), gS(-3,15), gS(-8,-5),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(18,-2), gS(23,22), gS(20,0), gS(24,0),
           gS(9,0), gS(-4,-6), gS(0,15), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-16,-53), gS(-8,-28), gS(0,-12), gS(4,-2), gS(8,3), gS(10,9), gS(11,9),
           gS(11,9), gS(9,12), gS(12,5), gS(17,3), gS(25,-2), gS(29,11), gS(43,-12),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-17,-86), gS(-3,-25), gS(5,-5), gS(10,1), gS(14,5),
           gS(18,9), gS(24,5), gS(28,-1), gS(32,-10),
};

const int KING_MOBILITY[9] = {
           gS(17,-21), gS(16,-2), gS(7,1), gS(0,5), gS(-6,5),
           gS(-14,4), gS(-8,2), gS(-14,0), gS(12,-16),
};

const int ROOK_MOBILITY[15] = {
           gS(-24,-66), gS(-11,-36), gS(-4,-27), gS(1,-18), gS(0,-10), gS(-5,1), gS(0,3),
           gS(2,3), gS(5,7), gS(8,11), gS(8,14), gS(10,19), gS(14,19), gS(19,19),
           gS(29,12),
};

const int QUEEN_MOBILITY[28] = {
           gS(-12,-114), gS(-7,-150), gS(-14,-110), gS(-11,-72), gS(-11,-52), gS(-9,-37), gS(-8,-20),
           gS(-7,-7), gS(-5,1), gS(-4,7), gS(-4,15), gS(-2,20), gS(-1,22), gS(-2,29),
           gS(-2,32), gS(0,36), gS(-2,45), gS(0,43), gS(6,44), gS(13,42), gS(18,41),
           gS(29,37), gS(17,37), gS(24,24), gS(6,17), gS(7,7), gS(8,5), gS(0,2),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(0,-84), gS(0,-26), gS(0,-10), gS(0,-6), gS(0,-18), gS(0,-19), gS(0,-9), gS(0,-92),
           gS(-1,-15), gS(0,29), gS(3,29), gS(0,31), gS(2,20), gS(0,38), gS(0,34), gS(-1,-10),
           gS(-1,-5), gS(27,25), gS(7,35), gS(-1,39), gS(0,43), gS(18,39), gS(12,33), gS(0,0),
           gS(-2,-20), gS(9,11), gS(-3,25), gS(-11,34), gS(-13,32), gS(-4,27), gS(4,14), gS(-31,-11),
           gS(-11,-30), gS(-3,0), gS(-9,16), gS(-22,28), gS(-25,26), gS(-28,15), gS(-11,0), gS(-44,-18),
           gS(-5,-31), gS(12,-7), gS(-15,7), gS(-30,19), gS(-23,16), gS(-25,6), gS(1,-7), gS(-12,-23),
           gS(13,-32), gS(2,-7), gS(12,-1), gS(-9,5), gS(-11,7), gS(8,-3), gS(11,-12), gS(3,-32),
           gS(-13,-60), gS(15,-43), gS(-12,-15), gS(-35,-15), gS(15,-36), gS(-14,-20), gS(10,-40), gS(0,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(47,71), gS(36,73), gS(35,75), gS(65,51), gS(54,53), gS(40,58), gS(-2,75), gS(-5,69),
           gS(3,33), gS(10,35), gS(28,13), gS(32,-3), gS(30,-6), gS(73,0), gS(31,24), gS(25,19),
           gS(-17,21), gS(1,10), gS(2,0), gS(3,-8), gS(8,-8), gS(15,-4), gS(13,0), gS(-2,0),
           gS(-30,9), gS(-19,10), gS(-13,0), gS(0,-5), gS(4,-6), gS(-1,-4), gS(-9,0), gS(-20,-6),
           gS(-32,3), gS(-19,3), gS(-11,-2), gS(-7,0), gS(2,-2), gS(-4,-5), gS(-6,-7), gS(-24,-10),
           gS(-37,11), gS(-15,10), gS(-10,8), gS(3,3), gS(0,11), gS(5,2), gS(-7,0), gS(-38,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-7,12), gS(-6,4), gS(-1,9), gS(-1,5), gS(-1,0), gS(0,0), gS(0,0), gS(-5,5),
           gS(-2,11), gS(-5,19), gS(-7,7), gS(0,6), gS(0,6), gS(3,5), gS(3,19), gS(-3,15),
           gS(-1,-14), gS(0,-16), gS(-8,-16), gS(0,-24), gS(7,-24), gS(1,-16), gS(0,-1), gS(1,-10),
           gS(5,-28), gS(2,-16), gS(7,-19), gS(10,-22), gS(5,-28), gS(5,-15), gS(3,-7), gS(10,-10),
           gS(4,-30), gS(0,-13), gS(-1,-5), gS(-3,-3), gS(-8,-8), gS(-4,8), gS(3,6), gS(6,-27),
           gS(14,-45), gS(4,-26), gS(-1,-10), gS(-13,-6), gS(-11,4), gS(1,9), gS(7,2), gS(22,-49),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-11,4), gS(-9,4), gS(-4,1), gS(-2,0), gS(-3,-2), gS(-1,-1), gS(-1,-1), gS(-7,0),
           gS(0,15), gS(-7,12), gS(-5,10), gS(-2,4), gS(-1,3), gS(2,1), gS(6,20), gS(1,18),
           gS(4,0), gS(1,-10), gS(0,-7), gS(-1,-16), gS(5,-21), gS(5,-18), gS(-5,-5), gS(7,-3),
           gS(6,12), gS(2,-7), gS(0,-7), gS(-5,-7), gS(0,-10), gS(3,-7), gS(5,-3), gS(7,-6),
           gS(4,13), gS(-2,1), gS(-6,-1), gS(-8,4), gS(-6,6), gS(-7,8), gS(-1,0), gS(5,2),
           gS(2,17), gS(-2,9), gS(-7,15), gS(-4,13), gS(-8,19), gS(-12,28), gS(4,12), gS(7,3),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(17,11), gS(8,16), gS(-3,25), gS(-6,19), gS(0,14), gS(7,17), gS(22,16), gS(34,13),
           gS(-5,10), gS(-6,17), gS(9,17), gS(27,4), gS(16,3), gS(23,3), gS(0,13), gS(15,6),
           gS(-9,10), gS(13,5), gS(8,7), gS(11,1), gS(31,-4), gS(27,-9), gS(33,-1), gS(4,0),
           gS(-6,12), gS(3,8), gS(10,10), gS(11,4), gS(14,-1), gS(22,-9), gS(17,2), gS(4,-3),
           gS(-15,7), gS(-13,9), gS(-4,7), gS(2,6), gS(3,2), gS(-6,0), gS(5,1), gS(-13,0),
           gS(-16,2), gS(-10,0), gS(-3,-1), gS(0,0), gS(5,-3), gS(6,-13), gS(24,-16), gS(0,-15),
           gS(-17,-6), gS(-8,-5), gS(2,-2), gS(3,-4), gS(11,-12), gS(5,-16), gS(20,-22), gS(-20,-11),
           gS(-6,-4), gS(-3,-6), gS(-1,0), gS(4,-5), gS(12,-13), gS(0,-6), gS(4,-14), gS(-6,-19),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-37,-2), gS(-44,-2), gS(-73,14), gS(-83,11), gS(-58,9), gS(-91,8), gS(-27,-6), gS(-34,-20),
           gS(-38,-7), gS(-25,-6), gS(-16,-2), gS(-15,6), gS(-14,1), gS(-19,-3), gS(-35,-8), gS(-34,-14),
           gS(-5,6), gS(-2,0), gS(-9,0), gS(8,-3), gS(3,-1), gS(20,-1), gS(3,-2), gS(13,0),
           gS(-11,2), gS(15,7), gS(3,6), gS(15,18), gS(20,7), gS(19,4), gS(21,0), gS(-8,-3),
           gS(-5,-4), gS(-9,6), gS(3,8), gS(19,7), gS(19,4), gS(7,3), gS(3,2), gS(12,-17),
           gS(-3,-12), gS(15,0), gS(8,0), gS(8,8), gS(10,12), gS(15,0), gS(19,-4), gS(13,-12),
           gS(6,-21), gS(6,-24), gS(14,-13), gS(1,2), gS(10,2), gS(13,-3), gS(21,-17), gS(8,-28),
           gS(-3,-30), gS(15,-15), gS(1,-1), gS(0,0), gS(11,-2), gS(-11,9), gS(1,-10), gS(2,-29),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-163,-52), gS(-35,-9), gS(-81,7), gS(-16,-8), gS(-2,-4), gS(-83,-2), gS(-31,-8), gS(-94,-66),
           gS(-38,-12), gS(-24,-2), gS(-8,0), gS(11,0), gS(2,-4), gS(27,-14), gS(-12,-4), gS(-22,-20),
           gS(-12,-4), gS(-1,1), gS(13,21), gS(19,9), gS(36,0), gS(47,-2), gS(8,-6), gS(7,-17),
           gS(2,-2), gS(15,4), gS(30,16), gS(44,19), gS(32,17), gS(60,12), gS(22,5), gS(25,-10),
           gS(-8,0), gS(8,3), gS(20,20), gS(27,18), gS(30,23), gS(33,11), gS(27,2), gS(8,0),
           gS(-19,-15), gS(1,-2), gS(9,9), gS(11,17), gS(23,15), gS(22,3), gS(22,-6), gS(6,-7),
           gS(-22,-24), gS(-18,-4), gS(-4,0), gS(8,5), gS(10,2), gS(7,0), gS(-2,-4), gS(-4,-4),
           gS(-42,-28), gS(-19,-6), gS(-14,-5), gS(-3,2), gS(7,4), gS(-4,-1), gS(-14,-3), gS(-21,-28),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-26,-7), gS(-11,0), gS(-3,8), gS(0,19), gS(-10,25), gS(-2,8), gS(-11,11), gS(-19,8),
           gS(-16,-7), gS(-29,12), gS(-21,22), gS(-32,43), gS(-44,63), gS(-27,27), gS(-16,21), gS(12,11),
           gS(-8,-14), gS(-9,-5), gS(-7,23), gS(-8,34), gS(-9,41), gS(4,31), gS(8,8), gS(5,5),
           gS(-8,-14), gS(0,5), gS(-4,17), gS(-11,37), gS(-8,48), gS(3,31), gS(12,24), gS(9,-3),
           gS(2,-28), gS(-6,1), gS(-3,7), gS(-2,34), gS(0,32), gS(-1,12), gS(12,2), gS(2,-1),
           gS(-3,-34), gS(9,-24), gS(2,-5), gS(0,4), gS(4,6), gS(9,-4), gS(23,-14), gS(8,-26),
           gS(3,-53), gS(9,-54), gS(13,-42), gS(13,-26), gS(14,-24), gS(16,-53), gS(28,-74), gS(3,-46),
           gS(0,-62), gS(0,-62), gS(4,-61), gS(6,-45), gS(8,-53), gS(0,-66), gS(2,-60), gS(12,-69),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(78,111), gS(469,689), gS(331,375), gS(357,386), gS(966,1259),
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
