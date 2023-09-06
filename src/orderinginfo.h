/*
    Drofa - UCI compatable chess engine
        Copyright (C) 2017 - 2019  Rhys Rustad-Elliott
                      2020 - 2023  Litov Alexander
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef ORDERINGINFO_H
#define ORDERINGINFO_H

#include "transptable.h"
#include "defs.h"
#include "movegen.h"
#include "move.h"
#include <cstdint>

#define cmhCalculateIndex(moveInt) (((moveInt & 0x7) + ((moveInt >> 15) & 0x3f) * 6))

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
   * @brief clear all existing history scores
   */
  void clearAllHistory();

  /**
   * @brief clear killers
   */
  void clearKillers();

  /**
   * @brief clear killers and counters
   */
  void clearChildrenKillers(int);

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

  void incrementCapHistory(PieceType, PieceType, int, int);

  void decrementCapHistory(PieceType, PieceType, int, int);

  void incrementCounterHistory(Color, int, PieceType, int, int);

  void decrementCounterHistory(Color, int, PieceType, int, int);

  /**
   * @brief Update countermove.
   *
   * @param color Color of the current sideToMove
   * @param counteredMove Previous move made in the Search
   * @param counterMove Move that rejected counteredMove
   *
   */
  void updateCounterMove(Color, int , int);

  /**
   * @brief Gets CounterMove using last move made in the position as info
   *
   * @param color Color of the current sideToMove
   * @param pMove Previous move made in the Search
   */
  int getCounterMoveINT(Color, int) const;

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
   * @brief Get history information for the current capture move
   *
   * @param capuringPiece   piece that is capturing
   * @param capturedPiece   piece that getting capured
   * @param to              move to location
   */
  int getCaptureHistory(PieceType, PieceType, int) const;

  /**
   * @brief Get CounterMoveHistory for the move
   *
   * @param prevMove  previous move made
   * @param pType     moving piece type
   * @param to        move to
   */
  int getCountermoveHistory(Color, int, PieceType, int) const;

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
  int getKiller1(int) const;

  /**
   * @brief Get the second killer move for the given ply.
   *
   * @param ply Ply to get killer move for
   * @return Move Second killer move at the given ply
   */
  int getKiller2(int) const;

 private:

  /**
   * @brief Array of first killer moves by ply
   */
  int _killer1[MAX_INT_PLY];

  /**
   * @brief Array of second killer moves by ply
   */
  int _killer2[MAX_INT_PLY];

  /**
   * @brief Table of beta-cutoff history values indexed by [color][from_square][to_square]
   */
  int16_t _history[2][64][64];

  /**
   * @brief Table of beta-cutoff values for captures indexed by [capturingPiece][capturedPiece][to_square]
   */
  int16_t _captureHistory[6][6][64];

  /**
   * @brief Table of beta-cutoff values dependand on the previous move by opponent
   * Indexed by [prevPieceType][prevTo][movePieceType][moveTo]
   */
  int16_t _counterMoveHistory[2][6 * 64][6][64];

  /**
   * @brief Array of the moves (represented by their INT), that counter move
   * made on the previous ply (cause beta - cutoff)
   * Indexed by [OppositeColor][PieceType][to_square] of the move they countered
   */
  int _counterMove[2][6][64];
};

#endif