#include "psquaretable.h"
#include "board.h"
#include "eval.h"
#include <algorithm>

int PSquareTable::PIECE_VALUES[2][6][64];
int PSquareTable::PAWN_ADJUSTMENTS[2][2][64];

// это PSQT для чёрных.
// для белых делается их mirror()

void PSquareTable::init() {

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
}

void PSquareTable::removePiece(Color color, PieceType pieceType, unsigned int square) {
}

void PSquareTable::movePiece(Color color, PieceType pieceType, unsigned int fromSquare, unsigned int toSquare) {

}

int PSquareTable::getScore(Color color) {
  return _scores[color];
}

int PSquareTable::getPawnAdjustment(Color color, int type) {
  return _pawnScores[color][type];
}

int PSquareTable::getPsqtValue(Color color, PieceType pieceType, int sqv){
  return PIECE_VALUES[color][pieceType][sqv];
}