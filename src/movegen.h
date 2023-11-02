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
#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "defs.h"

/**
 * @brief Vector of move objects
 */
typedef std::vector<Move> MoveList;

/**
 * @brief Pseudo-legal move generator.
 */
class MoveGen {
 public:
  /**
   * @brief Constructs a new MoveGen and generates moves for the given board.
   *
   * @param board Board to generate moves for.
   * @param isCaptureGenerated if true, we generate only Captures (we are in QSearch)
   */
  MoveGen(const Board *board, bool isCaptureGenerated);

  /**
   * @brief Constructs a new MoveGen for an empty board.
   */
  MoveGen();

  /**
   * @brief Sets the board for this MoveGen to the specified board and generates moves for it.
   *
   * @param board Board to set and generate moves for
   * @param isCaptureGenerated if true, we generate only Captures (we are in QSearch)
   */
  void setBoard(const Board *board, bool isCaptureGenerated);


  void initCapArray();

  /**
   * @brief Returns all pseudo-legal moves that have been generated for the current board.
   *
   * @return A MoveList of all pseudo-legal moves that have been generated for the current board.
   */
  MoveList * getMoves();

 private:
  /**
   * @brief A vector containing generated pseudo-legal moves
   */
  MoveList _moves;

  /**
   * @brief Size of _moves to pre-reserve before generating moves.
   *
   * At the current time 218 seems to be an upper bound on the maximum number
   * of moves from any one position.
   */
  static const int MOVELIST_RESERVE_SIZE = 218;
  static const int MOVELIST_RESERVE_SIZE_CAPS = 32;

  /**
   * @brief Generates pseudo-legal moves for the active player of the given board
   *
   * Generated pseudo-legal moves are stored in the _moves vector.
   *
   * @param board Board to generate moves for
   */
  void _genMoves(const Board *board);

 /**
   * @brief Generates pseudo-legal captures for the active player of the given board
   * Used in for QSearch move generation
   * Generated pseudo-legal moves are stored in the _moves vector.
   *
   * @param board Board to generate moves for
   */
  void _genCaptures(const Board *board);

  /**
   * @brief Populates the _legalMoves vector with moves from _moves that are legal.
   *
   * @param board Board to check legality of moves with.
   */
  void _genLegalMoves(const Board *board);

  /**
   * @brief Convenience function to generate pawn promotions.
   *
   * Given the from square, to square, extra flags (if applicable) and a captured
   * piece type (if this is a capture promotion), generate the 4 possible promotion moves
   * (queen, rook, bishop, knight).
   *
   * @param from              From square (little endian rank-file mapping)
   * @param to                To square (little endian rank-file mapping)
   * @param flags             Extra move flags (if applicable)
   * @param capturedPieceType Captured piece type (if applicable)
   */
  void _genPawnPromotions(unsigned int, unsigned int, unsigned int= 0, PieceType= PAWN);

  /**
   * @name White pawn pseudo-legal move generation functions
   *
   * These functions generate the four types of pawn moves for white for a given board.
   * @{
   */
  inline void _genPawnMoves(const Board *, Color color);
  inline void _genPawnAttacks(const Board *, Color color);
  inline void _getPromQonly(const Board *, Color color);
  /**@}*/

  /**
   * @name Generalized pseudo-legal generation functions for all pieces execpt pawns
   *
   * Each of these functions take a board reference, a bitboard of pieces to
   * generate moves for and a bitboard of attackable pieces.
   *
   */
  void _genKingMoves(const Board *, Color, U64, U64);
  void _genKnightMoves(const Board *, U64, U64);
  void _genBishopMoves(const Board *, U64, U64);
  void _genRookMoves(const Board *, U64, U64);
  void _genQueenMoves(const Board *, U64, U64);
  /**@}*/

    /**
   * @name Generalized pseudo-legal capture generation functions for all pieces execpt pawns
   *
   * Each of these functions take a board reference, a bitboard of pieces to
   * generate moves for and a bitboard of attackable pieces.
   *
   */
  inline void _genKingCaps(const Board *, U64, U64);
  inline void _genKnightCaps(const Board *, U64, U64);
  inline void _genBishopCaps(const Board *, U64, U64);
  inline void _genRookCaps(const Board *, U64, U64);
  inline void _genQueenCaps(const Board *, U64, U64);
  /**@}*/

  /**
   * @brief Convenience function to add moves from a bitboard of generated moves.
   *
   * Given a board, a from square, a PieceType, a bitboard containing generated
   * moves and a bitboard containing attackable pieces, generate all possible Move
   * objects, and add them to the vector of pseudo-legal moves.
   *
   * @param board      Board to generate moves for
   * @param from       Originating square of moves
   * @param pieceType  Type of piece that is moving
   * @param moves      Bitboard containing possible destination squares
   * @param attackable Bitboard containing attackable pieces for this move
   */
  void _addMoves(const Board *, int, PieceType, U64, U64);

    /**
   * @brief Convenience function to add Captures from a bitboard of generated moves.
   *
   * Given a board, a from square, a PieceType, a bitboard containing generated
   * moves and a bitboard containing attackable pieces, generate all possible Move
   * objects, and add them to the vector of pseudo-legal moves.
   *
   * @param board      Board to generate moves for
   * @param from       Originating square of moves
   * @param pieceType  Type of piece that is moving
   * @param moves      Bitboard containing possible destination squares
   * @param attackable Bitboard containing attackable pieces for this move
   */
  inline void _addCaps(const Board *, int, PieceType, U64, U64);
};

#endif
