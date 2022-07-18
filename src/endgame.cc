#include "defs.h"
#include "eval.h"
#include "attacks.h"
#include "movegen.h"
#include "endgame.h"


egEvalEntry myEvalHash [EG_HASH_SIZE];


int Eval::evaluateDraw(){
    return 0;
}

int Eval::evaluateEndgame(const Board &board, Color color){
    return 0;
}


inline void Eval::egHashAdd(U64 key, egEvalFunction ef){
    U64 index = key & (EG_HASH_SIZE - 1);
    myEvalHash[index] = egEvalEntry(key, ef);
}

void Eval::initEG(){
    // Veryfy for a change;

    ZKey test;
    test.setpKeyFromString("k/KB");
    //  kp/K

    egHashAdd(test.getValue(), &evaluateDraw);

    // initiate table with zero entries
    for (int i = 0; i < EG_HASH_SIZE; i++){
        myEvalHash[i] = egEvalEntry();
    }



}