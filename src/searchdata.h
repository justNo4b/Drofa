#ifndef SEARCHDATA_H
#define SEARCHDATA_H

#include "defs.h"

struct SEARCH_Data
{
    int statEval[MAX_INT_PLY];
    int moves   [MAX_INT_PLY];
    int8_t ply;

    SEARCH_Data() : statEval {0}, moves {0}, ply(0) {};

    void AddData(int m, int e){
        statEval[ply] = e;
        moves[ply] = m;
        ply++;
    };

    void Remove(){
        ply--;
    };

};


struct Hist{
    U64         hisKey[MAX_GAME_PLY];
    uint16_t    head;

    Hist() : hisKey {0}, head(0)  {};

    void Add(U64 key){
      hisKey[head] = key;
      head++;
    };

    void Remove(){
      head--;
    };
};


#endif