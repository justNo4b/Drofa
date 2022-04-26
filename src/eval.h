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
const int KING_HIGH_DANGER = gS(-42,-67);

const int KING_MED_DANGER = gS(-25,-31);

const int KING_LOW_DANGER = gS(24,-104);

const int BISHOP_PAIR_BONUS = gS(14,58);

const int DOUBLED_PAWN_PENALTY = gS(-7,-28);

const int ISOLATED_PAWN_PENALTY = gS(-3,-5);

const int PAWN_BLOCKED = gS(1,18);

const int PASSER_BLOCKED = gS(-2,-2);

const int PAWN_PUSH_THREAT = gS(13,19);

const int BISHOP_RAMMED_PENALTY = gS(-5,-2);

const int BISHOP_CENTER_CONTROL = gS(7,11);

const int BISHOP_POS_PROUTPOST_JUMP = gS(9,1);

const int BISHOP_POS_GENOUTPOST_JUMP = gS(0,3);

const int KNIGHT_POS_PROUTPOST_JUMP = gS(10,7);

const int KNIGHT_POS_GENOUTPOST_JUMP = gS(7,11);

const int MINOR_BEHIND_PAWN = gS(4,7);

const int MINOR_BEHIND_PASSER = gS(10,-2);

const int MINOR_BLOCK_OWN_PAWN = gS(-6,-5);

const int MINOR_BLOCK_OWN_PASSER = gS(-1,-3);

const int ROOK_LINE_TENSION = gS(25,-6);

const int ROOK_RAMMED_LINE = gS(-9,-9);

const int KING_AHEAD_PASSER = gS(-14,18);

const int KING_EQUAL_PASSER = gS(9,8);

const int KING_BEHIND_PASSER = gS(-1,-1);

const int KING_OPEN_FILE = gS(-59,6);

const int KING_OWN_SEMI_FILE = gS(-24,13);

const int KING_ENEMY_SEMI_LINE = gS(-18,5);

const int KING_ATTACK_PAWN = gS(4,53);

const int KING_PAWNLESS_FLANG = gS(-9,-64);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-3,-10), gS(1,3), gS(1,39),
           gS(18,51), gS(15,84), gS(24,95),
};

const int PASSED_PAWN_FILES[4] = {
           gS(0,18), gS(-2,17), gS(-2,9), gS(-2,-1),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(-1,7), gS(2,19), gS(57,40),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(3,27), gS(29,55), gS(73,140),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-7,-25), gS(10,0), gS(2,-25), gS(-8,-10),
           gS(-12,-12), gS(-18,-7), gS(-19,-18), gS(-13,-12),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(-2,37), gS(-7,27), gS(-6,9),
           gS(-1,0), gS(0,1), gS(11,0), gS(8,-3),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-45,-1), gS(2,-11), gS(-3,3),
           gS(1,13), gS(-1,20), gS(-1,25), gS(-9,24),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-10,-9), gS(0,-2), gS(7,6), gS(11,15),
};

const int CANDIDATE_PASSED_PAWN[7] = {
           gS(0,0), gS(-11,11), gS(-3,13), gS(5,14),
           gS(-8,20), gS(-35,25), gS(0,0),
};

const int CANDIDATE_PASSED_PAWN_FILES[4] = {
           gS(6,16), gS(-9,-6), gS(-15,-2), gS(-18,-1),
};

const int BACKWARD_PAWN[7] = {
           gS(0,0), gS(-1,-3), gS(0,-5), gS(-5,-11),
           gS(1,-3), gS(0,0), gS(0,0),
};

const int BACKWARD_OPEN_PAWN[7] = {
           gS(0,0), gS(-14,-11), gS(-6,-11), gS(-4,-12),
           gS(4,0), gS(0,0), gS(0,0),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */

const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(28,-4), gS(37,2),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(18,-1), gS(26,5),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(1,22), gS(-124,-118), gS(-71,-112), gS(-104,-149), gS(-21,-37),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(29,47), gS(26,59), gS(20,45),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-1,23), gS(63,77), gS(50,111),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(57,49), gS(19,30), gS(11,47),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-10,-1), gS(9,11), gS(1,11), gS(1,7),
           gS(-7,7), gS(-17,1), gS(1,22), gS(-1,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(15,1), gS(19,20), gS(24,2), gS(21,1),
           gS(12,1), gS(-13,0), gS(3,18), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-17,-66), gS(-7,-26), gS(-1,-12), gS(4,-3), gS(5,7), gS(4,12), gS(3,13),
           gS(0,12), gS(-3,13), gS(-4,9), gS(2,8), gS(19,-1), gS(27,9), gS(44,-11),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-18,-92), gS(-3,-32), gS(3,-7), gS(8,1), gS(11,8),
           gS(12,11), gS(14,7), gS(13,2), gS(8,-6),
};

const int KING_MOBILITY[9] = {
           gS(14,-32), gS(7,-9), gS(0,7), gS(-2,11), gS(-6,9),
           gS(-9,5), gS(0,3), gS(1,-1), gS(12,-15),
};

const int ROOK_MOBILITY[15] = {
           gS(-36,-89), gS(-26,-52), gS(-19,-36), gS(-12,-19), gS(-9,-6), gS(-11,4), gS(-6,9),
           gS(-3,7), gS(0,13), gS(1,18), gS(1,22), gS(3,24), gS(8,20), gS(18,17),
           gS(42,5),
};

const int QUEEN_MOBILITY[28] = {
           gS(-21,-114), gS(-11,-150), gS(-9,-112), gS(-8,-76), gS(-7,-57), gS(-5,-42), gS(-4,-22),
           gS(-3,-8), gS(-1,2), gS(-1,15), gS(0,23), gS(0,30), gS(0,35), gS(0,38),
           gS(0,40), gS(-2,44), gS(0,45), gS(1,41), gS(4,39), gS(7,36), gS(12,29),
           gS(15,20), gS(9,24), gS(18,14), gS(0,8), gS(5,3), gS(8,4), gS(0,2),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(0,-85), gS(0,-21), gS(0,-5), gS(0,-4), gS(0,-15), gS(0,-20), gS(0,-11), gS(0,-93),
           gS(0,-11), gS(0,38), gS(5,39), gS(0,35), gS(1,20), gS(0,40), gS(0,34), gS(-4,-18),
           gS(0,0), gS(27,32), gS(9,42), gS(0,40), gS(0,42), gS(17,41), gS(11,36), gS(0,-2),
           gS(0,-14), gS(10,14), gS(-2,28), gS(-8,33), gS(-9,32), gS(0,28), gS(6,15), gS(-31,-15),
           gS(-12,-28), gS(-2,0), gS(-6,12), gS(-15,24), gS(-15,21), gS(-22,15), gS(-5,-3), gS(-41,-24),
           gS(-8,-33), gS(7,-10), gS(-17,7), gS(-35,18), gS(-28,18), gS(-21,9), gS(4,-7), gS(-8,-33),
           gS(5,-38), gS(0,-8), gS(1,-1), gS(-27,11), gS(-22,10), gS(9,-2), gS(5,-11), gS(10,-38),
           gS(-23,-63), gS(8,-41), gS(-12,-15), gS(-34,-23), gS(18,-48), gS(-13,-33), gS(21,-38), gS(2,-70),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(50,73), gS(33,73), gS(31,63), gS(64,51), gS(55,55), gS(32,58), gS(-6,78), gS(2,74),
           gS(2,33), gS(10,26), gS(22,12), gS(42,4), gS(38,1), gS(53,5), gS(20,14), gS(28,19),
           gS(-16,19), gS(4,9), gS(3,4), gS(7,-1), gS(13,-3), gS(10,0), gS(17,1), gS(-6,4),
           gS(-28,5), gS(-22,11), gS(-9,-1), gS(7,-4), gS(7,-4), gS(3,-2), gS(-9,2), gS(-22,-5),
           gS(-27,-2), gS(-18,1), gS(-9,-2), gS(-7,-2), gS(-2,-2), gS(-5,-3), gS(-7,-7), gS(-23,-10),
           gS(-38,8), gS(-15,10), gS(-15,8), gS(-2,7), gS(-3,6), gS(2,7), gS(-4,2), gS(-31,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-3,18), gS(-8,12), gS(-2,10), gS(-2,5), gS(-2,2), gS(-6,1), gS(-4,3), gS(0,9),
           gS(-4,14), gS(-2,20), gS(-11,13), gS(0,4), gS(1,6), gS(-15,12), gS(-2,26), gS(-4,17),
           gS(1,-5), gS(0,-10), gS(-9,-7), gS(2,-13), gS(0,-10), gS(-2,0), gS(-1,-1), gS(-4,1),
           gS(8,-20), gS(2,-9), gS(9,-19), gS(3,-24), gS(2,-26), gS(3,-10), gS(0,0), gS(5,0),
           gS(9,-26), gS(0,-4), gS(-4,-9), gS(-2,2), gS(-4,-7), gS(-4,3), gS(7,0), gS(7,-27),
           gS(9,-37), gS(5,-14), gS(-1,2), gS(-12,1), gS(-8,5), gS(5,9), gS(5,0), gS(12,-41),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-8,8), gS(-13,6), gS(-4,3), gS(-3,0), gS(-3,0), gS(-8,0), gS(-6,0), gS(-2,0),
           gS(-3,14), gS(-5,13), gS(-9,17), gS(-1,4), gS(0,4), gS(-16,8), gS(0,26), gS(-5,11),
           gS(2,0), gS(-1,-6), gS(-2,1), gS(-1,-12), gS(-7,-11), gS(0,-5), gS(-10,-5), gS(-2,2),
           gS(0,12), gS(0,0), gS(-2,-9), gS(-4,-14), gS(-3,-12), gS(-2,-14), gS(-1,-4), gS(-2,-7),
           gS(0,0), gS(-5,0), gS(-5,-2), gS(-12,0), gS(-8,5), gS(-6,5), gS(1,-6), gS(2,-3),
           gS(0,30), gS(-3,11), gS(-8,24), gS(-5,17), gS(-7,18), gS(-5,17), gS(4,11), gS(2,13),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(13,20), gS(10,29), gS(-8,29), gS(-10,22), gS(-3,21), gS(-3,19), gS(21,23), gS(25,18),
           gS(5,21), gS(-1,23), gS(16,19), gS(34,11), gS(27,10), gS(23,7), gS(-4,22), gS(11,17),
           gS(-6,19), gS(15,12), gS(13,12), gS(24,4), gS(35,-2), gS(28,1), gS(23,4), gS(-2,16),
           gS(-7,14), gS(3,19), gS(11,9), gS(18,4), gS(21,3), gS(20,0), gS(11,10), gS(7,5),
           gS(-16,7), gS(-15,13), gS(-3,7), gS(-3,3), gS(-2,2), gS(-4,3), gS(-4,3), gS(-18,1),
           gS(-15,-3), gS(-5,-2), gS(-8,-2), gS(-7,-6), gS(-3,-9), gS(-4,-13), gS(-2,-13), gS(-8,-17),
           gS(-21,-19), gS(-10,-20), gS(-3,-16), gS(-1,-16), gS(4,-25), gS(-1,-29), gS(0,-29), gS(-44,-21),
           gS(-4,-20), gS(0,-22), gS(-1,-14), gS(9,-19), gS(11,-26), gS(12,-25), gS(-12,-15), gS(0,-36),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-38,-2), gS(-48,0), gS(-101,13), gS(-85,15), gS(-57,13), gS(-124,5), gS(-31,-6), gS(-21,-17),
           gS(-34,-1), gS(-36,0), gS(-9,5), gS(-38,7), gS(-43,1), gS(-24,4), gS(-58,-9), gS(-23,-9),
           gS(-32,3), gS(1,3), gS(-33,3), gS(3,3), gS(0,0), gS(-35,-3), gS(9,2), gS(-14,-3),
           gS(-1,2), gS(29,8), gS(-6,4), gS(11,13), gS(15,10), gS(-2,4), gS(27,3), gS(10,-7),
           gS(2,-4), gS(-12,8), gS(21,12), gS(19,12), gS(21,6), gS(19,8), gS(-1,0), gS(4,-14),
           gS(10,-13), gS(26,-4), gS(21,-5), gS(22,8), gS(16,9), gS(28,-5), gS(23,-7), gS(14,-6),
           gS(12,-21), gS(20,-25), gS(18,-9), gS(8,1), gS(20,0), gS(22,-12), gS(29,-22), gS(16,-27),
           gS(-3,-31), gS(4,-13), gS(1,-7), gS(-3,-4), gS(2,-10), gS(-9,2), gS(-1,-10), gS(-7,-29),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-155,-52), gS(-33,-4), gS(-92,5), gS(-17,-6), gS(-1,-3), gS(-100,-1), gS(-30,-6), gS(-90,-63),
           gS(-35,-10), gS(-24,-1), gS(8,5), gS(0,4), gS(-6,5), gS(21,-9), gS(-20,-5), gS(-20,-15),
           gS(-14,-2), gS(7,5), gS(-8,26), gS(19,12), gS(23,4), gS(21,8), gS(7,1), gS(11,-14),
           gS(7,-1), gS(22,3), gS(19,20), gS(31,22), gS(25,23), gS(30,16), gS(22,8), gS(25,-9),
           gS(3,0), gS(2,5), gS(28,22), gS(23,21), gS(25,23), gS(36,16), gS(16,1), gS(18,-1),
           gS(-9,-27), gS(9,-8), gS(16,9), gS(24,15), gS(24,12), gS(26,6), gS(28,-10), gS(1,-14),
           gS(-17,-32), gS(-12,-5), gS(3,-13), gS(5,9), gS(10,8), gS(12,-5), gS(-1,-3), gS(-8,-15),
           gS(-41,-32), gS(-20,-21), gS(-12,-8), gS(-7,-4), gS(8,-1), gS(-14,-9), gS(-15,-13), gS(-23,-31),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-11,1), gS(2,10), gS(3,10), gS(4,20), gS(-2,28), gS(2,11), gS(-4,19), gS(-4,18),
           gS(-16,3), gS(-26,23), gS(-11,27), gS(-23,46), gS(-39,65), gS(-17,33), gS(-8,30), gS(9,13),
           gS(-10,-15), gS(-3,0), gS(0,24), gS(0,37), gS(2,46), gS(8,36), gS(17,17), gS(-1,12),
           gS(-5,-16), gS(0,10), gS(-5,22), gS(-2,42), gS(0,56), gS(2,33), gS(0,28), gS(14,-1),
           gS(-1,-28), gS(-15,-1), gS(-3,13), gS(-8,39), gS(3,32), gS(-5,12), gS(5,6), gS(-6,0),
           gS(-11,-33), gS(6,-24), gS(3,-5), gS(3,-4), gS(-1,-5), gS(5,-8), gS(10,-15), gS(0,-26),
           gS(-3,-53), gS(3,-61), gS(15,-60), gS(4,-39), gS(10,-37), gS(11,-69), gS(20,-84), gS(-1,-46),
           gS(-4,-66), gS(-5,-73), gS(-3,-82), gS(-2,-71), gS(-2,-71), gS(-20,-83), gS(-4,-66), gS(10,-70),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(79,121), gS(403,679), gS(279,374), gS(309,392), gS(951,1228),
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
