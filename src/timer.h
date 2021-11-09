#ifndef TIMER_H
#define TIMER_H

#include "defs.h"
#include <chrono>


  /**
   * @brief Represents limits imposed on a search through the UCI protocol.
   */
  struct Limits {

    int depth;
    bool infinite;
    U64 nodes;
    int movesToGo;
    int moveTime;

    int time[2];
    int increment[2];

    Limits() : depth(0), infinite(false), nodes(0), movesToGo(0), moveTime(0), time{0}, increment{0} {};

  };



class Timer
{

public:



    Timer(Color, int, int);

    /**
   * @brief Returns True if this search has exceeded its given limits
   *
   * Note that to avoid a needless amount of computation, limits are only
   * checked every 4096 calls to _checkLimits() (using the Search::_limitCheckCount property).
   * If Search::_limitCheckCount is not 0, false will be returned.
   *
   * @return True if this search has exceed its limits, true otherwise
   */
  bool checkLimits(U64);

private:
    /**
     * @brief Limits object representing limits imposed on this search.
    */
  Limits _limits;

  /**
   * @brief time_point object representing the exact moment this search was started.
   */
  std::chrono::time_point<std::chrono::steady_clock> _start;

  /**
   * @brief Number of calls remaining to _checkLimits()
   */
  int _limitCheckCount;

    /**
   * @brief Time allocated for this search in ms
   */
  int _timeAllocated;

  /**
   * @brief This variable holds value of how much time left on our
   * clock. If it is too low, we do not prolong search.
   *
   */
  int _ourTimeLeft;

  /**
   * @brief We keep track of times we prolonged thought
   * during the search. It is important to not prolong a more
   * than one in a row in order not to lose on time.
   *
   */
  bool _wasThoughtProlonged;


  /**
   *  @brief We track how much time we spended while
   *  searching last ply. It is used to estimate how much time
   *  we grant engine when search be prolonged.
   */
  int _lastPlyTime;


  /**
   * @brief Estimated number of moves left in the game when in sudden death
   * that the Search class uses to calculate the time allocated to a sudden
   * death search.
   */
  static const int SUDDEN_DEATH_MOVESTOGO = 10;

};

#endif