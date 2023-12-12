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
#include "transptable.h"
#include "transptableentry.h"
#include <cstdint>

HASH * myHASH;

HASH::HASH(){

  double hashSize = 16; // когда-нить я сделаю аллокацию через UCI
  double p_Size = 6;

  U64 tableSizeMax = hashSize / (double) sizeof(HASH_Entry) *  0x100000;   // по идее это должно быть 1 МБ
  TableSize = 1;

  while (TableSize / 2 <= tableSizeMax){
    TableSize *= 2;
  }
  hashTable = new HASH_Entry [TableSize];
  HASH_Clear();
  TableMask = TableSize - 1;

  // Initalize pawn hash table for easier score computation
  // Сперва считаем максимальный объём таблицы
  // Потом ближайшее число которое есть степень двойки (ускоряем вычисления id)
  U64 ptableSizeMax = p_Size / (double) sizeof(pawn_HASH_Entry) * 0x100000;
  pTableSize = 1;
  while (pTableSize / 2 <= ptableSizeMax){
    pTableSize *= 2;
  }
  pHASH = new pawn_HASH_Entry[pTableSize];
  pHASH_Clear();

  // save mask
  pTableMask = pTableSize - 1;
}

void  HASH::HASH_Initalize_MB(const int MB){

  //delete previous TT
  delete[] hashTable;

  // set new size
  double hashSize = MB;
  U64 tableSizeMax = hashSize / (double) sizeof(HASH_Entry) *  0x100000;
  TableSize = 1;

  while (TableSize / 2 <= tableSizeMax){
    TableSize *= 2;
  }
  hashTable = new HASH_Entry [TableSize];
  HASH_Clear();
  TableMask = TableSize - 1;
}

U64 HASH::HASH_Size(){
  int k = TableSize;
  return k;
}

void HASH::HASH_Clear(){
    for (U64 i = 0; i < TableSize; i++){
      hashTable [i] = HASH_Entry();
    }
}

void  HASH::HASH_Store(U64 posKey, int cMove, CutOffState bound, int score, int depth, int ply){
      if (abs(score) > WON_IN_X){
        score = (score > 0) ? (score - ply) : (score + ply);
      }

      U64 index = posKey & TableMask;
      if (posKey !=  hashTable[index].posKey || depth * 2 >=  hashTable[index].depth || bound == EXACT){
         hashTable[index] = HASH_Entry(posKey, cMove, (int16_t)score, depth, bound);
      }
}


HASH_Entry  HASH::HASH_Get(U64 posKey){
  U64 index = posKey & TableMask;
  if (hashTable[index].posKey == posKey){
    return  hashTable[index];
  }
  return HASH_Entry();
}

void HASH::HASH_Prefetch(U64 posKey){
  __builtin_prefetch(&hashTable[posKey & TableMask]);
}

U64 HASH::pHASH_Size(){
  return pTableSize;
}

void HASH::pHASH_Clear(){
  for (U64 i = 0; i < pTableSize; i++){
    pHASH[i] = pawn_HASH_Entry();
  }
}

void HASH::pHASH_Store(U64 posKey, U64 whitePassers, U64 blackPassers, int score){
  U64 index = posKey & pTableMask;
  pHASH[index] = pawn_HASH_Entry(posKey, whitePassers, blackPassers, score);

}

pawn_HASH_Entry HASH::pHASH_Get (U64 posKey){
  U64 index = posKey & pTableMask;
  if (pHASH[index].posKey == posKey){
    return pHASH[index];
  }
  return pawn_HASH_Entry();
}