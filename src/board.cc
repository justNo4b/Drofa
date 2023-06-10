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
#include "board.h"
#include "bitutils.h"
#include "attacks.h"
#include "eval.h"
#include <sstream>

Board::Board() {
    setToFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false);
}

Board::Board(std::string fen, bool isFrc) {
  setToFen(fen, isFrc);
}

U64 Board::getPieces(Color color, PieceType pieceType) const {
  return _pieces[color][pieceType];
}

U64 Board::getAllPieces(Color color) const {
  return _allPieces[color];
}

U64 Board::getAttackable(Color color) const {
  return _allPieces[color] & ~_pieces[color][KING];
}

U64 Board::getOccupied() const {
  return _occupied;
}

U64 Board::getNotOccupied() const {
  return ~_occupied;
}

U64 Board::getEnPassant() const {
  return _enPassant;
}

Color Board::getActivePlayer() const {
  return _activePlayer;
}

U64 Board::getAttacksForSquare(PieceType pieceType, Color color, int square) const {
  // Special case for pawns
  if (pieceType == PAWN) {
    switch (color) {
      case WHITE: return _getWhitePawnAttacksForSquare(square);
      case BLACK: return _getBlackPawnAttacksForSquare(square);
    }
  }

  U64 own = getAllPieces(color);
  U64 attacks;
  switch (pieceType) {
    case ROOK: attacks = _getRookAttacksForSquare(square, own);
      break;
    case KNIGHT: attacks = _getKnightAttacksForSquare(square, own);
      break;
    case BISHOP: attacks = _getBishopAttacksForSquare(square, own);
      break;
    case QUEEN: attacks = _getQueenAttacksForSquare(square, own);
      break;
    case KING: attacks = _getKingAttacksForSquare(square, own);
      break;
    default: fatal("Invalid piece type");
  }

  return attacks;
}

U64 Board::getMobilityForSquare(PieceType pieceType, Color color, int square, U64 pBB) const {

  U64 scan;
  U64 own = getAllPieces(color);
  U64 attacks;
  switch (pieceType) {
    case PAWN:
      // pawn mobility isnt used
      return 0;
    case ROOK:
      own = own ^  getPieces(color, ROOK) ^ getPieces (color, QUEEN);
      scan =  getPieces(color, ROOK) | getPieces (color, QUEEN);
      attacks = _getRookMobilityForSquare(square, own, scan);
      break;
    case KNIGHT:
      attacks = _getKnightMobilityForSquare(square, own);
      break;
    case BISHOP:
      own = own ^ getPieces (color, QUEEN) ^ getPieces (color, BISHOP);
      scan = getPieces (color, QUEEN) | getPieces (color, BISHOP);
      attacks = _getBishopMobilityForSquare(square, own, scan);
      break;
    case QUEEN:
      // Queen is a special case
      // We sont want it to scan (i guess it plays out
      // with Safety (ie when Q behind B it isnt good in attack)
      // Maybe later test scan thorough R only test?
      scan = ZERO;
      attacks = _getRookMobilityForSquare(square, own, scan) | _getBishopMobilityForSquare(square, own, scan);
      break;
    case KING:
      attacks = _getKingAttacksForSquare(square, own);
      break;
  }
  attacks = attacks & (~pBB);
  return attacks;
}

Color Board::getInactivePlayer() const {
  return _activePlayer == WHITE ? BLACK : WHITE;
}

ZKey Board::getZKey() const {
  return _zKey;
}

ZKey Board::getPawnStructureZKey() const {
  return _pawnStructureZkey;
}

ZKey Board::getpCountKey() const {
    return _pCountKey;
}

PSquareTable Board::getPSquareTable() const {
  return _pst;
}

bool Board::colorIsInCheck(Color color) const {
  int kingSquare = _bitscanForward(getPieces(color, KING));
  return squareUnderAttack(getOppositeColor(color), kingSquare);
}

int Board::getHalfmoveClock() const {
  return _halfmoveClock;
}

std::string Board::getStringRep() const {
  std::string stringRep = "8  ";
  int rank = 8;

  U64 boardPos = 56; // Starts at a8, goes down rank by rank
  int squaresProcessed = 0;

  while (squaresProcessed < 64) {
    U64 square = ONE << boardPos;
    bool squareOccupied = (square & _occupied) != 0;

    if (squareOccupied) {
      if (square & _pieces[WHITE][PAWN]) stringRep += " P ";
      else if (square & _pieces[BLACK][PAWN]) stringRep += " p ";

      else if (square & _pieces[WHITE][ROOK]) stringRep += " R ";
      else if (square & _pieces[BLACK][ROOK]) stringRep += " r ";

      else if (square & _pieces[WHITE][KNIGHT]) stringRep += " N ";
      else if (square & _pieces[BLACK][KNIGHT]) stringRep += " n ";

      else if (square & _pieces[WHITE][BISHOP]) stringRep += " B ";
      else if (square & _pieces[BLACK][BISHOP]) stringRep += " b ";

      else if (square & _pieces[WHITE][QUEEN]) stringRep += " Q ";
      else if (square & _pieces[BLACK][QUEEN]) stringRep += " q ";

      else if (square & _pieces[WHITE][KING]) stringRep += " K ";
      else if (square & _pieces[BLACK][KING]) stringRep += " k ";
    } else {
      stringRep += " . ";
    }
    squaresProcessed++;

    if ((squaresProcessed % 8 == 0) && (squaresProcessed != 64)) {
      switch (squaresProcessed / 8) {
        case 1:
          stringRep += "        ";
          stringRep += getActivePlayer() == WHITE ? "White" : "Black";
          stringRep += " to Move";
          break;
        case 2:
          stringRep += "        Halfmove Clock: ";
          stringRep += std::to_string(_halfmoveClock);
          break;
        case 3:
          stringRep += "        Castling Rights: ";
          stringRep += _castlingRights & 1 ? "K" : "";
          stringRep += _castlingRights & 2 ? "Q" : "";
          stringRep += _castlingRights & 4 ? "k" : "";
          stringRep += _castlingRights & 8 ? "q" : "";
          break;
        case 4:
          stringRep += "        En Passant Square: ";
          stringRep += _enPassant == ZERO ? "-" : Move::indexToNotation(_bitscanForward(_enPassant));
          break;
      }
      stringRep += "\n" + std::to_string(--rank) + "  ";
      boardPos -= 16;
    }

    boardPos++;
  }

  stringRep += "\n\n    A  B  C  D  E  F  G  H";
  return stringRep;
}

void Board::_clearBitBoards() {
  for (Color color: {WHITE, BLACK}) {
    for (PieceType pieceType : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
      _pieces[color][pieceType] = ZERO;
    }

    _allPieces[WHITE] = ZERO;
    _allPieces[BLACK] = ZERO;
  }

  _enPassant = ZERO;

  _occupied = ZERO;
}

void Board::setToFen(std::string fenString, bool isFrc) {
  std::istringstream fenStream(fenString);
  std::string token;
  _gameClock = 0;
  _frc = isFrc;

  _clearBitBoards();

  U64 boardPos = 56; // Fen string starts at a8 = index 56
  fenStream >> token;
  for (auto currChar : token) {
    switch (currChar) {
      case 'p': _pieces[BLACK][PAWN] |= (ONE << boardPos++);
        break;
      case 'r': _pieces[BLACK][ROOK] |= (ONE << boardPos++);
        break;
      case 'n': _pieces[BLACK][KNIGHT] |= (ONE << boardPos++);
        break;
      case 'b': _pieces[BLACK][BISHOP] |= (ONE << boardPos++);
        break;
      case 'q': _pieces[BLACK][QUEEN] |= (ONE << boardPos++);
        break;
      case 'k': _pieces[BLACK][KING] |= (ONE << boardPos++);
        break;
      case 'P': _pieces[WHITE][PAWN] |= (ONE << boardPos++);
        break;
      case 'R': _pieces[WHITE][ROOK] |= (ONE << boardPos++);
        break;
      case 'N': _pieces[WHITE][KNIGHT] |= (ONE << boardPos++);
        break;
      case 'B': _pieces[WHITE][BISHOP] |= (ONE << boardPos++);
        break;
      case 'Q': _pieces[WHITE][QUEEN] |= (ONE << boardPos++);
        break;
      case 'K': _pieces[WHITE][KING] |= (ONE << boardPos++);
        break;
      case '/': boardPos -= 16; // Go down one rank
        break;
      default:boardPos += static_cast<U64>(currChar - '0');
    }
  }

  // Next to move
  fenStream >> token;
  _activePlayer = token == "w" ? WHITE : BLACK;

  // Castling availability
  fenStream >> token;
  _castlingRights = 0;
  for (auto currChar : token) {
    U64 rook = 0;
    int king = 0;
    switch (currChar) {
      case 'K':
        rook = _pieces[WHITE][ROOK];
        king = _bitscanForward(_pieces[WHITE][KING]);
        while (rook)
        {
            int sq = _popLsb(rook);
            if (sq > king) _castlingRights |= (ONE << sq);
        }
        break;
      case 'Q':
        rook = _pieces[WHITE][ROOK];
        king = _bitscanForward(_pieces[WHITE][KING]);
        while (rook)
        {
            int sq = _popLsb(rook);
            if (sq < king) _castlingRights |= (ONE << sq);
        }
        break;
      case 'k':
        rook = _pieces[BLACK][ROOK];
        king = _bitscanForward(_pieces[BLACK][KING]);
        while (rook)
        {
            int sq = _popLsb(rook);
            if (sq > king) _castlingRights |= (ONE << sq);
        }
        break;
      case 'q':
        rook = _pieces[BLACK][ROOK];
        king = _bitscanForward(_pieces[BLACK][KING]);
        while (rook)
        {
            int sq = _popLsb(rook);
            if (sq < king) _castlingRights |= (ONE << sq);
        }
        break;
      case 'A': _castlingRights |= (ONE << a1);
        break;
      case 'a': _castlingRights |= (ONE << a8);
        break;
      case 'B': _castlingRights |= (ONE << b1);
        break;
      case 'b': _castlingRights |= (ONE << b8);
        break;
      case 'C': _castlingRights |= (ONE << c1);
        break;
      case 'c': _castlingRights |= (ONE << c8);
        break;
      case 'D': _castlingRights |= (ONE << d1);
        break;
      case 'd': _castlingRights |= (ONE << d8);
        break;
      case 'E': _castlingRights |= (ONE << e1);
        break;
      case 'e': _castlingRights |= (ONE << e8);
        break;
      case 'F': _castlingRights |= (ONE << f1);
        break;
      case 'f': _castlingRights |= (ONE << f8);
        break;
      case 'G': _castlingRights |= (ONE << g1);
        break;
      case 'g': _castlingRights |= (ONE << g8);
        break;
      case 'H': _castlingRights |= (ONE << h1);
        break;
      case 'h': _castlingRights |= (ONE << h8);
        break;
    }
  }

  // En passant target square
  fenStream >> token;
  _enPassant = token == "-" ? ZERO : ONE << Move::notationToIndex(token);

  // Halfmove clock
  fenStream >> _halfmoveClock;
  // Our gameClock is number of plys, so multyply movenum by 2
  fenStream >> _gameClock;
  _gameClock = _gameClock * 2;


  // set up phase
  _phase = PHASE_WEIGHT_SUM;

  for (auto pieceType : {ROOK, KNIGHT, BISHOP, QUEEN}) {
    _phase -= _popCount(getPieces(WHITE, pieceType)) * PHASE_WEIGHTS[pieceType];
    _phase -= _popCount(getPieces(BLACK, pieceType)) * PHASE_WEIGHTS[pieceType];
  }

  // Make sure phase is not negative
  _phase = std::max(0, _phase);

  _updateNonPieceBitBoards();
  _zKey = ZKey(*this);
  _pawnStructureZkey.setFromPawnStructure(*this);
  _pCountKey.setFromPieceCounts(*this);

  _pst = PSquareTable(*this);
}

void Board::_updateNonPieceBitBoards() {
  _allPieces[WHITE] = _pieces[WHITE][PAWN] | \
    _pieces[WHITE][ROOK] | \
    _pieces[WHITE][KNIGHT] | \
    _pieces[WHITE][BISHOP] | \
    _pieces[WHITE][QUEEN] | \
    _pieces[WHITE][KING];

  _allPieces[BLACK] = _pieces[BLACK][PAWN] | \
    _pieces[BLACK][ROOK] | \
    _pieces[BLACK][KNIGHT] | \
    _pieces[BLACK][BISHOP] | \
    _pieces[BLACK][QUEEN] | \
    _pieces[BLACK][KING];

  _occupied = _allPieces[WHITE] | _allPieces[BLACK];
}

PieceType Board::getPieceAtSquare(Color color, int squareIndex) const {
  U64 square = ONE << squareIndex;

  PieceType piece;

  if (square & _pieces[color][PAWN]) piece = PAWN;
  else if (square & _pieces[color][ROOK]) piece = ROOK;
  else if (square & _pieces[color][KNIGHT]) piece = KNIGHT;
  else if (square & _pieces[color][BISHOP]) piece = BISHOP;
  else if (square & _pieces[color][KING]) piece = KING;
  else if (square & _pieces[color][QUEEN]) piece = QUEEN;
  else
    fatal((color == WHITE ? std::string("White") : std::string("Black")) +
        " piece at square " + std::to_string(squareIndex) + " does not exist");

  return piece;
}

void Board::_movePiece(Color color, PieceType pieceType, int from, int to) {
  U64 squareMask = to != from ? (ONE << to) | (ONE << from) : 0;

  _pieces[color][pieceType] ^= squareMask;
  _allPieces[color] ^= squareMask;

  _occupied ^= squareMask;

  // Update pawn structure ZKey if this is a pawn move
  if (pieceType == PAWN || pieceType == KING) {
    _pawnStructureZkey.movePiece(color, pieceType, from, to);
  }

  _zKey.movePiece(color, pieceType, from, to);
  _pst.movePiece(color, pieceType, from, to);
}

void Board::_removePiece(Color color, PieceType pieceType, int squareIndex) {
  U64 square = ONE << squareIndex;
  _phase += PHASE_WEIGHTS[pieceType];

  _pieces[color][pieceType] ^= square;
  _allPieces[color] ^= square;

  _occupied ^= square;

  if (pieceType == PAWN){
    _pawnStructureZkey.flipPiece(color, PAWN, squareIndex);
  }

  _pCountKey.flipPieceCount(color, pieceType, _popCount(getPieces(color, pieceType)) + 1);
  _zKey.flipPiece(color, pieceType, squareIndex);
  _pst.removePiece(color, pieceType, squareIndex);
}

void Board::_addPiece(Color color, PieceType pieceType, int squareIndex) {
  U64 square = ONE << squareIndex;
  _phase -= PHASE_WEIGHTS[pieceType];

  _pieces[color][pieceType] |= square;
  _allPieces[color] |= square;

  _occupied |= square;

  _pCountKey.flipPieceCount(color, pieceType, _popCount(getPieces(color, pieceType)));
  _zKey.flipPiece(color, pieceType, squareIndex);
  _pst.addPiece(color, pieceType, squareIndex);
}

bool Board:: isThereMajorPiece() const {
  Color active = getActivePlayer();
  return (_popCount(_allPieces[active] ^ _pieces[active][PAWN] ^ _pieces[active][KING]) > 0);
}

bool Board:: isEndGamePosition() const {
  return (_popCount(_allPieces[WHITE] ^ _pieces[WHITE][PAWN]) +
          _popCount(_allPieces[BLACK] ^ _pieces[BLACK][PAWN])) < 5;
}

U64 Board::_getLeastValuableAttacker(Color color, U64 attackers, PieceType &piece) const{

  U64 tmp = ZERO;
  //check pawns
  tmp = attackers & getPieces(color, PAWN);
  if (tmp){
    piece = PAWN;
    return ONE << _popLsb(tmp);
  }
  //check knight
  tmp = attackers & getPieces(color, KNIGHT);
  if (tmp){
    piece = KNIGHT;
    return ONE << _popLsb(tmp);
  }
  //check bishop
  tmp = attackers & getPieces(color, BISHOP);
  if (tmp){
    piece = BISHOP;
    return ONE << _popLsb(tmp);
  }
  // check ROOK
   tmp = attackers & getPieces(color, ROOK);
  if (tmp){
    piece = ROOK;
    return ONE << _popLsb(tmp);
  }
  // Check QUEEN
  tmp = attackers & getPieces(color, QUEEN);
    if (tmp){
    piece = QUEEN;
    return ONE << _popLsb(tmp);
  }
  // King
  tmp = attackers & getPieces(color, KING);
    if (tmp){
    piece = KING;
    return ONE << _popLsb(tmp);
  }
  piece = KING;
  return 0;
}

bool Board::SEE_GreaterOrEqual(const Move move, int threshold) const{

  // 0. Early exits
  // If move is special case (promotion, enpass, castle)
  // its SEE is at least 0 (well, not exactly, Prom could be -100, but still)
  // so just return true

  unsigned int flags = move.getFlags();
  if ((flags & Move::EN_PASSANT) || (flags & Move::KSIDE_CASTLE) || (flags & Move::QSIDE_CASTLE)){
       return 1024;
     }

  // 1. Set variables
  int from = move.getFrom();
  int to = move.getTo();
  Color side = getActivePlayer();
  PieceType movingPt = move.getPieceType();

  // 2. Early exits 2.
  // If we capture stuff and dont beat limit, we are done
  int value = (flags & Move::CAPTURE) ? _SEE_cost[getPieceAtSquare(getOppositeColor(side), to)] : 0;
  value -= threshold;
  if (value < 0) return false;

  // if we capture, lose a capturing piece and still beat limit,
  // we are good
  value -= _SEE_cost[movingPt];
  if (value >= 0) return true;

  // 3. Prepare variables for negamax
  // Get pieces that attack target sqv
  U64 aBoard[2];
  aBoard[WHITE] = _squareAttackedBy(WHITE, to);
  aBoard[BLACK] = _squareAttackedBy(BLACK, to);

  // get occupied
  U64 occupied = _occupied;

  U64 horiXray = getPieces(WHITE, ROOK) | getPieces(WHITE, QUEEN) |  getPieces(BLACK, ROOK) | getPieces(BLACK, QUEEN);
  U64 diagXray = getPieces(WHITE, PAWN) | getPieces(WHITE, BISHOP) | getPieces(WHITE, QUEEN) |
                 getPieces(BLACK, PAWN) | getPieces(BLACK, BISHOP) | getPieces(BLACK, QUEEN);
  U64 attBit = (ONE << from);



  occupied = occupied ^ attBit;
  if (horiXray & attBit) aBoard[side] |= (_squareAttackedByRook(side, to, occupied) & occupied);
  if (diagXray & attBit) aBoard[side] |= (_squareAttackedByBishop(side, to, occupied)  & occupied);

  side = getOppositeColor(side);

  while(true)
  {
    aBoard[getOppositeColor(side)] = aBoard[getOppositeColor(side)] & ~attBit;
    attBit = _getLeastValuableAttacker(side, aBoard[side], movingPt);
    if (!attBit) break;

    occupied = occupied ^ attBit;
    if (horiXray & attBit) aBoard[side] |= (_squareAttackedByRook(side, to, occupied) & occupied);
    if (diagXray & attBit) aBoard[side] |= (_squareAttackedByBishop(side, to, occupied)  & occupied);

    side = getOppositeColor(side);

    value = -value - 1 - _SEE_cost[movingPt];
    if (value >= 0){
       break;
    }
  }



  return side != getActivePlayer();

}

int  Board:: Calculate_SEE(const Move move) const{

  // 0. Early exits
  // If move is special case (promotion, enpass, castle)
  // its SEE is at least 0 (well, not exactly, Prom could be -100, but still)
  // so just return true

  unsigned int flags = move.getFlags();
  if ((flags & Move::EN_PASSANT) || (flags & Move::KSIDE_CASTLE) || (flags & Move::QSIDE_CASTLE)){
       return 1024;
     }


  // 1. Set variables
  int gain[32] = {0};
  int d = 0;
  int from = move.getFrom();
  int to = move.getTo();
  Color side = getActivePlayer();
  PieceType aPiece = move.getPieceType();

  // Get pieces that attack target sqv
  U64 aBoard[2];
  aBoard[WHITE] = _squareAttackedBy(WHITE, to);
  aBoard[BLACK] = _squareAttackedBy(BLACK, to);

  // get occupied
  U64 occupied = _occupied;

  U64 horiXray = getPieces(WHITE, ROOK) | getPieces(WHITE, QUEEN) |  getPieces(BLACK, ROOK) | getPieces(BLACK, QUEEN);
  U64 diagXray = getPieces(WHITE, PAWN) | getPieces(WHITE, BISHOP) | getPieces(WHITE, QUEEN) |
                 getPieces(BLACK, PAWN) | getPieces(BLACK, BISHOP) | getPieces(BLACK, QUEEN);
  U64 attBit = (ONE << from);


    gain[0] = (flags & Move::CAPTURE) ? _SEE_cost[getPieceAtSquare(getOppositeColor(side), to)] : 0;
    //std::cout <<"d"<< d << " gain[d] " << gain [d] <<std::endl;
  // 3.SEE Negamax Cycle
  do
  {
    d++;
    gain[d]  = _SEE_cost[aPiece] - gain[d-1];
    //std::cout <<"d"<< d << " gain[d] " << gain [d] << "  " << aPiece <<std::endl;
    if ( std::max(-gain[d-1], gain[d]) < 0){
      break;
    }
    aBoard[side] = aBoard[side] & ~attBit;
    occupied = occupied ^ attBit;
    if (horiXray & attBit){
      aBoard[side] |= (_squareAttackedByRook(side, to, occupied) & occupied);
    }
    if (diagXray & attBit){
      aBoard[side] |= (_squareAttackedByBishop(side, to, occupied)  & occupied);
    }
    // switch side and get next attacker
    side = getOppositeColor(side);
    attBit = _getLeastValuableAttacker(side, aBoard[side], aPiece);
    //std::cout << d << " atta " << _bitscanForward(attBit)<<std::endl;

  } while (attBit);

  // 4.Calculate value
  while (--d){
    gain[d-1] = - std::max(-gain[d-1], gain[d]);
  }

  return gain[0];
}

void Board::doMove(Move move) {
  // Clear En passant info after each move if it exists
  if (_enPassant) {
    _zKey.clearEnPassant();
    _enPassant = ZERO;
  }
  _gameClock++;
  int from = move.getFrom();
  int to = move.getTo();
  // Handle move depending on what type of move it is
  unsigned int flags = move.getFlags();
  if (!flags) {
    // No flags set, not a special move
    _movePiece(_activePlayer, move.getPieceType(), from, to);
  } else if ((flags & Move::CAPTURE) && (flags & Move::PROMOTION)) { // Capture promotion special case
    // Remove captured Piece
    PieceType capturedPieceType = move.getCapturedPieceType();
    _removePiece(getInactivePlayer(), capturedPieceType, to);

    // Remove promoting pawn
    _removePiece(_activePlayer, PAWN, from);

    // Add promoted piece
    PieceType promotionPieceType = move.getPromotionPieceType();
    _addPiece(_activePlayer, promotionPieceType, to);
  } else if (flags & Move::CAPTURE) {
    // Remove captured Piece
    PieceType capturedPieceType = move.getCapturedPieceType();
    _removePiece(getInactivePlayer(), capturedPieceType, to);

    // Move capturing piece
    _movePiece(_activePlayer, move.getPieceType(), from, to);
  } else if (flags & Move::KSIDE_CASTLE) {
    // Move the correct rook
    if (_activePlayer == WHITE) {
      _movePiece(_activePlayer, KING, from, g1);
      _movePiece(WHITE, ROOK, to, f1);
    } else {
      _movePiece(_activePlayer, KING, from, g8);
      _movePiece(BLACK, ROOK, to, f8);
    }
  } else if (flags & Move::QSIDE_CASTLE) {
    // Move the correct rook
    if (_activePlayer == WHITE) {
      _movePiece(_activePlayer, KING, from, c1);
      _movePiece(WHITE, ROOK, to, d1);
    } else {
      _movePiece(_activePlayer, KING, from, c8);
      _movePiece(BLACK, ROOK, to, d8);
    }
  } else if (flags & Move::EN_PASSANT) {
    // Remove the correct pawn
    if (_activePlayer == WHITE) {
      _removePiece(BLACK, PAWN, to - 8);
    } else {
      _removePiece(WHITE, PAWN, to + 8);
    }

    // Move the capturing pawn
    _movePiece(_activePlayer, move.getPieceType(), from, to);
  } else if (flags & Move::PROMOTION) {
    // Remove promoted pawn
    _removePiece(_activePlayer, PAWN, from);

    // Add promoted piece
    _addPiece(_activePlayer, move.getPromotionPieceType(), to);
  } else if (flags & Move::DOUBLE_PAWN_PUSH) {
    _movePiece(_activePlayer, move.getPieceType(), from, to);

    // Set square behind pawn as _enPassant
    unsigned int enPasIndex = _activePlayer == WHITE ? to - 8 : to + 8;
    _enPassant = ONE << enPasIndex;
    _zKey.setEnPassantFile(enPasIndex % 8);
  }

  // Halfmove clock reset on pawn moves or captures, incremented otherwise
  if (move.getPieceType() == PAWN || move.getFlags() & Move::CAPTURE) {
    _halfmoveClock = 0;
  } else {
    _halfmoveClock++;
  }

  if (_castlingRights) {
    _updateCastlingRightsForMove(move);
  }

  _zKey.flipActivePlayer();
  _activePlayer = getInactivePlayer();
}

void Board:: doNool(){
  // Clear En passant info after each move if it exists
  if (_enPassant) {
    _zKey.clearEnPassant();
    _enPassant = ZERO;
  }

  _zKey.flipActivePlayer();
  _activePlayer = getInactivePlayer();
}

bool Board::squareUnderAttack(Color color, int squareIndex) const {
  // Check for pawn, knight and king attacks
  if (Attacks::getNonSlidingAttacks(PAWN, squareIndex, getOppositeColor(color)) & getPieces(color, PAWN)) return true;
  if (Attacks::getNonSlidingAttacks(KNIGHT, squareIndex) & getPieces(color, KNIGHT)) return true;
  if (Attacks::getNonSlidingAttacks(KING, squareIndex) & getPieces(color, KING)) return true;

  // Check for bishop/queen attacks
  U64 bishopsQueens = getPieces(color, BISHOP) | getPieces(color, QUEEN);
  if (_getBishopAttacksForSquare(squareIndex, ZERO) & bishopsQueens) return true;

  // Check for rook/queen attacks
  U64 rooksQueens = getPieces(color, ROOK) | getPieces(color, QUEEN);
  if (_getRookAttacksForSquare(squareIndex, ZERO) & rooksQueens) return true;

  return false;
}

U64 Board::getCastlingRightsColored(Color color) const {
    return color == WHITE ? _castlingRights & RANK_1 : _castlingRights & RANK_8;
}

U64 Board::getCastlingRights() const{
    return _castlingRights;
}

U64 Board::_squareAttackedBy(Color color, int squareIndex) const {
  // Check for pawn, knight and king attacks
  U64 Attackers;

  Attackers  = Attacks::getNonSlidingAttacks(PAWN, squareIndex, getOppositeColor(color)) & getPieces(color, PAWN);
  Attackers |= Attacks::getNonSlidingAttacks(KNIGHT, squareIndex) & getPieces(color, KNIGHT);
  Attackers |= Attacks::getNonSlidingAttacks(KING, squareIndex) & getPieces(color, KING);

  // Check for bishop/queen attacks
  U64 bishopsQueens = getPieces(color, BISHOP) | getPieces(color, QUEEN);
  Attackers |= (_getBishopAttacksForSquare(squareIndex, ZERO) & bishopsQueens);

  // Check for rook/queen attacks
  U64 rooksQueens = getPieces(color, ROOK) | getPieces(color, QUEEN);
  Attackers |= (_getRookAttacksForSquare(squareIndex, ZERO) & rooksQueens);

  return Attackers;
}

U64 Board::_squareAttackedByRook(Color color, int square, U64 occupied) const{
  U64 rooksQueens = getPieces(color, ROOK) | getPieces(color, QUEEN);
  U64 Attackers = Attacks::getSlidingAttacks(ROOK, square, occupied) & rooksQueens;
  return Attackers;
}

U64 Board::_squareAttackedByBishop(Color color, int square, U64 occupied) const{
  U64 bishopsQueens = getPieces(color, BISHOP) | getPieces(color, QUEEN);
  U64 Attackers = Attacks::getSlidingAttacks(BISHOP, square, occupied) & bishopsQueens;
  return Attackers;
}

void Board::_updateCastlingRightsForMove(Move move) {
  unsigned int flags = move.getFlags();
  U64 oldCastlingRights = _castlingRights;

  // Update castling flags if rooks have been captured
  if (flags & Move::CAPTURE) {
    // Update castling rights if a rook was captured
    _castlingRights &= ~(ONE << move.getTo());
  }

  // Update castling flags if king have moved
  if (move.getPieceType() == KING){
    if(_row(move.getFrom()) == 0) _castlingRights &= ~RANK_1;
    if(_row(move.getFrom()) == 7) _castlingRights &= ~RANK_8;
  }

  // Update flasgs if rook have moved
  _castlingRights &= ~ (ONE << move.getFrom());

  _zKey.updateCastlingRights(oldCastlingRights ,_castlingRights);
}

void Board::setToStartPos() {
  setToFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false);
}

U64 Board::_getWhitePawnAttacksForSquare(int square) const {
  return Attacks::getNonSlidingAttacks(PAWN, square, WHITE);
}

U64 Board::_getBlackPawnAttacksForSquare(int square) const {
  return Attacks::getNonSlidingAttacks(PAWN, square, BLACK);
}

U64 Board::_getKnightAttacksForSquare(int square, U64 own) const {
  return Attacks::getNonSlidingAttacks(KNIGHT, square) & ~own;
}

U64 Board::_getKingAttacksForSquare(int square, U64 own) const {
  return Attacks::getNonSlidingAttacks(KING, square) & ~own;
}

U64 Board::_getBishopAttacksForSquare(int square, U64 own) const {
  return Attacks::getSlidingAttacks(BISHOP, square, _occupied) & ~own;
}

U64 Board::_getRookAttacksForSquare(int square, U64 own) const {
  return Attacks::getSlidingAttacks(ROOK, square, _occupied) & ~own;
}

U64 Board::_getQueenAttacksForSquare(int square, U64 own) const {
  return Attacks::getSlidingAttacks(QUEEN, square, _occupied) & ~own;
}

U64 Board::_getBishopMobilityForSquare(int square, U64 own, U64 scanthrough) const {
  return Attacks::getSlidingAttacks(BISHOP, square, _occupied ^ scanthrough)& ~own;
}

U64 Board::_getRookMobilityForSquare(int square, U64 own, U64 scanthrough) const {
  return Attacks::getSlidingAttacks(ROOK, square, _occupied ^ scanthrough) & ~own;
}

U64 Board::_getQueenMobilityForSquare(int square, U64 own, U64 scanthrough) const {
  return Attacks::getSlidingAttacks(QUEEN, square, _occupied ^ scanthrough) & ~own;
}

U64 Board::_getKnightMobilityForSquare(int square, U64 own) const {
  return Attacks::getNonSlidingAttacks(KNIGHT, square) & ~own;
}

int Board::_getGameClock() const{
  return _gameClock;
}

int Board::getPhase() const{
  return ((std::max(0, _phase) * MAX_PHASE) + (PHASE_WEIGHT_SUM / 2)) / PHASE_WEIGHT_SUM;
}

 bool Board::getFrcMode() const{
    return _frc;
 }