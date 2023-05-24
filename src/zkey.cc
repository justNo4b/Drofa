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
#include "zkey.h"
#include "board.h"
#include "bitutils.h"
#include <random>
#include <climits>
#include <iostream>
#include <sstream>

const unsigned int ZKey::PRNG_KEY = 0xDEADBEEF;

U64 ZKey::PIECE_KEYS[2][6][64];
U64 ZKey::CASTLE_RIGHTS_KEYS[64];
U64 ZKey::EN_PASSANT_KEYS[8];

U64 ZKey::PIECE_COUNT_KEY[2][6][11];


U64 ZKey::WHITE_TO_MOVE_KEY;

void ZKey::init() {
  std::mt19937_64 mt(PRNG_KEY);
  std::uniform_int_distribution<U64> dist(ZERO, ULLONG_MAX);

  WHITE_TO_MOVE_KEY = dist(mt);

  for (int file = 0; file < 8; file++) {
    EN_PASSANT_KEYS[file] = dist(mt);
  }

  for (int pieceType = 0; pieceType < 6; pieceType++) {
    for (int square = 0; square < 64; square++) {
      PIECE_KEYS[WHITE][pieceType][square] = dist(mt);
      PIECE_KEYS[BLACK][pieceType][square] = dist(mt);
    }
  }

  for (int pt = 0; pt < 6; pt++){
    for (int num = 0; num < 11; num++){
        PIECE_COUNT_KEY[WHITE][pt][num] = dist(mt);
        PIECE_COUNT_KEY[BLACK][pt][num] = dist(mt);
    }
  }

  for (int square = 0; square < 64; square++){
    CASTLE_RIGHTS_KEYS[square] = dist(mt);
  }

}

ZKey::ZKey() {
  _key = ZERO;
  _enPassantFile = -1;
}

ZKey::ZKey(const Board &board) {
  _key = ZERO;

  if (board.getActivePlayer() == WHITE) {
    _key ^= WHITE_TO_MOVE_KEY;
  }

  PieceType pieces[6] = {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING};

  // Add white/black pieces
  for (auto piece : pieces) {
    U64 whiteBitBoard = board.getPieces(WHITE, piece);
    U64 blackBitBoard = board.getPieces(BLACK, piece);

    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
      U64 square = ONE << squareIndex;
      if (square & whiteBitBoard) {
        flipPiece(WHITE, piece, squareIndex);
      } else if (square & blackBitBoard) {
        flipPiece(BLACK, piece, squareIndex);
      }
    }
  }

  // Add en passant
  if (board.getEnPassant()) {
    _enPassantFile = (_bitscanForward(board.getEnPassant())) % 8;
    _key ^= EN_PASSANT_KEYS[_enPassantFile];
  } else {
    _enPassantFile = -1;
  }

  // Add castles
  U64 castles = board.getCastlingRights();
  while(castles){
    int square = _popLsb(castles);
    flipCRight(square);
  }
}

void ZKey::setFromPawnStructure(const Board &board) {
  _key = ZERO;

  // Add white/black pieces
  U64 whitePawns = board.getPieces(WHITE, PAWN);
  U64 blackPawns = board.getPieces(BLACK, PAWN);
  U64 whiteKing  = board.getPieces(WHITE, KING);
  U64 blackKing  = board.getPieces(BLACK, KING);

  for (unsigned int squareIndex = 0; squareIndex < 64; squareIndex++) {
    U64 square = ONE << squareIndex;
    if (square & whitePawns) {
      flipPiece(WHITE, PAWN, squareIndex);
    } else if (square & blackPawns) {
      flipPiece(BLACK, PAWN, squareIndex);
    } else if (square & whiteKing){
      flipPiece(WHITE, KING, squareIndex);
    }else if (square & blackKing){
      flipPiece(BLACK, KING, squareIndex);
    }
  }
}

void ZKey::setFromPieceCounts(const Board &board) {
    _key = ZERO;
    for (auto pt : {  PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING}){
        U64 blackPiece = board.getPieces(BLACK, pt);
        U64 whitePiece = board.getPieces(WHITE, pt);

        for (int j = 0 ; j <= _popCount(whitePiece); j++){
            flipPieceCount(WHITE, pt, j);
        }
        for (int j = 0; j <= _popCount(blackPiece); j++){
            flipPieceCount(BLACK, pt, j);
        }

    }
}

U64 ZKey::getValue() const {
  return _key;
}

void ZKey::movePiece(Color color, PieceType piece, unsigned int from, unsigned int to) {
  flipPiece(color, piece, from);
  flipPiece(color, piece, to);
}

void ZKey::flipPiece(Color color, PieceType piece, unsigned int index) {
  _key ^= PIECE_KEYS[color][piece][index];
}

void ZKey::flipPieceCount(Color color, PieceType pt, int count){
    _key ^= PIECE_COUNT_KEY[color][pt][count];
}

void ZKey::updateCastlingRights(U64 oldRights, U64 cRights) {
    cRights = oldRights & ~cRights;
    while(cRights){
        int square = _popLsb(cRights);
        flipCRight(square);
    }
}

void ZKey::clearEnPassant() {
  if (_enPassantFile != -1) {
    _key ^= EN_PASSANT_KEYS[_enPassantFile];
    _enPassantFile = -1;
  }
}

void ZKey::setEnPassantFile(unsigned int file) {
  _enPassantFile = file;
  _key ^= EN_PASSANT_KEYS[file];
}

void ZKey::flipActivePlayer() {
  _key ^= WHITE_TO_MOVE_KEY;
}

void ZKey::flipCRight(int square){
    _key ^= CASTLE_RIGHTS_KEYS[square];
}

bool ZKey::operator==(const ZKey &other) {
  return other.getValue() == _key;
}

void ZKey::setpKeyFromString(std::string pseudoFen){
    _key = ZERO;
    int pArray[2][6] = {{0}};
    std::string token;
    // Process string and initilize an array;
    std::istringstream pfStream(pseudoFen);
    pfStream >> token;
    for (auto currChar : token) {
      switch (currChar) {
        case 'p': pArray[BLACK][PAWN]++;
          break;
        case 'r': pArray[BLACK][ROOK]++;
          break;
        case 'n': pArray[BLACK][KNIGHT]++;
          break;
        case 'b': pArray[BLACK][BISHOP]++;
          break;
        case 'q': pArray[BLACK][QUEEN]++;
          break;
        case 'k': pArray[BLACK][KING]++;
          break;
        case 'P': pArray[WHITE][PAWN]++;
          break;
        case 'R': pArray[WHITE][ROOK]++;
          break;
        case 'N': pArray[WHITE][KNIGHT]++;
          break;
        case 'B': pArray[WHITE][BISHOP]++;
          break;
        case 'Q': pArray[WHITE][QUEEN]++;
          break;
        case 'K': pArray[WHITE][KING]++;
          break;
        case '/': break;
        }
    }
    // Set a key
    for (auto pt : {  PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING}){
        for (int j = 0; j <= pArray[WHITE][pt]; j++){
            flipPieceCount(WHITE, pt, j);
        }

        for (int j = 0; j <= pArray[BLACK][pt]; j++){
            flipPieceCount(BLACK, pt, j);
        }
    }

}
