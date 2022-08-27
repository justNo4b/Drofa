#ifndef ENDGAME_H
#define ENDGAME_H

#include "defs.h"


#define EG_HASH_SIZE        (2048)
#define LARGEST_SP_EVAL     (5)

typedef int (*egEvalFunction) (const Board &board, Color color);

enum egEntryType{
    RETURN_SCORE,
    RETURN_SCALE
};

struct egEvalEntry{
    U64            key;
    egEvalFunction eFunction;
    egEntryType    evalType;

    egEvalEntry(): key(0), eFunction(nullptr), evalType(RETURN_SCALE)  {};
    egEvalEntry(U64 k, egEvalFunction ef, egEntryType et): key(k), eFunction(ef), evalType(et) {};
};

// constant scores


const int MIN_MATE_SCORE = 29000;
const int MINIMAL_WON_SCORE      =  7500;
const int EASY_WIN_SCORE         =  9500;
const int CONFIDENT_WIN_SCORE    =  8500;

const int DRAW_ZONE_ABS = 20;
const int DRAW_WITH_ADVANTAGE = 10;




#endif