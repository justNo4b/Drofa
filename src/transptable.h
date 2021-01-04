#ifndef TRANSPTABLE_H
#define TRANSPTABLE_H

#include "board.h"
#include "zkey.h"
#include "transptableentry.h"
#include <unordered_map>

/**
 * @brief A transposition table.
 *
 * Each entry is mapped to by a ZKey and contains a score, depth and flag which
 * indicates if the stored score is an upper bound, lower bound or exact score.
 *
 */
class HASH{
  public:
  
  HASH();

  void        HASH_Clear();
  HASH_Entry  HASH_Get  (U64 posKey);
  void        HASH_Store(U64 posKey, int cMove, CutOffState bound, int score, int depth, int ply);
  U64         HASH_Size();

  void            pHASH_Clear();
  pawn_HASH_Entry pHASH_Get(U64 posKey);
  void            pHASH_Store(U64 posKey, U64, U64, int END_score, int OP_Score);
  U64             pHASH_Size();

  private:

  HASH_Entry *hashTable;
  U64 TableSize;

  pawn_HASH_Entry *pHASH;
  U64 pTableSize;

};

#endif
