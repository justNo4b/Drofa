#include "orderinginfo.h"
#include <cstring>

OrderingInfo::OrderingInfo() {
  _ply = 0;
  std::memset(_history, 0, sizeof(_history));
  std::memset(_counterMove, 0, sizeof(_counterMove));
}

void OrderingInfo::updateCounterMove(Color color, const Move *counteredMove, Move counterMove){
  _counterMove[color][counteredMove->getPieceType()][counteredMove->getTo()] = counterMove.getMoveINT();
}

int OrderingInfo::getCounterMoveINT(Color color, PieceType type, int to) const{
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