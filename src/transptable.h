#ifndef TRANSPTABLE_H
#define TRANSPTABLE_H

#include "transptableentry.h"

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

  void          HASH_Clear();
  void          HASH_Initalize_MB(const int MB);
  HASH_Entry    HASH_Get  (U64 posKey);
  void          HASH_Store(U64 posKey, int cMove, CutOffState bound, int score, int depth, int ply);
  U64           HASH_Size();
  void          HASH_Prefetch(U64 posKey);

  void            pHASH_Clear();
  pawn_HASH_Entry pHASH_Get(U64 posKey);
  void            pHASH_Store(U64 posKey, U64, U64, int score, uint8_t, uint8_t, uint8_t, uint8_t);
  U64             pHASH_Size();

  private:

  HASH_Entry *hashTable;
  U64 TableSize;

  pawn_HASH_Entry *pHASH;
  U64 pTableSize;

};

#endif
