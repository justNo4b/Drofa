/**
 * @file
 *
 * Contains global constants and functions used in many classes / functions.
 *
 */

#ifndef DEFS_H
#define DEFS_H

#include <limits>
#include <iostream>
#include <cstdlib>

/**
 * An unsigned 64 bit integer (A bitboard).
 */
typedef unsigned long long U64;


#define MIN_HASH    (    8)
#define MAX_HASH    (65536)
#define MIN_THREADS (    1)
#define MAX_THREADS (  172)
#define MAX_PLY     (  127)
#define MAX_INT_PLY (  128)

/**
 * @name Rank bitboards
 *
 * @{
 */
const U64 RANK_1 = 0xffull;
const U64 RANK_2 = 0xff00ull;
const U64 RANK_3 = 0xff0000ull;
const U64 RANK_4 = 0xff000000ull;
const U64 RANK_5 = 0xff00000000ull;
const U64 RANK_6 = 0xff0000000000ull;
const U64 RANK_7 = 0xff000000000000ull;
const U64 RANK_8 = 0xff00000000000000ull;

/**@}*/


/**
 * Global search constants here
 *
*/
const int LOST_SCORE = -30000;
const int NOSCORE = 32666;
const int WON_IN_X = 30000 - 50;
const int MAX_GAME_PLY = 2048;

/**
 * @enum SquareIndex
 * @brief Little endian rank file mapping of each square.
 */
enum SquareIndex {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
};

/**
 * @brief An empty bitboard. (ie. the number 0)
 */
const U64 ZERO = U64(0);

/**
 * @brief A bitboard containing only the square a1. (ie. the number 1)
 */
const U64 ONE = U64(1);

/**
 * @name File bitboards
 *
 * @{
 */
const U64 FILE_H = 0x8080808080808080ull;
const U64 FILE_G = 0x4040404040404040ull;
const U64 FILE_F = 0x2020202020202020ull;
const U64 FILE_E = 0x1010101010101010ull;
const U64 FILE_D = 0x808080808080808ull;
const U64 FILE_C = 0x404040404040404ull;
const U64 FILE_B = 0x202020202020202ull;
const U64 FILE_A = 0x101010101010101ull;
/**@}*/

/**
 * @name Black and white squares
 */
const U64 BLACK_SQUARES = 0xAA55AA55AA55AA55;
const U64 WHITE_SQUARES = 0x55AA55AA55AA55AA;
/**@}*/

/**
 * @name Other helpfull bitboards
 */
const U64 CENTER          = (ONE << e4) | (ONE << e5) | (ONE << d4) | (ONE << d5);
const U64 FIGHTING_AREA   = RANK_3 | RANK_4 | RANK_5 | RANK_6;
const U64 EXTENDED_CENTER = FIGHTING_AREA & (FILE_C | FILE_D | FILE_E | FILE_F);

const U64 PASSER_ZONE [2] = { (RANK_5 | RANK_6),
                              (RANK_3 | RANK_4) };
const U64 ENEMY_SIDE [2]  = { (RANK_5 | RANK_6 | RANK_7 | RANK_8),
                              (RANK_1 | RANK_2 | RANK_3 | RANK_4) };
const U64 KSIDE_CASTLE [2] = { ((ONE << g1) | (ONE << h1) | (ONE << g2) | (ONE << h2)),
                               ((ONE << g8) | (ONE << h8) | (ONE << g7) | (ONE << h7)) };
const U64 QSIDE_CASTLE [2] = { ((ONE << a1) | (ONE << b1) | (ONE << c1) | (ONE << a2) | (ONE << b2)),
                               ((ONE << a8) | (ONE << b8) | (ONE << c8) | (ONE << a7) | (ONE << b7)) };
/**@}*/

/** @brief Positive infinity to be used during search (eg. as a return value for winning) */
const int INF = std::numeric_limits<int>::max();

/**
 * @enum Color
 * @brief Represents a color.
 */
enum Color {
  WHITE,
  BLACK
};

/**
 * @enum PieceType
 * @brief Represents a piece type.
 */
enum PieceType {
  PAWN,
  ROOK,
  KNIGHT,
  BISHOP,
  QUEEN,
  KING
};

const int PHASE_WEIGHT_SUM = 24;
const int MAX_PHASE = 256;

const int PHASE_WEIGHTS[6] = {
    [PAWN] = 0,
    [ROOK] = 2,
    [KNIGHT] = 1,
    [BISHOP] = 1,
    [QUEEN] = 4,
    [KING] = 0
};

/**
 * @brief Returns the opposite of the given color
 *
 * @param  color Color to get the opposite of
 * @return WHITE if color == BLACK, BLACK otherwise
 */
inline Color getOppositeColor(Color color) {
  return color == WHITE ? BLACK : WHITE;
}

/**
 * @brief Print the given message to stderr and exit with code 1. Should be used in
 * unrecoverable situations.
 *
 * @param msg Message to print to stderr before exiting
 */
[[ noreturn ]]
inline void fatal(std::string msg) {
  std::cerr << msg << std::endl;
  std::exit(1);
}

/**
 * @enum GamePhase
 * @brief Enum representing the game phase (opening/endgame)
 */
enum GamePhase {
  OPENING,
  ENDGAME
};
#endif
