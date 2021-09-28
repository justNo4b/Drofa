#include "psquaretable.h"
#include "board.h"
#include "eval.h"
#include <algorithm>

int PSquareTable::PIECE_VALUES[2][6][64];
int PSquareTable::PAWN_ADJUSTMENTS[2][2][64];

// это PSQT для чёрных.
// для белых делается их mirror()

void PSquareTable::init() {

    for (int i = 0; i < 64; i++){
      PIECE_VALUES[BLACK][KING][i] = Eval::KING_PSQT_BLACK[i];
      PIECE_VALUES[WHITE][KING][i] = Eval::KING_PSQT_BLACK[_mir(i)];

      PIECE_VALUES[BLACK][PAWN][i] = Eval::PAWN_PSQT_BLACK[i];
      PIECE_VALUES[WHITE][PAWN][i] = Eval::PAWN_PSQT_BLACK[_mir(i)];

      PIECE_VALUES[BLACK][ROOK][i] = Eval::ROOK_PSQT_BLACK[i];
      PIECE_VALUES[WHITE][ROOK][i] = Eval::ROOK_PSQT_BLACK[_mir(i)];

      PIECE_VALUES[BLACK][KNIGHT][i] = Eval::KNIGHT_PSQT_BLACK[i];
      PIECE_VALUES[WHITE][KNIGHT][i] = Eval::KNIGHT_PSQT_BLACK[_mir(i)];

      PIECE_VALUES[BLACK][BISHOP][i] = Eval::BISHOP_PSQT_BLACK[i];
      PIECE_VALUES[WHITE][BISHOP][i] = Eval::BISHOP_PSQT_BLACK[_mir(i)];

      PIECE_VALUES[BLACK][QUEEN][i] = Eval::QUEEN_PSQT_BLACK[i];
      PIECE_VALUES[WHITE][QUEEN][i] = Eval::QUEEN_PSQT_BLACK[_mir(i)];

      PAWN_ADJUSTMENTS[BLACK][0][i] = Eval::PAWN_PSQT_BLACK_IS_OWN_QUEEN[i];
      PAWN_ADJUSTMENTS[WHITE][0][i] = Eval::PAWN_PSQT_BLACK_IS_OWN_QUEEN[_mir(i)];

      PAWN_ADJUSTMENTS[BLACK][1][i] = Eval::PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[i];
      PAWN_ADJUSTMENTS[WHITE][1][i] = Eval::PAWN_PSQT_BLACK_IS_ENEMY_QUEEN[_mir(i)];
    }
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
  _scores[color] += Eval::MATERIAL_VALUES[pieceType];

  _pawnScores[color][0] += PAWN_ADJUSTMENTS[color][0][square] * (pieceType == PAWN);
  _pawnScores[color][1] += PAWN_ADJUSTMENTS[color][1][square] * (pieceType == PAWN);
}

void PSquareTable::removePiece(Color color, PieceType pieceType, unsigned int square) {
  _scores[color] -= PIECE_VALUES[color][pieceType][square];
  _scores[color] -= Eval::MATERIAL_VALUES[pieceType];

  _pawnScores[color][0] -= PAWN_ADJUSTMENTS[color][0][square] * (pieceType == PAWN);
  _pawnScores[color][1] -= PAWN_ADJUSTMENTS[color][1][square] * (pieceType == PAWN);
}

void PSquareTable::movePiece(Color color, PieceType pieceType, unsigned int fromSquare, unsigned int toSquare) {
  removePiece(color, pieceType, fromSquare);
  addPiece(color, pieceType, toSquare);
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