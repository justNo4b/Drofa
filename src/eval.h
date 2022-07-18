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
  const int EG_SCALE_BASE   = 32;
  const int EG_SCALE_PAWN    = 8;
  const int EG_SCALE_MAXIMUM = 256;

  const int SCALE_OCB = 2;
  const int SCALE_ROOK_OCB = 3;
  const int SCALE_KNIGHT_OCB = 3;
  const int SCALE_NOSCALE = 4;
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

const int KING_HIGH_DANGER = gS(-46,-73);

const int KING_MED_DANGER = gS(-25,-44);

const int KING_LOW_DANGER = gS(87,-65);

const int BISHOP_PAIR_BONUS = gS(27,61);

const int DOUBLED_PAWN_PENALTY = gS(-10,-20);

const int ISOLATED_PAWN_PENALTY = gS(-5,-7);

const int PAWN_BLOCKED = gS(2,14);

const int PASSER_BLOCKED = gS(4,1);

const int PAWN_PUSH_THREAT = gS(12,16);

const int BISHOP_RAMMED_PENALTY = gS(-2,-4);

const int BISHOP_CENTER_CONTROL = gS(8,12);

const int BISHOP_POS_PROUTPOST_JUMP = gS(12,-3);

const int BISHOP_POS_GENOUTPOST_JUMP = gS(0,5);

const int KNIGHT_POS_PROUTPOST_JUMP = gS(7,8);

const int KNIGHT_POS_GENOUTPOST_JUMP = gS(7,7);

const int MINOR_BEHIND_PAWN = gS(4,8);

const int MINOR_BEHIND_PASSER = gS(7,-1);

const int MINOR_BLOCK_OWN_PAWN = gS(-7,-1);

const int MINOR_BLOCK_OWN_PASSER = gS(-1,-4);

const int ROOK_LINE_TENSION = gS(20,-3);

const int ROOK_RAMMED_LINE = gS(-8,-8);

const int KING_AHEAD_PASSER = gS(-9,12);

const int KING_EQUAL_PASSER = gS(11,8);

const int KING_BEHIND_PASSER = gS(-4,0);

const int KING_OPEN_FILE = gS(-63,12);

const int KING_OWN_SEMI_FILE = gS(-22,11);

const int KING_ENEMY_SEMI_LINE = gS(-18,9);

const int KING_ATTACK_PAWN = gS(9,53);

const int KING_PAWNLESS_FLANG = gS(-13,-58);

/**
 * @brief Various array terms, related to the evaluation of the pawns
 */
const int PASSED_PAWN_RANKS[7] = {
           gS(0,0), gS(-12,-9), gS(-9,3), gS(-4,40),
           gS(14,55), gS(25,79), gS(51,90),
};

const int PASSED_PAWN_FILES[4] = {
           gS(-2,26), gS(-3,15), gS(0,5), gS(0,-6),
};

const int PASSED_PAWN_FREE[7] = {
           gS(0,0), gS(-7,8), gS(-11,2), gS(-8,5),
           gS(0,8), gS(9,20), gS(66,25),
};

const int PASSED_PAWN_POS_ADVANCE[7] = {
           gS(0,0), gS(4,5), gS(5,1), gS(6,13),
           gS(10,29), gS(35,59), gS(68,114),
};

const int PASSED_PASSED_DISTANCE[8] = {
           gS(-5,-22), gS(14,7), gS(-1,-16), gS(-7,-6),
           gS(-9,-8), gS(-18,-9), gS(-16,-15), gS(-14,-12),
};

const int KING_PASSER_DISTANCE_FRIENDLY[8] = {
           gS(0,0), gS(0,35), gS(-9,25), gS(-7,9),
           gS(-3,0), gS(0,1), gS(13,0), gS(11,0),
};

const int KING_PASSER_DISTANCE_ENEMY[8] = {
           gS(0,0), gS(-54,-5), gS(1,-13), gS(-2,5),
           gS(2,14), gS(-1,23), gS(2,23), gS(-6,22),
};

const int KNIGHT_PASSER_DISTANCE_ENEMY[4] = {
           gS(-1,-11), gS(4,0), gS(11,3), gS(17,5),
};

const int CANDIDATE_PASSED_PAWN[7] = {
           gS(0,0), gS(-18,0), gS(-11,6), gS(0,12),
           gS(5,31), gS(7,53), gS(0,0),
};

const int CANDIDATE_PASSED_PAWN_FILES[4] = {
           gS(9,18), gS(0,8), gS(0,1), gS(-5,-2),
};

const int BACKWARD_PAWN[7] = {
           gS(0,0), gS(0,-6), gS(1,-5), gS(-1,-7),
           gS(0,-4), gS(0,0), gS(0,0),
};

const int BACKWARD_OPEN_PAWN[7] = {
           gS(0,0), gS(-12,-13), gS(-7,-10), gS(-10,-4),
           gS(-1,0), gS(0,0), gS(0,0),
};

/**
 * @brief Bonuses given to a player for each rook on an Open and SemiOpen file
 *        We apply different bonus if there is an enemy outpost on that file
 */

const int ROOK_OPEN_FILE_BONUS[2] = {
           gS(20,8), gS(33,0),
};

const int ROOK_SEMI_FILE_BONUS[2] = {
           gS(14,-5), gS(22,5),
};

/**
 * @brief indexed by [pieceType]
 * Evaluate OUR pieces attacked by an enemy pawn
 * and OUR pieces attacking enemy pawns - [PAWN]
 */
const int HANGING_PIECE[5] = {
           gS(0,19), gS(-68,-40), gS(-53,-47), gS(-55,-67), gS(-50,-32),
};

const int MINOR_ATTACKED_BY[4] = {
           gS(0,0), gS(19,32), gS(16,36), gS(15,35),
};

const int ROOK_ATTACKED_BY[4] = {
           gS(0,0), gS(-3,18), gS(64,23), gS(43,44),
};

const int QUEEN_ATTACKED_BY[4] = {
           gS(0,0), gS(62,36), gS(40,21), gS(41,45),
};

const int KING_PAWN_SHIELD_KS[8] = {
           gS(-6,2), gS(6,9), gS(0,11), gS(-1,4),
           gS(-9,5), gS(-21,0), gS(-7,18), gS(0,0),
};

const int KING_PAWN_SHIELD_QS[8] = {
           gS(8,0), gS(14,20), gS(13,0), gS(24,0),
           gS(9,0), gS(-18,-1), gS(-5,15), gS(10,0),
};

/**
 * @brief Bonuses given to a player having a move available (opening/endgame)
 */
const int BISHOP_MOBILITY[14] = {
           gS(-5,-65), gS(3,-24), gS(10,-11), gS(15,0), gS(17,9), gS(18,12), gS(17,15),
           gS(16,16), gS(14,18), gS(13,17), gS(16,13), gS(28,3), gS(30,11), gS(45,-9),
};

const int KNIGHT_MOBILITY[9] = {
           gS(-2,-94), gS(10,-35), gS(16,-6), gS(21,4), gS(24,14),
           gS(27,18), gS(29,17), gS(31,13), gS(33,2),
};

const int KING_MOBILITY[9] = {
           gS(14,-31), gS(6,-6), gS(2,1), gS(0,5), gS(-3,5),
           gS(-8,5), gS(0,3), gS(-5,3), gS(5,-8),
};

const int ROOK_MOBILITY[15] = {
           gS(-28,-89), gS(-15,-50), gS(-8,-30), gS(0,-16), gS(2,-4), gS(0,8), gS(5,10),
           gS(7,8), gS(10,14), gS(12,19), gS(15,22), gS(19,22), gS(19,24), gS(22,25),
           gS(41,15),
};

const int QUEEN_MOBILITY[28] = {
           gS(-10,-114), gS(-6,-149), gS(-5,-111), gS(-5,-74), gS(-4,-53), gS(-3,-35), gS(-3,-18),
           gS(-1,-5), gS(0,6), gS(0,17), gS(0,24), gS(1,30), gS(2,34), gS(2,38),
           gS(2,41), gS(3,44), gS(2,47), gS(6,42), gS(8,40), gS(10,38), gS(14,31),
           gS(17,23), gS(10,25), gS(19,16), gS(0,8), gS(5,4), gS(8,5), gS(0,1),
};

/**
 * @brief Piece - Square - Tables
 * Bonuses for each piece being on the square on the board. From Black point of view
 */
const int KING_PSQT_BLACK[64] = {
           gS(0,-85), gS(0,-23), gS(0,-5), gS(0,-4), gS(0,-17), gS(0,-20), gS(0,-10), gS(0,-94),
           gS(0,-11), gS(0,35), gS(4,35), gS(0,34), gS(0,17), gS(0,35), gS(0,31), gS(-4,-19),
           gS(0,0), gS(26,27), gS(8,39), gS(-1,42), gS(0,42), gS(16,38), gS(10,29), gS(0,0),
           gS(0,-11), gS(9,18), gS(-2,29), gS(-8,40), gS(-9,36), gS(0,29), gS(6,17), gS(-30,-14),
           gS(-10,-22), gS(0,4), gS(-5,18), gS(-14,25), gS(-16,25), gS(-22,18), gS(-3,2), gS(-38,-19),
           gS(-6,-30), gS(12,-7), gS(-10,4), gS(-29,16), gS(-24,14), gS(-20,6), gS(1,-10), gS(-4,-31),
           gS(5,-31), gS(0,-12), gS(0,-5), gS(-20,4), gS(-21,5), gS(0,-5), gS(0,-16), gS(6,-37),
           gS(-13,-57), gS(7,-41), gS(-13,-18), gS(-34,-17), gS(12,-49), gS(-11,-29), gS(6,-40), gS(1,-72),
};

const int PAWN_PSQT_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(48,71), gS(40,74), gS(35,64), gS(65,49), gS(59,55), gS(40,58), gS(0,79), gS(0,68),
           gS(0,35), gS(7,28), gS(24,12), gS(38,-1), gS(37,0), gS(66,6), gS(22,23), gS(23,17),
           gS(-19,17), gS(-5,12), gS(-3,2), gS(6,-1), gS(10,-1), gS(7,0), gS(12,1), gS(-14,8),
           gS(-28,5), gS(-26,14), gS(-13,1), gS(5,-3), gS(4,-4), gS(2,-1), gS(-11,3), gS(-23,-4),
           gS(-29,0), gS(-21,0), gS(-8,-5), gS(-5,-2), gS(0,-2), gS(0,-5), gS(-4,-10), gS(-22,-11),
           gS(-37,6), gS(-18,5), gS(-13,6), gS(0,5), gS(0,8), gS(1,4), gS(-5,-1), gS(-28,-6),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-3,15), gS(-2,10), gS(0,7), gS(0,3), gS(0,1), gS(0,0), gS(0,0), gS(0,7),
           gS(-6,12), gS(-7,18), gS(-11,8), gS(-1,0), gS(0,2), gS(-7,5), gS(-4,22), gS(-5,15),
           gS(-2,-4), gS(-1,-4), gS(-9,-9), gS(1,-20), gS(1,-17), gS(-4,-4), gS(-3,-2), gS(-2,0),
           gS(0,-18), gS(-3,-9), gS(3,-15), gS(0,-26), gS(0,-25), gS(1,-8), gS(-3,-1), gS(0,0),
           gS(0,-28), gS(-2,-4), gS(-7,-7), gS(-4,0), gS(-7,-10), gS(-8,0), gS(2,-8), gS(0,-29),
           gS(0,-46), gS(0,-22), gS(-6,-6), gS(-14,0), gS(-9,0), gS(0,-4), gS(0,-9), gS(0,-50),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_PSQT_BLACK_IS_OWN_QUEEN[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-6,7), gS(-5,7), gS(-1,2), gS(0,0), gS(0,0), gS(-1,0), gS(-1,0), gS(-2,0),
           gS(-4,11), gS(-8,12), gS(-8,12), gS(-1,0), gS(0,1), gS(-7,3), gS(0,22), gS(-3,9),
           gS(1,0), gS(2,0), gS(2,0), gS(3,-15), gS(2,-14), gS(2,-7), gS(-5,-3), gS(3,2),
           gS(0,16), gS(2,2), gS(2,-3), gS(0,-5), gS(0,-4), gS(2,-7), gS(2,1), gS(1,2),
           gS(3,4), gS(0,6), gS(-1,4), gS(-5,5), gS(-1,7), gS(-2,12), gS(4,0), gS(7,5),
           gS(4,33), gS(0,17), gS(-3,26), gS(-2,20), gS(-4,17), gS(2,21), gS(6,15), gS(8,21),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int ROOK_PSQT_BLACK[64] = {
           gS(28,15), gS(16,22), gS(0,28), gS(0,21), gS(5,19), gS(10,21), gS(23,19), gS(36,16),
           gS(7,16), gS(0,17), gS(15,15), gS(34,6), gS(26,7), gS(25,6), gS(-7,17), gS(7,12),
           gS(2,18), gS(17,9), gS(17,11), gS(25,2), gS(39,-2), gS(32,0), gS(25,2), gS(-5,14),
           gS(1,12), gS(2,16), gS(19,11), gS(21,3), gS(21,2), gS(21,1), gS(8,6), gS(0,2),
           gS(-13,6), gS(-9,13), gS(0,7), gS(0,5), gS(2,3), gS(-1,2), gS(-3,6), gS(-19,2),
           gS(-16,-2), gS(-9,0), gS(-4,0), gS(-3,-4), gS(-2,-4), gS(0,-9), gS(0,-8), gS(-13,-12),
           gS(-21,-10), gS(-6,-10), gS(0,-8), gS(0,-11), gS(11,-17), gS(7,-23), gS(4,-24), gS(-40,-12),
           gS(-6,-13), gS(0,-14), gS(1,-10), gS(11,-24), gS(14,-26), gS(12,-22), gS(-10,-11), gS(-4,-27),
};

const int BISHOP_PSQT_BLACK[64] = {
           gS(-36,-1), gS(-42,-2), gS(-73,16), gS(-80,16), gS(-53,15), gS(-90,13), gS(-26,-6), gS(-29,-17),
           gS(-37,-2), gS(-19,-2), gS(-13,-1), gS(-11,9), gS(-12,6), gS(-17,3), gS(-27,-6), gS(-24,-8),
           gS(-4,0), gS(0,-1), gS(-2,3), gS(19,-1), gS(13,1), gS(18,-1), gS(3,1), gS(5,-2),
           gS(-7,-1), gS(15,6), gS(5,3), gS(22,14), gS(17,14), gS(13,7), gS(13,4), gS(-1,-7),
           gS(5,-4), gS(-3,5), gS(7,8), gS(19,9), gS(23,6), gS(7,5), gS(7,2), gS(5,-14),
           gS(2,-14), gS(19,-3), gS(10,-2), gS(10,8), gS(5,13), gS(21,-6), gS(17,-9), gS(7,-10),
           gS(14,-19), gS(11,-25), gS(15,-9), gS(3,1), gS(13,2), gS(14,-4), gS(20,-21), gS(7,-29),
           gS(-3,-32), gS(6,-6), gS(-2,-1), gS(1,1), gS(6,-2), gS(-12,7), gS(0,-7), gS(-2,-24),
};

const int KNIGHT_PSQT_BLACK[64] = {
           gS(-166,-56), gS(-31,-3), gS(-82,8), gS(-13,-2), gS(0,0), gS(-83,2), gS(-29,-3), gS(-91,-64),
           gS(-35,-9), gS(-21,1), gS(-4,0), gS(18,7), gS(7,3), gS(24,-11), gS(-13,-2), gS(-22,-16),
           gS(-11,-1), gS(4,3), gS(9,21), gS(26,8), gS(40,2), gS(44,6), gS(10,-1), gS(13,-13),
           gS(5,0), gS(13,3), gS(19,18), gS(35,22), gS(23,25), gS(39,16), gS(16,4), gS(23,-9),
           gS(0,1), gS(13,6), gS(19,21), gS(24,19), gS(24,25), gS(31,15), gS(25,2), gS(11,-1),
           gS(-8,-22), gS(9,-7), gS(10,3), gS(20,15), gS(23,13), gS(22,0), gS(24,-9), gS(2,-12),
           gS(-7,-27), gS(-12,1), gS(3,-7), gS(5,7), gS(12,5), gS(12,-5), gS(0,0), gS(1,-15),
           gS(-40,-30), gS(-14,-16), gS(-1,-6), gS(0,1), gS(7,2), gS(-2,-4), gS(-8,-9), gS(-18,-29),
};

const int QUEEN_PSQT_BLACK[64] = {
           gS(-15,0), gS(0,10), gS(0,8), gS(0,16), gS(-7,24), gS(0,10), gS(-10,17), gS(-14,15),
           gS(-14,3), gS(-28,22), gS(-13,25), gS(-26,44), gS(-40,63), gS(-20,30), gS(-13,29), gS(1,10),
           gS(-10,-15), gS(-1,0), gS(-4,23), gS(0,35), gS(0,43), gS(4,33), gS(7,12), gS(-5,8),
           gS(-3,-16), gS(-4,8), gS(-3,19), gS(-1,40), gS(-4,54), gS(0,33), gS(0,25), gS(5,-4),
           gS(0,-26), gS(-6,0), gS(-1,11), gS(-2,38), gS(3,34), gS(-3,10), gS(8,5), gS(-5,-3),
           gS(-4,-32), gS(9,-23), gS(5,-4), gS(3,0), gS(3,0), gS(7,-3), gS(13,-11), gS(3,-25),
           gS(3,-51), gS(10,-57), gS(18,-57), gS(11,-33), gS(13,-29), gS(20,-64), gS(24,-79), gS(0,-44),
           gS(5,-61), gS(6,-67), gS(4,-75), gS(4,-58), gS(6,-63), gS(-4,-74), gS(0,-61), gS(13,-69),
};


/**
 * @brief Array indexed by [PieceType] of material values (in centipawns)
 */
const int MATERIAL_VALUES[5] = {
           gS(88,118), gS(515,717), gS(355,399), gS(380,413), gS(1016,1268),
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
