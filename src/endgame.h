#ifndef ENDGAME_H
#define ENDGAME_H

#include "defs.h"


#define EG_HASH_SIZE        (2048)
#define LARGEST_SP_EVAL     (5)

typedef int (*egEvalFunction) (const Board &board, Color color);

struct egEvalEntry{
    U64            key;
    egEvalFunction eFunction;

    egEvalEntry(): key(0), eFunction(nullptr)  {};
    egEvalEntry(U64 k, egEvalFunction ef): key(k), eFunction(ef) {};
};

// constant scores


const int MIN_MATE_SCORE = 29000;
const int MINIMAL_WON_SCORE      =  7500;
const int EASY_WIN_SCORE         =  9500;
const int CONFIDENT_WIN_SCORE    =  8500;

const int DRAW_ZONE_ABS = 20;
const int DRAW_WITH_ADVANTAGE = 10;




#endif