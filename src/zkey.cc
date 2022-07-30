#include "zkey.h"
#include "board.h"
#include "bitutils.h"
#include <random>
#include <climits>
#include <iostream>
#include <sstream>

const unsigned int ZKey::PRNG_KEY = 0xDEADBEEF;

U64 ZKey::PIECE_KEYS[2][6][64];
U64 ZKey::EN_PASSANT_KEYS[8];

U64 ZKey::PIECE_COUNT_KEY[2][6][11];

U64 ZKey::KS_CASTLE_KEYS[2];
U64 ZKey::QS_CASTLE_KEYS[2];

U64 ZKey::WHITE_TO_MOVE_KEY;

void ZKey::init() {
  std::mt19937_64 mt(PRNG_KEY);
  std::uniform_int_distribution<U64> dist(ZERO, ULLONG_MAX);

  KS_CASTLE_KEYS[WHITE] = dist(mt);
  QS_CASTLE_KEYS[WHITE] = dist(mt);
  KS_CASTLE_KEYS[BLACK] = dist(mt);
  QS_CASTLE_KEYS[BLACK] = dist(mt);

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

}

ZKey::ZKey() {
  _key[mainkey] = ZERO;
  _key[pawnkey] = ZERO;
  _key[countkey] = ZERO;

  _whiteKs = false, _whiteQs = false, _blackKs = false, _blackQs = false;
  _enPassantFile = -1;
}

ZKey::ZKey(const Board &board) {
  _key[mainkey] = ZERO;

  if (board.getActivePlayer() == WHITE) {
    _key[mainkey] ^= WHITE_TO_MOVE_KEY;
  }

  PieceType pieces[6] = {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING};

  // Add white/black pieces
  for (auto piece : pieces) {
    U64 whiteBitBoard = board.getPieces(WHITE, piece);
    U64 blackBitBoard = board.getPieces(BLACK, piece);

    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
      U64 square = ONE << squareIndex;
      if (square & whiteBitBoard) {
        flipPiece(mainkey, WHITE, piece, squareIndex);
      } else if (square & blackBitBoard) {
        flipPiece(mainkey, BLACK, piece, squareIndex);
      }
    }
  }

  // Add en passant
  if (board.getEnPassant()) {
    _enPassantFile = (_bitscanForward(board.getEnPassant())) % 8;
    _key[mainkey] ^= EN_PASSANT_KEYS[_enPassantFile];
  } else {
    _enPassantFile = -1;
  }

  _whiteKs = false, _whiteQs = false, _blackKs = false, _blackQs = false;
  // Add castles
  if (board.getKsCastlingRights(WHITE)) {
    _whiteKs = true;
    _flipKsCastle(WHITE);
  }
  if (board.getQsCastlingRights(WHITE)) {
    _whiteQs = true;
    _flipQsCastle(WHITE);
  }
  if (board.getKsCastlingRights(BLACK)) {
    _blackKs = true;
    _flipKsCastle(BLACK);
  }
  if (board.getQsCastlingRights(BLACK)) {
    _blackQs = true;
    _flipQsCastle(BLACK);
  }
}

void ZKey::setFromPawnStructure(const Board &board) {
  _key[pawnkey] = ZERO;

  // Add white/black pieces
  U64 whitePawns = board.getPieces(WHITE, PAWN);
  U64 blackPawns = board.getPieces(BLACK, PAWN);

  for (unsigned int squareIndex = 0; squareIndex < 64; squareIndex++) {
    U64 square = ONE << squareIndex;
    if (square & whitePawns) {
      flipPiece(pawnkey, WHITE, PAWN, squareIndex);
    } else if (square & blackPawns) {
      flipPiece(pawnkey, BLACK, PAWN, squareIndex);
    }
  }
}

void ZKey::setFromPieceCounts(const Board &board) {
    _key[countkey] = ZERO;
    for (auto pt : {  PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING}){
        U64 blackPiece = board.getPieces(BLACK, pt);
        U64 whitePiece = board.getPieces(WHITE, pt);
        flipPieceCount(WHITE, pt, _popCount(whitePiece));
        flipPieceCount(BLACK, pt, _popCount(blackPiece));
    }
}

U64 ZKey::getValue() const {
  return _key[mainkey];
}

U64 ZKey::getPawnValue() const{
  return _key[pawnkey];
}

U64 ZKey::getCountValue() const{
  return _key[countkey];
}

void ZKey::movePiece(KeyType type, Color color, PieceType piece, unsigned int from, unsigned int to) {
  flipPiece(type, color, piece, from);
  flipPiece(type, color, piece, to);
}

void ZKey::pCountAddRemove(Color color, PieceType piece, int cWas, int cNow){
    flipPieceCount(color, piece, cWas);
    flipPieceCount(color, piece, cNow);
}

void ZKey::flipPiece(KeyType type, Color color, PieceType piece, unsigned int index) {
  _key[type] ^= PIECE_KEYS[color][piece][index];
}

void ZKey::flipPieceCount(Color color, PieceType pt, int count){
    _key[2] ^= PIECE_COUNT_KEY[color][pt][count];
}

void ZKey::updateCastlingRights(bool whiteKs, bool whiteQs, bool blackKs, bool blackQs) {
  if (whiteKs != _whiteKs) {
    _whiteKs = false;
    _flipKsCastle(WHITE);
  }
  if (whiteQs != _whiteQs) {
    _whiteQs = false;
    _flipQsCastle(WHITE);
  }
  if (blackKs != _blackKs) {
    _blackKs = false;
    _flipKsCastle(BLACK);
  }
  if (blackQs != _blackQs) {
    _blackQs = false;
    _flipQsCastle(BLACK);
  }
}

void ZKey::clearEnPassant() {
  if (_enPassantFile != -1) {
    _key[mainkey] ^= EN_PASSANT_KEYS[_enPassantFile];
    _enPassantFile = -1;
  }
}

void ZKey::setEnPassantFile(unsigned int file) {
  _enPassantFile = file;
  _key[mainkey] ^= EN_PASSANT_KEYS[file];
}

void ZKey::_flipKsCastle(Color color) {
  _key[mainkey] ^= KS_CASTLE_KEYS[color];
}

void ZKey::_flipQsCastle(Color color) {
  _key[mainkey] ^= QS_CASTLE_KEYS[color];
}

void ZKey::flipActivePlayer() {
  _key[mainkey] ^= WHITE_TO_MOVE_KEY;
}

void ZKey::setpKeyFromString(std::string pseudoFen){
    _key[countkey] = ZERO;
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
        flipPieceCount(WHITE, pt, pArray[WHITE][pt]);
        flipPieceCount(BLACK, pt, pArray[BLACK][pt]);
    }

}
