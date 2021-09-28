#include "poshistory.h"
#include <cstring>

Poshistory::Poshistory(){
    ZeroingTables();
}

void Poshistory::ZeroingTables(){
    std::memset(_hisKey, 0, sizeof(_hisKey));
    std::memset(_moves, 0, sizeof(_moves));
    std::memset(_statEval, 0, sizeof(_statEval));
    _ply = 0;
    _head = 0;
}

void Poshistory::AddNode(U64 posKey, int move){
    _hisKey[_head] = posKey;
    _moves[_head]  = move;
    _head++;
    _ply++;
}

void Poshistory::RemoveLast(){
    _head--;
    _ply--;
}

void Poshistory::ZeroingPly(){
    _ply = 0;
}

void Poshistory::UpdateEval(int eval){
    _statEval[_head] = eval;
}

bool Poshistory::IsRepetitionDraw(U64 currKey, int untilFifty){
    for (int i = _head - 2; (i >= 0 || i > _head - 2 - untilFifty); i-=2){
        if (_hisKey[i] == currKey){
            return true;
        }
    }
  return false;
}

int Poshistory::GetEval(int height){
    return _statEval[_head + height];
}

int Poshistory::GetMove(int height){
    return _moves[_head + height];
}

int Poshistory::GetPly(){
    return _ply;
}