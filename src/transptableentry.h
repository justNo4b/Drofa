#ifndef TRANSPTABLEENTRY_H
#define TRANSPTABLEENTRY_H

#include "move.h"

/**
 * @brief Represent CuttOffState of the node saved in the transposition table.
 */
enum CutOffState{
    NONE,
    EXACT,
    BETA
};

/**
 * @brief Represents an entry in a transposition table.
 *
 * Stores key, score, depth, upper/lower bound information and the best move found
 */
struct HASH_Entry
{
  uint32_t posKey;      // 4
  int move;             // 4
  int eval;             // 4
  int16_t score;        // 2
  uint8_t depth;        // 1
  uint8_t Flag;         // 1

  HASH_Entry() : posKey(0), move(0), eval(0),  score(0), depth(0), Flag(NONE) {}
  HASH_Entry( uint32_t key, int cMove, int e, int16_t s, uint8_t d, CutOffState state) :
      posKey(key), move(cMove), eval(e), score(s), depth(d), Flag(state) {}
};

/**
 * @brief Represents an entry in pawn hash table
 *
 * Stores key and scores for an ENDGAME and OPENING elavuation.
 * Also store bitboards for white and black passed pawns
 */

struct pawn_HASH_Entry
{
    U64 posKey;
    U64 wPassers;
    U64 bPassers;
    int score;

    pawn_HASH_Entry() : posKey(0), wPassers(0), bPassers(0), score(0) {}
    pawn_HASH_Entry( U64 key, U64 wP, U64 bP, int sc ) :
        posKey(key), wPassers(wP), bPassers(bP), score(sc) {}
};


#endif
