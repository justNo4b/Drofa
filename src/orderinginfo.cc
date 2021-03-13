#include "orderinginfo.h"
#include <cstring>

OrderingInfo::OrderingInfo() {
  _ply = 0;
  std::memset(_history, 0, sizeof(_history));
  std::memset(_counterMove, 0, sizeof(_counterMove));
  std::memset(_killer1, 0, sizeof(_killer1));
  std::memset(_killer2, 0, sizeof(_killer2));
}

void OrderingInfo::updateCounterMove(Color color, int counteredMove, int counterMove){
  int pType = counteredMove & 0x7;
  int to = (counteredMove >> 15) & 0x3f;
  _counterMove[color][pType][to] = counterMove;
}

int OrderingInfo::getCounterMoveINT(Color color, int pMove) const{
  int type = pMove & 0x7;
  int to = (pMove >> 15) & 0x3f;
  return _counterMove[color][type][to];
}

void OrderingInfo::incrementHistory(Color color, int from, int to, int depth) {
  _history[color][from][to] += depth * depth;
}

void OrderingInfo::decrementHistory(Color color, int from, int to, int depth) {
  _history[color][from][to] -= depth * (depth - 1);
}

int OrderingInfo::getHistory(Color color, int from, int to) const {
  return _history[color][from][to];
}

void OrderingInfo::updateKillers(int ply, Move move) {
  int t = move.getMoveINT();
  if (!(_killer1[ply] == t)){
      _killer2[ply] = _killer1[ply];
      _killer1[ply] = t;
  }

}

int OrderingInfo::getKiller1(int ply) const {
  return _killer1[ply];
}

int OrderingInfo::getKiller2(int ply) const {
  return _killer2[ply];
}