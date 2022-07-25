#include "orderinginfo.h"
#include <cstring>

OrderingInfo::OrderingInfo() {
  clearAllHistory();
}

void OrderingInfo::clearAllHistory(){
  std::memset(_history, 0, sizeof(_history));
  std::memset(_captureHistory, 0, sizeof(_captureHistory));
  std::memset(_counterMove, 0, sizeof(_counterMove));
  std::memset(_counterMoveHistory, 0, sizeof(_counterMoveHistory));
  std::memset(_followMoveHistory, 0, sizeof(_followMoveHistory));
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

void OrderingInfo::incrementCounterHistory(Color color, int pMove, PieceType pType, int to, int depth){
  int indx = (pMove & 0x7) + ((pMove >> 15) & 0x3f) * 6;
  int16_t current = _counterMoveHistory[color][indx][pType][to];
  int16_t bonus   = depth * depth;
  _counterMoveHistory[color][indx][pType][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::decrementCounterHistory(Color color, int pMoveIndx, PieceType pType, int to, int depth){
  int16_t current = _counterMoveHistory[color][pMoveIndx][pType][to];
  int16_t bonus   = -1 * depth * depth;
  _counterMoveHistory[color][pMoveIndx][pType][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::incrementFollowHistory(Color color, int ppMove, PieceType pType, int to, int depth){
  int indx = (ppMove & 0x7) + ((ppMove >> 15) & 0x3f) * 6;
  int16_t current = _followMoveHistory[color][indx][pType][to];
  int16_t bonus   = depth * depth;
  _counterMoveHistory[color][indx][pType][to] += 32 * bonus - current * abs(bonus) / 512;
}

void OrderingInfo::decrementFollowHistory(Color color, int pMoveIndx, PieceType pType, int to, int depth){
  int16_t current = _followMoveHistory[color][pMoveIndx][pType][to];
  int16_t bonus   = -1 * depth * depth;
  _counterMoveHistory[color][pMoveIndx][pType][to] += 32 * bonus - current * abs(bonus) / 512;
}

int OrderingInfo::getHistory(Color color, int from, int to) const {
  return _history[color][from][to];
}

int OrderingInfo::getCaptureHistory(PieceType capturingPiece, PieceType capturedPiece, int to) const{
  return _captureHistory[capturingPiece][capturedPiece][to];
}

int OrderingInfo::getCountermoveHistory(Color color, int pMoveIndx, PieceType pType, int to) const{
  return _counterMoveHistory[color][pMoveIndx][pType][to];
}

int OrderingInfo::getFollowmoveHistory(Color color, int ppMoveIndx, PieceType pType, int to) const{
  return _counterMoveHistory[color][ppMoveIndx][pType][to];
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