#include "psquaretable.h"
#include "board.h"
#include "eval.h"
#include <algorithm>

int PSquareTable::PIECE_VALUES[2][6][64];

std::vector<int> PSquareTable::_mirrorList(std::vector<int> list) {
  std::reverse(list.begin(), list.end());
  return list;
}

void PSquareTable::_setValues(std::vector<int> list, PieceType pieceType) {
  std::copy(list.begin(), list.end(), PIECE_VALUES[BLACK][pieceType]);

  std::vector<int> mirrored = _mirrorList(list);
  std::copy(mirrored.begin(), mirrored.end(), PIECE_VALUES[WHITE][pieceType]);
}


// это PSQT для чёрных.
// для белых делается их mirror()

void PSquareTable::init() {
  _setValues(std::vector<int>({
    gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),
    gS( 50, 90),  gS( 50, 90),  gS( 50, 90),  gS( 50, 90),  gS( 50, 90),  gS( 50, 90),  gS( 50, 90),  gS( 50, 90),
    gS( 10, 48),  gS( 10, 48),  gS( 20, 48),  gS( 30, 48),  gS( 30, 48),  gS( 20, 48),  gS( 10, 48),  gS( 10, 48),
    gS(  5, 28),  gS(  5, 28),  gS( 10, 28),  gS( 25, 28),  gS( 25, 28),  gS( 10, 28),  gS(  5, 28),  gS(  5, 28),
    gS(  0, 12),  gS(  0, 12),  gS(  0, 12),  gS( 20, 12),  gS( 20, 12),  gS(  0, 12),  gS(  0, 12),  gS(  0, 12),
    gS(  5,  4),  gS(  -5, 4),  gS(-10,  4),  gS(  0,  4),  gS(  0,  4),  gS( -10, 4),  gS( -5,  4),  gS(  5,  4),
    gS(  5,  0),  gS( 10,  0),  gS( 10,  0),  gS(-20,  0),  gS(-20,  0),  gS( 10,  0),  gS( 10,  0),  gS(  5,  0),
    gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0),  gS(  0,  0)
  }), PAWN);

  _setValues(std::vector<int>({
    gS( -15, -15), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS( -15, -15),
    gS( -10, -10), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS( -10, -10),
    gS( -10, -10), gS(  0,  0), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS(  0,  0), gS( -10, -10),
    gS( -10, -10), gS(  7,  7), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS(  7,  7), gS( -10, -10),
    gS( -10, -10), gS(  0,  0), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS(  0,  0), gS( -10, -10),
    gS( -10, -10), gS(  7,  7), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS(  7,  7), gS( -10, -10),
    gS( -10, -10), gS(  0,  0), gS(  0,  0), gS(  5,  5), gS(  5,  5), gS(  0,  0), gS(  0,  0), gS( -10, -10),
    gS( -15, -15), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS( -15, -15)
  }), KNIGHT);


  _setValues(std::vector<int>({
    gS(-15,-15), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-15,-15),
    gS(-10,-10), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(-10,-10),
    gS(-10,-10), gS(  0,  0), gS(  5,  5), gS( 10, 10), gS( 10, 10), gS(  5,  5), gS(  0,  0), gS(-10,-10),
    gS(-10,-10), gS(  5,  5), gS(  5,  5), gS( 10, 10), gS( 10, 10), gS(  5,  5), gS(  5,  5), gS(-10,-10),
    gS(-10,-10), gS(  0,  0), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS(  0,  0), gS(-10,-10),
    gS(-10,-10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS( 10, 10), gS(-10,-10),
    gS(-10,-10), gS( 10, 10), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS( 10, 10), gS(-10,-10),
    gS(-15,-15), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-10,-10), gS(-15,-15)
  }), BISHOP);


  _setValues(std::vector<int>({
    gS(  0,  0),  gS(  0,  0), gS(  5,  5), gS(  7,  7), gS(  7,  7), gS(  5,  5),  gS(  0,  0),  gS(  0,  0),
    gS( 17, 17),  gS( 17, 17), gS( 17, 17), gS( 17, 17), gS( 17, 17), gS( 17, 17),  gS( 17, 17),  gS( 17, 17),
    gS(  0,  0),  gS(  0,  0), gS(  5,  5), gS(  7,  7), gS(  7,  7), gS(  5,  5),  gS(  0,  0),  gS(  0,  0),
    gS(  0,  0),  gS(  0,  0), gS(  5,  5), gS(  7,  7), gS(  7,  7), gS(  5,  5),  gS(  0,  0),  gS(  0,  0),
    gS(  0,  0),  gS(  0,  0), gS(  5,  5), gS(  7,  7), gS(  7,  7), gS(  5,  5),  gS(  0,  0),  gS(  0,  0),
    gS(  0,  0),  gS(  0,  0), gS(  5,  5), gS(  7,  7), gS(  7,  7), gS(  5,  5),  gS(  0,  0),  gS(  0,  0),
    gS(  0,  0),  gS(  0,  0), gS(  5,  5), gS(  7,  7), gS(  7,  7), gS(  5,  5),  gS(  0,  0),  gS(  0,  0),
    gS(  0,  0),  gS(  0,  0), gS(  5,  5), gS(  7,  7), gS(  7,  7), gS(  5,  5),  gS(  0,  0),  gS(  0,  0)
  }), ROOK);


  _setValues(std::vector<int>({
    gS(-20,-20), gS(-10,-10), gS(-10,-10), gS( -5, -5), gS( -5, -5), gS(-10,-10), gS(-10,-10), gS(-20,-20),
    gS(-10,-10), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(-10,-10),
    gS(-10,-10), gS(  0,  0), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  0,  0), gS(-10,-10),
    gS( -5, -5), gS(  0,  0), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  0,  0), gS( -5, -5),
    gS(  0,  0), gS(  0,  0), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  0,  0), gS( -5, -5),
    gS(-10,-10), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  5,  5), gS(  0,  0), gS(-10,-10),
    gS(-10,-10), gS(  0,  0), gS(  5,  5), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(  0,  0), gS(-10,-10),
    gS(-20,-20), gS(-10,-10), gS(-10,-10), gS( -5, -5), gS( -5, -5), gS(-10,-10), gS(-10,-10), gS(-20,-20)
  }), QUEEN);

  _setValues(std::vector<int>({
    gS(-10,-15), gS(-15,-7), gS(-15,-7), gS(-15,-7), gS(-15,-7), gS(-15,-7), gS(-15,-7), gS(-10,-15),
    gS(-10, -7), gS(-15, 0), gS(-15, 0), gS(-15, 0), gS(-15, 0), gS(-15, 0), gS(-15, 0), gS(-10, -7),
    gS(-10, -7), gS(-15, 0), gS(-15, 7), gS(-15, 7), gS(-15, 7), gS(-15, 7), gS(-15, 0), gS(-10, -7),
    gS(-10, -7), gS(-15, 0), gS(-15, 7), gS(-15,15), gS(-15,15), gS(-15, 7), gS(-15, 0), gS(-10, -7),
    gS(-10, -7), gS(-15, 0), gS(-15, 7), gS(-15,15), gS(-15,15), gS(-15, 7), gS(-15, 0), gS(-10, -7),
    gS(-10, -7), gS(-15, 0), gS(-15, 7), gS(-15, 7), gS(-15, 7), gS(-15, 7), gS(-15, 0), gS(-10, -7),
    gS(  0, -7), gS(  0, 0), gS(  0, 0), gS(  0, 0), gS(  0, 0), gS(  0, 0), gS(  0, 0), gS(  0, -7),
    gS(  7,-15), gS( 10,-7), gS(  7,-7), gS(  0,-7), gS(  0,-7), gS(  7,-7), gS( 10,-7), gS(  7,-15)
  }), KING);


}

PSquareTable::PSquareTable() = default;

PSquareTable::PSquareTable(const Board &board) {
  for (auto pieceType : {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING}) {
    U64 whiteBitBoard = board.getPieces(WHITE, pieceType);
    U64 blackBitBoard = board.getPieces(BLACK, pieceType);

    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
      U64 square = ONE << squareIndex;
      if (square & whiteBitBoard) {
        addPiece(WHITE, pieceType, squareIndex);
      } else if (square & blackBitBoard) {
        addPiece(BLACK, pieceType, squareIndex);
      }
    }
  }
}

void PSquareTable::addPiece(Color color, PieceType pieceType, unsigned int square) {
  _scores[color] += PIECE_VALUES[color][pieceType][square];
  _scores[color] += PIECE_VALUES[color][pieceType][square];
}

void PSquareTable::removePiece(Color color, PieceType pieceType, unsigned int square) {
  _scores[color] -= PIECE_VALUES[color][pieceType][square];
  _scores[color] -= PIECE_VALUES[color][pieceType][square];
}

void PSquareTable::movePiece(Color color, PieceType pieceType, unsigned int fromSquare, unsigned int toSquare) {
  removePiece(color, pieceType, fromSquare);
  addPiece(color, pieceType, toSquare);
}

int PSquareTable::getScore(Color color) {
  return _scores[color];
}
