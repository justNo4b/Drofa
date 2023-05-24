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
#ifndef SEARCHDATA_H
#define SEARCHDATA_H

#include "defs.h"
#include "move.h"

struct SEARCH_Data
{
    int     statEval  [MAX_INT_PLY];
    Move    moves     [MAX_INT_PLY];
    int8_t  ply;
    Color   sideBehind;
    bool    nmpTree;

    SEARCH_Data() : statEval {0}, moves {0}, ply(0), sideBehind(WHITE), nmpTree(false) {};

    void AddEval(int e){
        statEval[ply] = e;
    };

    void AddMove(Move m){
        moves[ply] = m;
        ply++;
    };

    void AddNullMove(Color sB){
        moves[ply] = Move();
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