#include "transptable.h"
#include "transptableentry.h"
#include <iostream>

HASH myHASH;

HASH::HASH(){
  // For now we give 250 MB to the main table and table and 6 MB to the pawn one.
  double hashSize = 250; // когда-нить я сделаю аллокацию через UCI
  double p_Size = 6;

  TableSize = hashSize / (double) sizeof(HASH_Entry) *  0x100000;   // по идее это должно быть 1 МБ

  // Initalize main hash table for transpositions
  TableSize = TableSize - 2;
  hashTable = new HASH_Entry [TableSize];

  HASH_Clear();

  // Initalize pawn hash table for easier score computation

  pTableSize = p_Size / (double) sizeof(pawn_HASH_Entry) * 0x100000;
  pTableSize = pTableSize - 2;
  pHASH = new pawn_HASH_Entry[pTableSize];

  pHASH_Clear();

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
      if (abs(score) + 50 > LOST_SCORE * -1){
        if (score > 0){
          score -= ply;
        }
        if (score < 0){
          score += ply;
        }
      }

      U64 index = posKey % TableSize;
      if (index < TableSize){
        hashTable[index] = HASH_Entry(posKey, cMove, bound, score, depth);
      }
}


HASH_Entry  HASH::HASH_Get(U64 posKey){
  U64 index = posKey % TableSize;
  if (hashTable[index].posKey == posKey){
    return  hashTable[index];
  }
  return HASH_Entry();
}

U64 HASH::pHASH_Size(){
  return pTableSize;
}

void HASH::pHASH_Clear(){
  for (U64 i = 0; i < pTableSize; i++){
    pHASH[i] = pawn_HASH_Entry();
  }
}

void HASH::pHASH_Store(U64 posKey, int END_score, int OP_Score){
  U64 index = posKey % pTableSize;
  if (index < pTableSize){
    pHASH[index] = pawn_HASH_Entry(posKey, END_score, OP_Score);
  }
}

pawn_HASH_Entry HASH::pHASH_Get (U64 posKey){
  U64 index = posKey % pTableSize;
  if (pHASH[index].posKey == posKey){
    return pHASH[index];
  }
  return pawn_HASH_Entry();
}