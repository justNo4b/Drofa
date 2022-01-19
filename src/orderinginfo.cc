#include "orderinginfo.h"
#include <cstring>

OrderingInfo::OrderingInfo() {
  clearAllHistory();
}

void OrderingInfo::clearAllHistory(){
  std::memset(_history, 0, sizeof(_history));
  std::memset(_captureHistory, 0, sizeof(_captureHistory));
  std::memset(_counterMove, 0, sizeof(_counterMove));
  std::memset(_killer1, 0, sizeof(_killer1));
  std::memset(_killer2, 0, sizeof(_killer2));
}

void OrderingInfo::clearKillers(){
  std::memset(_killer1, 0, sizeof(_killer1));
  std::memset(_killer2, 0, sizeof(_killer2));
}

void OrderingInfo::clearChildrenKillers(int ply){
  _killer1[ply + 2] = 0;
  _killer2[ply + 2] = 0;
}

void OrderingInfo::updateCounterMove(Color color, int pType, int pTo, int counterMove){
  _counterMove[color][pType][pTo] = counterMove;
}

int OrderingInfo::getCounterMoveINT(Color color, int pType, int pTo) const{
  return _counterMove[color][pType][pTo];
}

// currently use formula clamps history between (-16384 and 16384)
void OrderingInfo::incrementHistory(Color color, int from, int to, int depth) {
  int16_t current = _history[color][from][to];
  int16_t bonus   = depth * depth;
  _history[color][from][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::decrementHistory(Color color, int from, int to, int depth) {
  int16_t current = _history[color][from][to];
  int16_t bonus   = -1 * depth * (depth - 1);
  _history[color][from][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::incrementCapHistory(PieceType capturingPiece, PieceType capturedPiece, int to, int depth){
  int16_t current = _captureHistory[capturingPiece][capturedPiece][to];
  int16_t bonus   = depth * depth;
  _captureHistory[capturingPiece][capturedPiece][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::decrementCapHistory(PieceType capturingPiece, PieceType capturedPiece, int to, int depth){
  int16_t current = _captureHistory[capturingPiece][capturedPiece][to];
  int16_t bonus   = -1 * depth * depth;
  _captureHistory[capturingPiece][capturedPiece][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::incrementCounterHistory(Color color, int pTypeCoord, int pTo, int to, int depth){
  int16_t current = _counterMoveHistory[color][pTypeCoord][pTo][to];
  int16_t bonus   = depth * depth;
  _counterMoveHistory[color][pTypeCoord][pTo][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::decrementCounterHistory(Color color, int pTypeCoord, int pTo, int to, int depth){
  int16_t current = _counterMoveHistory[color][pTypeCoord][pTo][to];
  int16_t bonus   = -1 * depth * depth;
  _counterMoveHistory[color][pTypeCoord][pTo][to] += 32 * bonus - current * abs(bonus) / 512;
}

int OrderingInfo::getHistory(Color color, int from, int to) const {
  return _history[color][from][to];
}

int OrderingInfo::getCaptureHistory(PieceType capturingPiece, PieceType capturedPiece, int to) const{
  return _captureHistory[capturingPiece][capturedPiece][to];
}

int OrderingInfo::getCountermoveHistory(Color color, int pTypeCoord, int pTo, int to) const{
  return _counterMoveHistory[color][pTypeCoord][pTo][to];
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