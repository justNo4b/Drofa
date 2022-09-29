#ifndef MOVEPICKER_H
#define MOVEPICKER_H

#include "defs.h"
#include "board.h"
#include "orderinginfo.h"

#define CAPTURE_BASE_SCORE       (400000)
#define PROMOTION_BASE_SCORE     (300000)
#define KILLER1_BASE_SCORE       (200000)
#define KILLER2_BASE_SCORE       (150000)
#define COUNTER_BASE_SCORE       (50000)
#define QUIET_BASE_SCORE         (0)
#define BAD_CAPTURE_SCORE        (-16384)

/**
 * @brief Abstract base class for an object that picks moves from a move list in an optimal order.
 *
 * Implementations must define hasNext(), which returns true if there are remaining moves
 * to be processed and getNext(), which gets the next move from the provided MoveList.
 *
 * Note that instances of a MovePicker modify their provided MoveList instances in place.
 */
class MovePicker {
 public:
  /**
   * @brief Constructs a new GeneralMovePicker for the given OrderingInfo, Board and MoveList.
   *
   * @param orderingInfo OrderingInfo object containing information about the current state of the search
   * @param board Current board state for all moves in the provided MoveList
   * @param moveList Pointer to the MoveList to pick moves from
   */
  MovePicker(const OrderingInfo *, const Board *, MoveList *, int, Color, int, int);

  /**
   * @brief Returns the next best move from this MovePicker's MoveList.
   *
   * @return Move the next best move from this MovePicker's MoveList
   */
  Move getNext();

  /**
   * @brief Returns true if there are more moves to be picked from this MovePicker's MoveList
   *
   * @return true if there are more moves to be picked from this MovePicker's MoveList, false otherwise.
   */
  bool hasNext() const;

  /**
   * @brief Refresh movepicker to the first entry to start picking moves from the beginning
   *
   * Used in the probcut implementation to avoid double-move-generation
   */
  void refreshPicker();

   private:
  /**
   * @brief List of moves this MovePicker picks from
   */
  MoveList *_moves;

  /**
   * @brief Bonuses applied to specific move types.
   * @{
   */
  static const int CAPTURE_BONUS      = CAPTURE_BASE_SCORE;
  static const int KILLER1_BONUS      = KILLER1_BASE_SCORE;
  static const int KILLER2_BONUS      = KILLER2_BASE_SCORE;
  static const int COUNTERMOVE_BONUS  = COUNTER_BASE_SCORE;
  static const int QUIET_BONUS        = QUIET_BASE_SCORE;
  static const int BAD_CAPTURE        = BAD_CAPTURE_SCORE;
  const int PROMOTION_SORT[6] = {0, 0, 100000, -50000, 300000, 0};
  /**@}*/

  /**
   * @brief Assigns a value to each move in this GeneralMovePicker's MoveList representing desirability
   * in a negamax search.
   */
  void _scoreMoves(const Board *);

  /**
   * @brief Position of the first unpicked move in this GeneralMovePicker's MoveList
   */
  size_t _currHead;

  /**
   * @brief OrderingInfo object containing search related information used by this GeneralMovePicker
   */
  const OrderingInfo *_orderingInfo;

  /**
   * @brief posKey of the current position
   */
  int _hashMove;

  /**
   * @brief Color of the current sideToMove
   */
  Color _color;

  /**
   * @brief Current ply of the search
   */
  int _ply;

  /**
   * @brief Previous move made during the search
   * ie one that lead to the position we are currently in.
   *
   * Used to determine CounterMoves
   */
  int _pMove;
};

#endif