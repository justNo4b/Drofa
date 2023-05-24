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
#ifndef ZKEY_H
#define ZKEY_H

#include "defs.h"

class Board;

/**
 * @brief Represents a Zobrist key.
 *
 * Zobrist keys are represented as unsigned 64 bit integers (same as bitboards).
 */
class ZKey {
 public:
  /**
   * @brief Constructs a new empty ZKey.
   */
  ZKey();

  /**
   * @brief Constructs a new ZKey from the given board.
   * @param board Board to construct this ZKey from
   */
  ZKey(const Board &board);

  /**
   * @brief Initialize the pseudo-random values used to generate a ZKey.
   *
   * This must be called once prior to using the ZKey class.
   */
  static void init();

  /**
   * @brief Returns The value of this ZKey.
   *
   * @return The value of this ZKey
   */
  U64 getValue() const;

  /**
   * @brief Updates the ZKey such that the given piece has been moved between the given squares.
   *
   * @param color  Color of piece to move
   * @param piece  Type of piece to move
   * @param from   Square to move from
   * @param to     Square to move to
   */
  void movePiece(Color, PieceType, unsigned int, unsigned int);

  /**
   * @brief Flips the value of the piece with the given color and PieceType on the given square in the ZKey.
   *
   * If the piece is set, this method will unset it and vica-versa.
   *
   * @param color  Color of piece to flip
   * @param piece  Type of piece to flip
   * @param index  Square to flip piece on (little endian rank-file mapping)
   */
  void flipPiece(Color, PieceType, unsigned int);

  /**
   * @brief flipping Zkey for a given piece count
   *
   */
  void flipPieceCount(Color, PieceType, int);

  /**
   * @brief Flips the value of the active player in the ZKey
   *
   * If the active color is black, this method changes it to white, and vica-versa.
   */
  void flipActivePlayer();

  void flipCRight(int);

  /**
   * @brief Clears all en passant information in the ZKey.
   */
  void clearEnPassant();

  /**
   * @brief Sets the en passant file to the given file.
   *
   * File numbers are 0 indexed and start at a.
   *
   * @param file file number to set en passant file to
   */
  void setEnPassantFile(unsigned int);

  /**
   * @brief Updates the castling rights in the ZKey.
   */
  void updateCastlingRights(U64, U64);

  /**
   * @brief Sets this ZKey from the given board, taking into account only pawn
   * positions
   */
  void setFromPawnStructure(const Board&);

  /**
   * @brief Sets Zkey from the given board, but only considering piece counts
   *
   */
  void setFromPieceCounts(const Board&);

  /**
   * @brief sets pCount-based ZKey from pseudo-fen
   * needed for EG evaluation initializing
   */
  void setpKeyFromString(const std::string);

  /**
   * @brief Compare Zobrist keys
   *
   * @param other ZKey to compare
   * @return true if the two ZKeys have the same value, false otherwise
   */
  bool operator==(const ZKey &);

 private:
  /**
   * @brief The current value of the Zobrist key.
   */
  U64 _key;

  /**
   * @brief Current en passant file
   */
  int _enPassantFile;

  /**
   * @brief Array indexed by [Color][PieceType][SquareIndex] of pseudo-random
   * values to xor into _key for each color, piece type and square.
   */
  static U64 PIECE_KEYS[2][6][64];

  static U64 CASTLE_RIGHTS_KEYS[64];

  /**
   * @brief Array indexed by [color][PieceType][PieceNumber]
   * Used to form a key for EG_Eval calculations
   */
  static U64 PIECE_COUNT_KEY[2][6][11];

  /**
   * @brief Pseudo-random keys to xor into _key for each en passant file
   */
  static U64 EN_PASSANT_KEYS[8];

  /**
   * @brief Key to xor into _key when it's white's turn to move.
   */
  static U64 WHITE_TO_MOVE_KEY;

  /**
   * @brief Seed used by the PRNG.
   */
  static const unsigned int PRNG_KEY;
};

#endif
