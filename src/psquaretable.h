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
#ifndef PSQUARETABLE_H
#define PSQUARETABLE_H

#include "defs.h"

class Board;

/**
 * @brief Represents a Piece Square Table.
 */
class PSquareTable {
 public:
  PSquareTable();

  /**
   * @brief Constructs a new piece square table from the given board.
   *
   * @param board Board to construct this PSquareTable for
   */
  PSquareTable(const Board&);

  /**
   * @brief Initializes PSquareTable square values.
   *
   * This must be called once prior to using a PSquareTable.
   */
  static void init();

  /**
   * @brief Adds a piece at the given square.
   *
   * @param color     Color of piece to add
   * @param pieceType Type of piece to add
   * @param square    Square to add piece at (little endian rank-file mapping)
   */
  void addPiece(Color, PieceType, unsigned int);

  /**
   * @brief Removes a piece from the given square.
   *
   * @param color     Color of piece to remove
   * @param pieceType Type of piece to remove
   * @param square    Square to remove piece from (little endian rank-file mapping)
   */
  void removePiece(Color, PieceType, unsigned int);

  /**
   * @brief Moves a piece between the given squares.
   *
   * @param color      Color of piece to add
   * @param pieceType  Type of piece to add
   * @param fromSquare Square to add piece at (little endian rank-file mapping)
   * @param toSquare   Square to move piece to (little endian rank-file mapping)
   */
  void movePiece(Color, PieceType, unsigned int, unsigned int);

  /**
   * @brief Gets the piece square table score of the given player in the
   * given phase
   *
   * @param phase GamePhase to get score
   * @param  color Color to get score for
   * @return The piece square table score for the given player
   */
  int getScore(Color);

  int getPawnAdjustment(Color, int);

  int getPsqtValue(Color, PieceType, int);

 private:
  /**
   * @brief Array indexed by [Color][PieceType][SquareIndex] of square values for each  piece,
   * square and color.
   */
  static int PIECE_VALUES[2][6][64];

  static int PAWN_ADJUSTMENTS[2][2][64];

  /**
   * @brief Array indexed by [Color] of each color's piece square table score.
   */
  int _scores[2] = {0};

  /**
   * @brief Holds an adjustments for a pawn PSQT
   */
  int _pawnScores[2][2] = {0};
};

#endif
