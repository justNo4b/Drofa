#include "move.h"
#include "eval.h"
#include <iostream>
#include <algorithm>

const char Move::RANKS[] = {'1', '2', '3', '4', '5', '6', '7', '8'};
const char Move::FILES[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
const std::string Move::NULL_MOVE_NOTATION = "(none)";

extern U64 PASSED_PAWN_MASKS[2][64];

Move::Move() {
  _move = ((NULL_MOVE & 0x7f) << 21);
  _value = 0;
}

Move::Move(unsigned int from, unsigned int to, PieceType piece, unsigned int flags) {
  _move = ((flags & 0x7f) << 21) | ((to & 0x3f) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
  _value = 0;
}

Move::Move(int cMove){
  _move = cMove;
  _value = 0;
}

PieceType Move::getPieceType() const {
  return static_cast<PieceType>(_move & 0x7);
}

PieceType Move::getCapturedPieceType() const {
  return static_cast<PieceType>((_move >> 6) & 0x7);
}

void Move::setCapturedPieceType(PieceType pieceType) {
  unsigned int mask = 0x7 << 6;
  _move = (_move & ~mask) | ((pieceType << 6) & mask);
}

PieceType Move::getPromotionPieceType() const {
  return static_cast<PieceType>((_move >> 3) & 0x7);
}

void Move::setPromotionPieceType(PieceType pieceType) {
  unsigned int mask = 0x7 << 3;
  _move = (_move & ~mask) | ((pieceType << 3) & mask);
}

int Move::getValue() {
  return _value;
}

int Move::getMoveINT() const{
  return _move;
}

void Move::setValue(int value) {
  _value = value;
}

unsigned int Move::getFrom() const {
  return ((_move >> 9) & 0x3f);
}

unsigned int Move::getTo() const {
  return ((_move >> 15) & 0x3f);
}

unsigned int Move::getFlags() const {
  return ((_move >> 21) & 0x7f);
}

bool Move::isQuiet() const {
  // geflags + stuff
  return !(((_move >> 21) & 0x7f) & 0x63);
}

bool Move::isItPasserPush(const Board &board) const{
  Color movingColor = board.getActivePlayer();
  Color defColor = board.getInactivePlayer();
  int to = getTo();
  if (getPieceType() == PAWN &&
      !(board.getPieces(defColor, PAWN) & Eval::detail::PASSED_PAWN_MASKS[movingColor][to]) &&
      !(board.getAllPieces(defColor) & Eval::detail::FORWARD_BITS[movingColor][to])){
        return ((ONE << to) & PASSER_ZONE[movingColor]) ? true : false;
      }
  return false;
}

void Move::setFlag(Flag flag) {
  _move = _move | (flag << 21);
}

std::string Move::getNotation() const {
  // Special case for null moves
  if (getFlags() & NULL_MOVE) {
    return NULL_MOVE_NOTATION;
  }

  int fromIndex = getFrom();
  int toIndex = getTo();

  // special notation for non-FRC castling
  if (getFlags() & KSIDE_CASTLE){
     std::string moveNotation = indexToNotation(fromIndex);
     moveNotation += _row(fromIndex) == 0 ? "g1" : "g8";
     return moveNotation;

  }else if (getFlags() & QSIDE_CASTLE){
    std::string moveNotation = indexToNotation(fromIndex);
    moveNotation += _row(fromIndex) == 0 ? "c1" : "c8";
    return moveNotation;
  }


  std::string moveNotation = indexToNotation(fromIndex) + indexToNotation(toIndex);

  if (getFlags() & PROMOTION) {
    switch (getPromotionPieceType()) {
      case QUEEN: moveNotation += 'q';
        break;
      case ROOK: moveNotation += 'r';
        break;
      case KNIGHT: moveNotation += 'n';
        break;
      case BISHOP: moveNotation += 'b';
        break;
      default:break;
    }
  }

  return moveNotation;
}

bool Move::operator==(const Move other) const {
  return other._move == _move;
}

std::string Move::indexToNotation(int index) {
  return std::string({FILES[index % 8], RANKS[index / 8]});
}

unsigned int Move::notationToIndex(std::string notation) {
  int file = std::find(FILES, FILES + 8, notation[0]) - std::begin(FILES);
  int rank = std::find(RANKS, RANKS + 8, notation[1]) - std::begin(RANKS);

  return rank * 8 + file;
}
