#ifndef POSHISTORY_H
#define POSHISTORY_H

#include "defs.h"


class Poshistory
{
    private:
        U64         _hisKey[MAX_GAME_PLY];
        int         _moves[MAX_GAME_PLY];
        int         _statEval[MAX_GAME_PLY];
        uint16_t    _ply;
        uint16_t    _head;

    public:

        Poshistory();
        void AddNode(U64, int);
        void RemoveLast();
        void UpdateEval(int);
        bool IsRepetitionDraw(U64, int);
        void ZeroingTables();
        void ZeroingPly();

        int GetEval(int);
        int GetMove(int);
        int GetPly();

};


#endif