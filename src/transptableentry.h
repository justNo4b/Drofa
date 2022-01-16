#ifndef TRANSPTABLEENTRY_H
#define TRANSPTABLEENTRY_H

#include "move.h"

/**
 * @brief Represent CuttOffState of the node saved in the transposition table.
 */
enum CutOffState{
    NONE,
    EXACT,
    ALPHA,
    BETA
};

/**
 * @brief Represents an entry in a transposition table.
 *
 * Stores key, score, depth, upper/lower bound information and the best move found
 */
struct HASH_Entry
{
  U64 posKey;           // 8
  int move;             // 4
  int16_t score;        // 2
  uint8_t depth;        // 1
  uint8_t Flag;         // 1

  HASH_Entry() : posKey(0), move(0),  score(0), depth(0), Flag(NONE) {}
  HASH_Entry( U64 key, int cMove, int16_t s, uint8_t d, CutOffState state) :
      posKey(key), move(cMove), score(s), depth(d), Flag(state) {}
};

/**
 * @brief Represents an entry in pawn hash table
 *
 * Stores key and scores for an ENDGAME and OPENING elavuation.
 * Also store bitboards for white and black passed pawns
 */

struct pawn_HASH_Entry
{
    U64 posKey;             // 8
    U64 wPassers;           // 8
    U64 bPassers;           // 8
    int score;              // 4
    uint8_t wKsMask;        // 1
    uint8_t wQsMask;        // 1
    uint8_t bKsMask;        // 1
    uint8_t bQsMask;        // 1

    pawn_HASH_Entry() : posKey(0), wPassers(0), bPassers(0), score(0),
                        wKsMask(0), wQsMask(0), bKsMask(0), bQsMask(0) {}
    pawn_HASH_Entry( U64 key, U64 wP, U64 bP, int sc, uint8_t wkm, uint8_t wqm, uint8_t bkm, uint8_t bqm) :
                        posKey(key), wPassers(wP), bPassers(bP), score(sc),
                        wKsMask(wkm), wQsMask(wqm), bKsMask(bkm), bQsMask(bqm) {}
};


#endif
