/*
    Drofa - UCI compatable chess engine
        Copyright (C) 2017 - 2019  Rhys Rustad-Elliott
                      2020 - 2023  Litov Alexander
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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
  void            pHASH_Store(U64 posKey, U64, U64, int score);
  U64             pHASH_Size();

  private:

  HASH_Entry *hashTable;
  U64 TableSize;
  U64 TableMask;

  pawn_HASH_Entry *pHASH;
  U64 pTableSize;
  U64 pTableMask;

};

#endif
