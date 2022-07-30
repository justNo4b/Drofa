#include "transptable.h"
#include "transptableentry.h"

HASH * myHASH;

HASH::HASH(){

  double hashSize = 16; // когда-нить я сделаю аллокацию через UCI
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

void  HASH::HASH_Initalize_MB(const int MB){

  //delete previous TT
  delete[] hashTable;

  // set new size
  double hashSize = MB;
  TableSize = hashSize / (double) sizeof(HASH_Entry) *  0x100000;

  // Initalize main hash table for transpositions
  TableSize = TableSize - 2;
  hashTable = new HASH_Entry [TableSize];
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

      U64 index = posKey % TableSize;
      if (posKey !=  hashTable[index].posKey || depth + 4 >=  hashTable[index].depth || bound == EXACT){
         hashTable[index] = HASH_Entry(posKey, cMove, (int16_t)score, depth, bound);
      }
}


HASH_Entry  HASH::HASH_Get(U64 posKey){
  U64 index = posKey % TableSize;
  if (hashTable[index].posKey == posKey){
    return  hashTable[index];
  }
  return HASH_Entry();
}

void HASH::HASH_Prefetch(U64 posKey){
  __builtin_prefetch(&hashTable[posKey % TableSize]);
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
  U64 index = posKey % pTableSize;
  if (index < pTableSize){
    pHASH[index] = pawn_HASH_Entry(posKey, whitePassers, blackPassers, score);
  }
}

pawn_HASH_Entry HASH::pHASH_Get (U64 posKey){
  U64 index = posKey % pTableSize;
  if (pHASH[index].posKey == posKey){
    return pHASH[index];
  }
  return pawn_HASH_Entry();
}