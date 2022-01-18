#include "timer.h"
#include "math.h"


Timer::Timer(Limits l, Color color, int movenum){
    _limits = l;
    _wasThoughtProlonged = false;
    if (_limits.infinite) { // Infinite search
        _searchDepth = INF;
        _timeAllocated = INF;
    } else if (_limits.depth != 0) { // Depth search
        _searchDepth = _limits.depth;
        _timeAllocated = INF;
    } else if (_limits.moveTime != 0) {
        _searchDepth = MAX_SEARCH_DEPTH;
        _timeAllocated = _limits.moveTime;
    } else if (_limits.time[color] != 0) {
        _setupTimer(color, movenum);
    } else { // No limits specified, use default depth
        _searchDepth = DEFAULT_SEARCH_DEPTH;
        _timeAllocated = INF;
    }
}

void Timer::_setupTimer(Color color, int movenum){
    int ourTime = _limits.time[color];
    //int opponentTime = _limits.time[_initialBoard.getInactivePlayer()];
    int ourIncrement = _limits.increment[color];
    double tCoefficient = 0;

    // Divide up the remaining time (If movestogo not specified we are in
    // sudden death)
    if (_limits.movesToGo == 0) {
      tCoefficient = INCR_T_WIDTH_A / pow((INCR_T_WIDTH + pow((movenum - INCR_T_MOVE), 2)), 1.5);
      _timeAllocated = ourTime * tCoefficient;
      if (movenum > INCR_CRIT_MOVE) _timeAllocated = ourTime / 10 + ourIncrement;
      _timeAllocated = std::min(_timeAllocated, ourTime + ourIncrement - 10);
    } else {
      // when movetogo is specified, use different coefficients

      tCoefficient = CYCL_T_WIDTH_A / pow((CYCL_T_WIDTH + pow((movenum - CYCL_T_MOVE), 2)), 1.5);
      _timeAllocated = ourTime * tCoefficient;
      if (movenum > CYCL_CRIT_MOVE) _timeAllocated = ourTime / 10 + ourIncrement;
      _timeAllocated = std::min(_timeAllocated, ourTime + ourIncrement - 10);
    }

    // Depth is infinity in a timed search (ends when time runs out)
    _searchDepth = MAX_SEARCH_DEPTH;
    _ourTimeLeft = ourTime - _timeAllocated;
}

bool Timer::checkLimits(U64 nodes){
  int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();

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

void Timer::startIteration(){
    _start = std::chrono::steady_clock::now();
    _lastPlyTime = 0;
}

bool Timer::finishOnThisDepth(int * elapsedTime, U64 totalNodes, U64 bestNodes, int bestScore, int bestScoreDelta){
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
    _lastPlyTime =  elapsed - _lastPlyTime;

    double nodesConfidance = bestNodes * 100.0 / totalNodes;
    // clamp coeff between 25 and 75
    // we assume that standart case is about ~50% of nodes go in bestMove
    nodesConfidance = std::max(25.0, nodesConfidance);
    nodesConfidance = std::min(85.0, nodesConfidance);

    double nodesCoeff = 1.0 + (50.0 - nodesConfidance) / 50.0;

    // Calculate how much does score of the best move changed in the iteration
    // If it is changed too much we probably should try and search deeper
    // Dont do it if abs(score) is too high already
    double scoreCoeff = 1.0;
    if (abs(bestScore) < 300){
        bestScoreDelta = std::min(75, bestScoreDelta);
        scoreCoeff = 1.0 + 0.01 * (bestScoreDelta - 15.0);
    }


    *elapsedTime = elapsed;
    if (_wasThoughtProlonged ||  (elapsed >= (_timeAllocated * nodesCoeff * scoreCoeff * 0.5))){
        return true;
    }

    return false;
}

int Timer::getSearchDepth(){
    return _searchDepth;
}