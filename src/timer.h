#ifndef TIMER_H
#define TIMER_H

#include "defs.h"
#include <chrono>


  /**
   * @brief Represents limits imposed on a search through the UCI protocol.
   */
  struct Limits {

    U64  nodes;         // limit search by nodes searched
    int  depth;         // limit search by depth
    int  moveTime;      // limit search by time
    int  movesToGo;     // (for cyclic) - moves till next time addition
    int  time[2];       // time left for black and white
    int  increment[2];   // increment for black and white
    bool infinite;      // ignore limits (infinite search)

    Limits() : nodes(0), depth(0),  moveTime(0), movesToGo(0), time{}, increment{}, infinite(false) {};
  };

  class Timer
  {
  private:

    /**
     * @brief Limits object representing limits imposed on this search.
     *
     */
    Limits _limits;

    /**
     * @brief Time allocated for this search in ms
     *
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
     * @brief Depth of this search in plys
     */
    int _searchDepth;

    /**
     * @brief time_point object representing the exact moment this search was started.
     *
     */
    std::chrono::time_point<std::chrono::steady_clock> _start;

    /**
     * @brief Constants used for time management.
     * General time calculations formula (Where MOVENUM is current move number in the game):
     *
     * movenum <= critical_move:
     *                                        tWidth_a
     *  portion_of_time_used = --------------------------------------
     *                        (tWidth + (movenum - tMove) ^ 2 ) ^ 1.5
     *
     * movenum > critical_move:
     *
     * portion_of_time_used = 0.1
     *
     *
     * We use separate sets of constants cyclic and increment time controls
     *
     * @{
     */
      const int INCR_T_WIDTH_A = 300;
      const int INCR_T_WIDTH   = 175;
      const int INCR_T_MOVE    = 20;
      const int INCR_CRIT_MOVE = 28;

      const int CYCL_T_WIDTH_A = 750;
      const int CYCL_T_WIDTH   = 200;
      const int CYCL_T_MOVE    = 35;
      const int CYCL_CRIT_MOVE = 20;


    /**@}*/


    void _setupTimer(Color, int);

  public:
      Timer(Limits, Color, int);

      bool checkLimits(U64);

      void startIteration();

      bool finishOnThisDepth(int *);

      int getSearchDepth();
  };

#endif