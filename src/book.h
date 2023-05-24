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
#ifndef BOOK_H
#define BOOK_H

#include "defs.h"
#include "move.h"
#include "board.h"
#include <vector>
#include <unordered_map>

/**
 * @brief Represents an opening book
 *
 * Opening books are constructed with a path to a file containing PolyGlot
 * book data, and load all moves into memory.
 */
class Book {
 public:
  /**
   * @brief Constructs a new empty opening book
   */
  Book();

  /**
   * @brief Constructs a new opening book using the data from the PolyGlot
   * opening book file at the specified path
   *
   * @param path Path to the PolyGlot opening book (.bin)
   */
  Book(std::string);

  /**
   * @brief Gets a move from the opening book for the given board
   *
   * Moves are returned according to a weighted average of each move's
   * weight parameter in the PolyGlot book data.
   *
   * If no move exists in the book for the given position, a
   * std::logic_error will be thrown.
   *
   * @param board Board to get moves for
   * @return A valid move for the given board from the opening book
   */
  Move getMove(const Board &) const;

  /**
   * @brief Returns true if there is at least one move in this Book for the
   * given board
   *
   * @param board Board to query book for
   * @return true if there is at least one move in this book for the given
   * board, false otherwise
   */
  bool inBook(const Board &) const;

  /**
   * @brief Returns the 64 bit PolyGlot hash of the given board
   *
   * @param board Board to calculate PolyGlot hash of
   * @return The PolyGlot hash of the given board
   */
  static U64 hashBoard(const Board &);

  /**
   * @brief Given a move in PolyGlot form, and the board to which it
   * corresponds, return a Move object representing it
   *
   * @param board Board that move corresponds to
   * @param move A move in PolyGlot form (16 bit packed data)
   * @return A Move object representing the PolyGlot move
   */
  static Move decodeMove(const Board &, unsigned short);

 private:
  /**
   * @brief Values used to calculate PolyGlot hashes
   *
   * @{
   */
  static const U64 RANDOM_PIECE[768];
  static const U64 RANDOM_CASTLE[4];
  static const U64 RANDOM_ENPASSANT[8];
  static const U64 RANDOM_TURN;
  /**@}*/

  /**
   * @brief Mapping of PolyGlot hashes to moves
   *
   * Each move is stored as a pair of 16 bit values, the first being the move
   * (in PolyGlot form), and the second being the weight assigned to the move
   * in the book.
   */
  std::unordered_map<U64, std::vector<std::pair<unsigned short, unsigned short>>> _content;

  /**
   * @brief Reads all moves from the PolyGlot book file at the given path and
   * loads them into this book
   *
   * @param path Path to PolyGlot book file
   */
  void _initMoves(std::string);
};

#endif