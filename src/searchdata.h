#ifndef SEARCHDATA_H
#define SEARCHDATA_H

#include "defs.h"

struct SEARCH_Data
{
    int     statEval  [MAX_INT_PLY];
    int     moves     [MAX_INT_PLY];
    int8_t  ply;
    Color   sideBehind;
    bool    nmpTree;

    SEARCH_Data() : statEval {0}, moves {0}, ply(0), sideBehind(WHITE), nmpTree(false) {};

    void AddEval(int e){
        statEval[ply] = e;
    };

    void AddMove(int m){
        moves[ply] = m;
        ply++;
    };

    void AddNullMove(Color sB){
        moves[ply] = 0;
        ply++;
        sideBehind = sB;
        nmpTree = true;
    };

    void Remove(){
        ply--;
    };

    void RemoveNull(Color sB, bool treeStatus){
        ply--;
        sideBehind = sB;
        nmpTree = treeStatus;
    }

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

  struct pV {
    int     pVmoves [MAX_INT_PLY];
    uint8_t length;

    pV () : pVmoves {0}, length(0) {};
  };


#endif