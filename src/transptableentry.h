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
  U64 posKey;
  int move;
  CutOffState Flag;
  int score;
  int depth;

  HASH_Entry() : posKey(0), move(0), Flag(NONE), score(0), depth(0) {}
  HASH_Entry( U64 key, int cMove, CutOffState state, int s, int d) :
      posKey(key), move(cMove), Flag(state), score(s), depth(d) {}
};

/**
 * @brief Represents an entry in pawn hash table
 *
 * Stores key and scores for an ENDGAME and OPENING elavuation.
 * For now. Maybe will store more later.
 */

struct pawn_HASH_Entry
{
    U64 posKey;
    int score_ENDGAME;
    int score_OPENING;

    pawn_HASH_Entry() : posKey(0), score_ENDGAME(0), score_OPENING(0) {}
    pawn_HASH_Entry( U64 key, int scE, int scO) :
        posKey(key), score_ENDGAME(scE), score_OPENING(scO) {}
};


#endif
