#ifndef MOVEPICKER_H
#define MOVEPICKER_H

#include "defs.h"
#include "board.h"
#include "orderinginfo.h"

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
  MovePicker(const OrderingInfo *, MoveList *, U64, Color, int, int);

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
   * @brief Initializes constants used in picking moves.
   * 
   * Currently this initialized the internal _mvvLvaTable array only.
   */
  static void init();
 protected:
  /**
   * @brief List of moves this MovePicker picks from
   */
  MoveList *_moves;

  /**
   * @brief Table mapping [victimValue][attackerValue] to an integer represnting move desirability
   * according to MVV/LVA.
   */
  int _mvvLvaTable[5][6] = {
            [PAWN] = {
              [PAWN]    = 5,
              [ROOK]    = 2,
              [KNIGHT]  = 4,
              [BISHOP]  = 3,
              [QUEEN]   = 1,
              [KING]    = 0
            },
            [ROOK] = {
              [PAWN]    = 23,
              [ROOK]    = 20,
              [KNIGHT]  = 22,
              [BISHOP]  = 21,
              [QUEEN]   = 19,
              [KING]    = 18
            },
            [KNIGHT] = {
              [PAWN]    = 11,
              [ROOK]    = 8,
              [KNIGHT]  = 10,
              [BISHOP]  = 9,
              [QUEEN]   = 7,
              [KING]    = 6
            },
            [BISHOP] = {
              [PAWN]    = 17,
              [ROOK]    = 14,
              [KNIGHT]  = 16,
              [BISHOP]  = 15,
              [QUEEN]   = 13,
              [KING]    = 12
            },
            [QUEEN] = {
              [PAWN]    = 29,
              [ROOK]    = 26,
              [KNIGHT]  = 28,
              [BISHOP]  = 27,
              [QUEEN]   = 25,
              [KING]    = 24
            }  
  };

  /**
   * @brief Bonuses applied to specific move types.
   * @{
   */
  static const int CAPTURE_BONUS      = 400000;
  static const int PROMOTION_BONUS    = 300000;
  static const int KILLER1_BONUS      = 200000;
  static const int KILLER2_BONUS      = 150000;
  static const int COUNTERMOVE_BONUS  = 50000;
  static const int QUIET_BONUS        = 0;
  /**@}*/

   private:
  /**
   * @brief Assigns a value to each move in this GeneralMovePicker's MoveList representing desirability
   * in a negamax search.
   */
  void _scoreMoves();

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
  U64 _posKey;

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