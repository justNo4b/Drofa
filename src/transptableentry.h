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
    U64 posKey;
    U64 wPassers;
    U64 bPassers;
    U64 wCandidates;
    U64 bCandidates;
    int score;

    pawn_HASH_Entry() : posKey(0), wPassers(0), bPassers(0), wCandidates(0), bCandidates(0), score(0) {}
    pawn_HASH_Entry( U64 key, U64 wP, U64 bP, U64 wC, U64 bC, int sc ) :
        posKey(key), wPassers(wP), bPassers(bP), wCandidates(wC), bCandidates(bC), score(sc) {}
};


#endif
