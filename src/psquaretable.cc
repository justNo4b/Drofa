#include "psquaretable.h"
#include "board.h"
#include <algorithm>

int PSquareTable::PIECE_VALUES[2][2][6][64];

std::vector<int> PSquareTable::_mirrorList(std::vector<int> list) {
  std::reverse(list.begin(), list.end());
  return list;
}

void PSquareTable::_setValues(std::vector<int> list, PieceType pieceType, GamePhase phase) {
  std::copy(list.begin(), list.end(), PIECE_VALUES[phase][BLACK][pieceType]);

  std::vector<int> mirrored = _mirrorList(list);
  std::copy(mirrored.begin(), mirrored.end(), PIECE_VALUES[phase][WHITE][pieceType]);
}


// это PSQT для чёрных.
// для белых делается их mirror()

void PSquareTable::init() {
  _setValues(std::vector<int>({
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
  }), PAWN, OPENING);

  _setValues(std::vector<int>({
    -15,-10,-10,-10,-10,-10,-10,-15,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  7, 10, 10, 10, 10,  7,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  7, 10, 10, 10, 10,  7,-10,
    -10,  0,  0,  5,  5,  0,  0,-10,
    -15,-10,-10,-10,-10,-10,-10,-15
  }), KNIGHT, OPENING);

  _setValues(std::vector<int>({
    -15,-10,-10,-10,-10,-10,-10,-15,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10, 10,  0,  0,  0,  0, 10,-10,
    -15,-10,-10,-10,-10,-10,-10,-15
  }), BISHOP, OPENING);

  _setValues(std::vector<int>({
    0,  0,  5,  7,  7,  5,  0,  0,
   17, 17, 17, 17, 17, 17, 17, 17,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0
  }), ROOK, OPENING);

  _setValues(std::vector<int>({
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
  }), QUEEN, OPENING);

  _setValues(std::vector<int>({
    -10,-15,-15,-15,-15,-15,-15,-10,
    -10,-15,-15,-15,-15,-15,-15,-10,
    -10,-15,-15,-15,-15,-15,-15,-10,
    -10,-15,-15,-15,-15,-15,-15,-10,
    -10,-15,-15,-15,-15,-15,-15,-10,
    -10,-15,-15,-15,-15,-15,-15,-10,
      0,  0,  0,  0,  0,  0,  0,  0,
      7, 10,  7,  0,  0,  7, 10,  7
  }), KING, OPENING);

  // ENDGAME
  _setValues(std::vector<int>({
     0,   0,  0,  0,  0,  0,  0,  0,
     90, 90, 90, 90, 90, 90, 90, 90,
     48, 48, 48, 48, 48, 48, 48, 48,
     28, 28, 28, 28, 28, 28, 28, 28,
     12, 12, 12, 12, 12, 12, 12, 12,
      4,  4,  4,  4,  4,  4,  4,  4,
      0,  0,- 0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0
  }), PAWN, ENDGAME);

  _setValues(std::vector<int>({
    -15,-10,-10,-10,-10,-10,-10,-15,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  7, 10, 10, 10, 10,  7,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  7, 10, 10, 10, 10,  7,-10,
    -10,  0,  0,  5,  5,  0,  0,-10,
    -15,-10,-10,-10,-10,-10,-10,-15
  }), KNIGHT, ENDGAME);

  _setValues(std::vector<int>({
    -15,-10,-10,-10,-10,-10,-10,-15,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10, 10,  0,  0,  0,  0, 10,-10,
    -15,-10,-10,-10,-10,-10,-10,-15
  }), BISHOP, ENDGAME);

  _setValues(std::vector<int>({
    0,  0,  5,  7,  7,  5,  0,  0,
   17, 17, 17, 17, 17, 17, 17, 17,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0,
    0,  0,  5,  7,  7,  5,  0,  0
  }), ROOK, ENDGAME);

  _setValues(std::vector<int>({
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
  }), QUEEN, ENDGAME);

  _setValues(std::vector<int>({
    -15, -7, -7, -7, -7, -7, -7,-15,
     -7,  0,  0,  0,  0,  0,  0, -7,
     -7,  0,  7,  7,  7,  7,  0, -7,
     -7,  0,  7, 15, 15,  7,  0, -7,
     -7,  0,  7, 15, 15,  7,  0, -7,
     -7,  0,  7,  7,  7,  7,  0, -7,
     -7,  0,  0,  0,  0,  0,  0, -7,
    -15, -7, -7, -7, -7, -7, -7,-15
  }), KING, ENDGAME);
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
  _scores[OPENING][color] += PIECE_VALUES[OPENING][color][pieceType][square];
  _scores[ENDGAME][color] += PIECE_VALUES[ENDGAME][color][pieceType][square];
}

void PSquareTable::removePiece(Color color, PieceType pieceType, unsigned int square) {
  _scores[OPENING][color] -= PIECE_VALUES[OPENING][color][pieceType][square];
  _scores[ENDGAME][color] -= PIECE_VALUES[ENDGAME][color][pieceType][square];
}

void PSquareTable::movePiece(Color color, PieceType pieceType, unsigned int fromSquare, unsigned int toSquare) {
  removePiece(color, pieceType, fromSquare);
  addPiece(color, pieceType, toSquare);
}

int PSquareTable::getScore(GamePhase phase, Color color) {
  return _scores[phase][color];
}
