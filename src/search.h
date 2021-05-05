#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "defs.h"
#include "movegen.h"
#include "transptable.h"
#include "orderinginfo.h"
#include <chrono>
#include <atomic>

/**
 * @brief Represents a search through a minmax tree.
 *
 * Searches are performed using recursive alpha-beta search (invoked initially
 * through a call to the _rootMax() method, which in turn calls _negaMax().
 * This search is followed by a quiescence search at leaf nodes.
 */
class Search {
 public:
  /**
   * @brief Represents limits imposed on a search through the UCI protocol.
   */
  struct Limits {
    /**
     * @brief Constructs a new Limits struct with all numerical limits set to 0.
     */
    Limits() : depth(0), infinite(false), nodes(0), movesToGo(0), moveTime(0), time{}, increment{} {};

    /**
     * @brief Maximum depth to search to
     */
    int depth;

    /**
     * @brief If true, don't stop the search until the stop flag is set
     */
    bool infinite;

    /**
     * @brief Maximum number of nodes to search
     */
    U64 nodes;

    /**
     * @brief Moves left to the next time control
     */
    int movesToGo;

    /**
     * @brief If nonzero, search exactly this number of milliseconds
     */
    int moveTime;

    /**
     * @brief Array indexed by [color] of time left on the clock for black and white.
     */
    int time[2];

    /**
     * @brief Array indexed by [color] of increment per move for black and white.
     */
    int increment[2];
  };

  struct pV {
    int     pVmoves [100];
    uint8_t length;

    pV () : pVmoves {0}, length(0) {};
  };

  /**
   * @brief Constructs a new Search for the given board.
   *
   * @param board The board to search
   * @param limits limits imposed on this search
   * @param positionHistory Vector of ZKeys reprenting all positions that have
   * occurred in the game
   * @param logUci If logUci is set, UCI info commands about the search will be printed
   * to standard output in real time.k
   */
  Search(const Board &, Limits, Hist, bool= true);

  /**
   * @brief Performs an iterative deepening search within the constraints of the given limits.
   */
  void iterDeep();

  /**
   * @brief Returns the best move obtained through the last search performed.
   * @return The best move obtained through the last search
   */
  Move getBestMove();

  int getBestScore();

  /**
   * @brief Instructs this Search to stop as soon as possible.
   */
  void stop();

  /**
   * 
   */ 
  int getNodes();

 private:

  /**
   * @brief Array of reductions applied to the branch during 
   * LATE MOVE REDUCTION during AB-search
   */ 
  int _lmr_R_array[34][34];

  /**
   * @brief Array of the pre-calculated move-nums
   * used for LATE MOVE PRUNING during AB-search
   */
  int _lmp_Array[100][2]; 

  /**
   * @brief Default depth to search to if no limits are specified.
   */
  static const int DEFAULT_SEARCH_DEPTH = 15;

  /**
   * @brief Estimated number of moves left in the game when in sudden death
   * that the Search class uses to calculate the time allocated to a sudden
   * death search.
   */
  static const int SUDDEN_DEATH_MOVESTOGO = 10;

  /**
   * @brief Maximum depth to search to if depth is not explicitly specified
   * and time limits are imposed.
   */
  static const int MAX_SEARCH_DEPTH = 50;


  //
  int _selDepth = 0; // int that is showing maxDepth with extentions we reached in the search
  //

  //search_constants
  //
  const int NULL_MOVE_REDUCTION = 3;
  const int DELTA_MOVE_CONST = 200;
  const int FUTIL_MOVE_CONST = 150;
  const int REVF_MOVE_CONST = 200;
  const int RAZORING_MARGIN = 650;
  //

  /**
   * @brief Vector of ZKeys for each position that has occurred in the game
   * 
   * This is used to detect threefold repetitions.
   */
  Hist  _posHist;

  /**
   * @brief Array of int, constitutes history of the static eval
   * 
   * This is used for calculating "improving" paramenter 
   * during the search
   */ 
  int _sEvalArray[100];

  /**
   * @brief OrderingInfo object containing information about the current state
   * of this search
   */
  OrderingInfo _orderingInfo;

  /**
   * @brief Limits object representing limits imposed on this search.
   * 
   */
  Limits _limits;

  /**
   * @brief Initial board being used in this search.
   */
  Board _initialBoard;

  /**
   * @brief True if UCI will be logged to standard output during the search.
   */
  bool _logUci;

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
   * @brief Depth of this search in plys
   */
  int _searchDepth;

  int _curMaxDepth;

  /**
   * @brief If this flag is set, calls to _negaMax() and _rootMax() will end as soon
   * as possible and calls to _rootMax will not set the best move and best score.
   */
  std::atomic<bool> _stop;

  /**
   * @brief time_point object representing the exact moment this search was started.
   */
  std::chrono::time_point<std::chrono::steady_clock> _start;

  /**
   * @brief Returns True if this search has exceeded its given limits 
   * 
   * Note that to avoid a needless amount of computation, limits are only
   * checked every 4096 calls to _checkLimits() (using the Search::_limitCheckCount property).
   * If Search::_limitCheckCount is not 0, false will be returned.
   * 
   * @return True if this search has exceed its limits, true otherwise
   */
  bool _checkLimits();

  /**
   * @brief Number of calls remaining to _checkLimits()
   */
  int _limitCheckCount;

  /**
   * @brief Number of nodes searched in the last search.
   */
  U64 _nodes;

  /**
   * @brief Best move found on last search.
   */
  Move _bestMove;

  /**
   * @brief Score corresponding to _bestMove
   */
  int _bestScore;

  /**
   * 
   * 
   */
  pV _ourPV; 

  /**
   * @brief updating heuristics when alpha cut occured 
   *
   * @param move  Move that caused cut
   * @param color moving player
   * @param depth search depth
   */
  inline void _updateAlpha(const Move move, Color Color, int depth, int pMove);

  /**
   * @brief updating heuristics when beta cut occured 
   *
   * @param move Move that caused cut
   * @param color moving player
   * @param pMove previous move
   * @param ply   search ply
   * @param depth search depth
   */
  inline void _updateBeta(const Move move, Color color, int pMove, int ply, int depth);

  inline bool _isRepetitionDraw(U64);

  /**
   * @brief Root negamax function.
   *
   * Starts performing a search to the given depth using recursive minimax
   * with alpha-beta pruning.
   *
   * @param board Board to search through
   * @param depth Depth to search to
   */
  int _rootMax(const Board &, int, int, int, int);

  /**
   * @brief Non root negamax function, should only be called by _rootMax()
   *
   *
   * @param  board Board to search
   * @param  depth Plys remaining to search
   * @param  alpha Alpha value
   * @param  beta  Beta value
   * @param  ply   ply value (distance from root)
   * @param  doNool was last move a null one
   * @param  int    int of the last move that was made
   * @return The score of the given board
   */
  int _negaMax(const Board &, pV *myPV, int, int, int, int, bool, int);

  /**
   * @brief Performs a quiescence search
   *
   * _qSearch only takes into account captures (checks, promotions are not
   * considered)
   *
   * @param  board Board to perform a quiescence search on
   * @param  alpha Alpha value
   * @param  beta  Beta value
   * @param  ply   ply value (distance from root)
   * @return The score of the given board
   */
  int _qSearch(const Board &, int= -INF, int= INF, int = 0);

  /**
   * @brief Logs info about a search according to the UCI protocol.
   *
   * @param pv        MoveList representing the Principal Variation (first moves at index 0)
   * @param depth     Depth of search
   * @param bestScore Score corresponding to the best move
   * @param nodes     Number of nodes searched
   * @param elapsed   Time taken to complete the search in milliseconds
   */
  void _logUciInfo(const MoveList &, int, int, U64, int);

  /**
   * @brief Returns the principal variation for the last performed search.
   * 
   * Internally, this method probes the transposition table for the PV of the last
   * performed search.
   * 
   * @return MoveList The principal variation for the last performed search
   */
  MoveList _getPv();

  /**
   * @brief this function calculates reductions values and stores
   * it in the _lmr_R_array
   */ 
  void init_LMR_array();

  void _setupTimer(const Board &, int);
};

#endif
