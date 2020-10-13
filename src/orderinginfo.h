#ifndef ORDERINGINFO_H
#define ORDERINGINFO_H

#include "transptable.h"
#include "defs.h"
#include "movegen.h"
#include "move.h"

/**
 * @brief Contains information related to a search in progress
 * for move ordering purposes.
 */
class OrderingInfo {
 public:
  /**
   * @brief Construct a new OrderingInfo
   * 
   */
  OrderingInfo();

  /**
   * @brief Increment the ply number of this search by one.
   */
  void incrementPly();

  /**
   * @brief Decrement the ply number of this search by one.
   */
  void deincrementPly();

  /**
   * @brief Get the ply number of this search.
   * 
   * @return The ply number of this search.
   */
  int getPly() const;

  /**
   * @brief Increment the history heuristic value of the board for 
   * the given color, from square, to square and depth.
   * 
   * @param color Color to increment history for
   * @param from From square to increment history for
   * @param to To square to increment history for
   * @param depth Depth of move that caused this increment
   */
  void incrementHistory(Color, int, int, int);

  /**
   * @brief Lower history heuristic value of the board for 
   * the given color, from square, to square and depth.
   * 
   * @param color Color to increment history for
   * @param from From square to increment history for
   * @param to To square to increment history for
   * @param depth Depth of move that caused this increment
   */
  void decrementHistory(Color, int, int, int);

  /**
   * @brief Get beta-cutoff history information for the given color, from square and
   * to square.
   * 
   * @param color Color of side moving
   * @param from From square to get history for
   * @param to To square to get history for
   * @return int History heuristic value for the given from square, to square and color
   */
  int getHistory(Color, int, int) const;

  /**
   * @brief Update the killer moves for the given ply with the given move.
   * 
   * @param ply Ply to update killer moves for
   * @param move Move to update killer moves with
   */
  void updateKillers(int, Move);

  /**
   * @brief Get the first killer move for the given ply.
   * 
   * @param ply Ply to get killer move for
   * @return First killer move at the given ply
   */
  Move getKiller1(int) const;

  /**
   * @brief Get the second killer move for the given ply.
   * 
   * @param ply Ply to get killer move for
   * @return Move Second killer move at the given ply
   */
  Move getKiller2(int) const;

 private:

  /**
   * @brief Array of first killer moves by ply
   */
  Move _killer1[100];

  /**
   * @brief Array of second killer moves by ply
   */
  Move _killer2[100];

  /**
   * @brief Current ply of search
   */
  int _ply;

  /**
   * @brief Table of beta-cutoff history values indexed by [color][from_square][to_square]
   */
  int _history[2][64][64];
};

#endif