#include "transptable.h"
#include "transptableentry.h"

HASH * myHASH;

HASH::HASH(){

  // by defaul initialize with 16 MB hash and 6 MB pawn hash
  HASH_Initalize_MB(16);
  HASH_Clear();


  // Initalize pawn hash table for easier score computation
  double p_Size = 6;
  pTableSize = p_Size / (double) sizeof(pawn_HASH_Entry) * 0x100000;

  pHASH = new pawn_HASH_Entry[pTableSize];

  pHASH_Clear();

}

void  HASH::HASH_Initalize_MB(const int MB){

  //delete previous TT
  delete[] hashTable;

  // set new size
  double hashSize = MB;
  U64 max_entries = hashSize / (double) sizeof(HASH_Entry) *  0x100000;
  _tableSize = 1;

  // initilize Hash on the power of two size

  while (_tableSize <= max_entries){
    _tableSize = _tableSize * 2;
  }
  _tableSize = _tableSize / 2;
  _mask = _tableSize - 1;

  // Initalize main hash table for transpositions
  hashTable = new HASH_Entry [_tableSize];
}

U64 HASH::HASH_Size(){
  int k = _tableSize;
  return k;
}

void HASH::HASH_Clear(){
    for (U64 i = 0; i < _tableSize; i++){
      hashTable [i] = HASH_Entry();
    }
}

void  HASH::HASH_Store(U64 posKey, int cMove, CutOffState bound, int score, int depth, int ply){
    if (abs(score) > WON_IN_X){
        score = (score > 0) ? (score - ply) : (score + ply);
    }

    U64 index = posKey & _mask;
    hashTable[index] = HASH_Entry(posKey, cMove, (int16_t)score, depth, bound);

}


HASH_Entry  HASH::HASH_Get(U64 posKey){
  U64 index = posKey & _mask;
  if (hashTable[index].posKey == posKey){
    return  hashTable[index];
  }
  return HASH_Entry();
}

void HASH::HASH_Prefetch(U64 posKey){
  __builtin_prefetch(&hashTable[posKey % _mask]);
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