#include "timer.h"
#include <math.h>

Timer::Timer(Color active, int moveNum, int curPlyNum){

    int ourTime = _limits.time[active];
    //int opponentTime = _limits.time[_initialBoard.getInactivePlayer()];
    int ourIncrement = _limits.increment[active];

    int tWidth_a = 30;
    int tWidth = 175;
    int tMove = 20;
    int criticalMove = 28;

    double tCoefficient = 0;

    // Divide up the remaining time (If movestogo not specified we are in
    // sudden death)
    if (_limits.movesToGo == 0) {
      tCoefficient = 10 * (tWidth_a / pow((tWidth + pow((moveNum - tMove), 2)), 1.5));
      _timeAllocated = ourTime * tCoefficient;
      if (moveNum > criticalMove) _timeAllocated = ourTime / 10 + ourIncrement;
      _timeAllocated = std::min(_timeAllocated, ourTime + ourIncrement - 10);
    } else {
      // when movetogo is specified, use different coefficients
      tWidth_a = 75;
      tWidth = 200;
      tMove = 35;
      criticalMove = 20;

      tCoefficient = 10 * (tWidth_a / pow((tWidth + pow((moveNum - tMove), 2)), 1.5));
      _timeAllocated = ourTime * tCoefficient;
      if (moveNum > criticalMove) _timeAllocated = ourTime / 10 + ourIncrement;
      _timeAllocated = std::min(_timeAllocated, ourTime + ourIncrement - 10);
    }

    // Depth is infinity in a timed search (ends when time runs out)
    //_searchDepth = MAX_SEARCH_DEPTH;
    _ourTimeLeft = ourTime - _timeAllocated;
}

bool Timer::checkLimits(U64 nodes) {

  if (--_limitCheckCount > 0) {
    return false;
  }

  _limitCheckCount = 2048;

  int elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();

  if (_limits.nodes != 0 && (nodes >= _limits.nodes)) return true;

  // when searching at a time control we will try to use time efficiatnly.
  // If we already started the search, but it took way longer than expected
  // we actually do not want to lose all of our effort
  // So we check if we have enough time to actually finish it
  // If we have much time left, we will allocate some more
  // time to finish search and set a flag that search was prolonged
  // so we didnt prolong it again.

  if (_wasThoughtProlonged && elapsed >= (_timeAllocated)){
    return true;
  } else  if (elapsed >= (_timeAllocated)){

    // if we have so much time left that we supposedly
    // can search last ply ~25 times at least
    // we can prolong thought here.
    if (_ourTimeLeft > _lastPlyTime * 20 + 30 ){
      _timeAllocated += _lastPlyTime * 2;
      _wasThoughtProlonged = true;
      return false;
    }else{
      return true;
    }

  }

  return false;
}
