#include "movegen.h"
#include "movepicker.h"
#include "eval.h"


MoveGen::MoveGen(const Board &board, bool isCaptureGenerated) {
  setBoard(board, isCaptureGenerated);
}

MoveGen::MoveGen() {
  setBoard(Board(), false);
}

void MoveGen::setBoard(const Board &board, bool isCaptureGenerated) {
  _moves = MoveList();
  if (!isCaptureGenerated){
     _genMoves(board);
  } else{
    _genCaptures(board);
  }
}


MoveList MoveGen::getMoves() {
  return _moves;
}

void MoveGen::_genMoves(const Board &board) {
  _moves.reserve(MOVELIST_RESERVE_SIZE);
  switch (board.getActivePlayer()) {
    case WHITE: _genWhiteMoves(board);
      break;
    case BLACK: _genBlackMoves(board);
      break;
  }
}

void MoveGen::_genCaptures(const Board &board) {
    _moves.reserve(MOVELIST_RESERVE_SIZE);
    Color color = board.getActivePlayer();
    Color otherColor = getOppositeColor(color);

    _genPawnAttacks(board, color);
    _getPromQonly(board, color);
    _genRookCaps(board, board.getPieces(color, ROOK), board.getAttackable(otherColor));
    _genKnightCaps(board, board.getPieces(color, KNIGHT), board.getAttackable(otherColor));
    _genBishopCaps(board, board.getPieces(color, BISHOP), board.getAttackable(otherColor));
    _genKingCaps(board, board.getPieces(color, KING), board.getAttackable(otherColor));
    _genQueenCaps(board, board.getPieces(color, QUEEN), board.getAttackable(otherColor));

}

void MoveGen::_genWhiteMoves(const Board &board) {
  _genWhitePawnMoves(board);
  _genWhiteRookMoves(board);
  _genWhiteKnightMoves(board);
  _genWhiteBishopMoves(board);
  _genWhiteKingMoves(board);
  _genWhiteQueenMoves(board);
}

void MoveGen::_genBlackMoves(const Board &board) {
  _genBlackPawnMoves(board);
  _genBlackRookMoves(board);
  _genBlackKnightMoves(board);
  _genBlackBishopMoves(board);
  _genBlackKingMoves(board);
  _genBlackQueenMoves(board);
}

void MoveGen::_genPawnPromotions(unsigned int from, unsigned int to, unsigned int flags, PieceType capturedPieceType) {
  Move promotionBase = Move(from, to, PAWN, flags | Move::PROMOTION);
  if (flags & Move::CAPTURE) {
    promotionBase.setCapturedPieceType(capturedPieceType);
  }

  Move queenPromotion = promotionBase;
  queenPromotion.setPromotionPieceType(QUEEN);
  _moves.push_back(queenPromotion);

  Move rookPromotion = promotionBase;
  rookPromotion.setPromotionPieceType(ROOK);
  _moves.push_back(rookPromotion);

  Move bishopPromotion = promotionBase;
  bishopPromotion.setPromotionPieceType(BISHOP);
  _moves.push_back(bishopPromotion);

  Move knightPromotion = promotionBase;
  knightPromotion.setPromotionPieceType(KNIGHT);
  _moves.push_back(knightPromotion);
}

void MoveGen::_genWhitePawnSingleMoves(const Board &board) {
  U64 movedPawns = board.getPieces(WHITE, PAWN) << 8;
  movedPawns &= board.getNotOccupied();

  U64 promotions = movedPawns & RANK_8;
  movedPawns &= ~RANK_8;

  // Generate single non promotion moves
  while (movedPawns) {
    int to = _popLsb(movedPawns);
    _moves.push_back(Move(to - 8, to, PAWN));
  }

  // Generate promotions
  while (promotions) {
    int to = _popLsb(promotions);
    _genPawnPromotions(to - 8, to);
  }
}

inline void MoveGen::_getPromQonly(const Board &board, Color color){
  U64 promotions = color == WHITE ? board.getPieces(color, PAWN) << 8 : board.getPieces(color, PAWN) >> 8;
  promotions &= board.getNotOccupied();
  promotions &= color == WHITE ? RANK_8 : RANK_1;
  int fromAdj = color == WHITE ? -8 : 8;

  // Generate promotions
  while (promotions) {
    int to = _popLsb(promotions);
    Move m = Move(to + fromAdj, to, PAWN, Move::PROMOTION);
    m.setPromotionPieceType(QUEEN);
    _moves.push_back(m);
  }
}

void MoveGen::_genWhitePawnDoubleMoves(const Board &board) {
  U64 singlePushes = (board.getPieces(WHITE, PAWN) << 8) & board.getNotOccupied();
  U64 doublePushes = (singlePushes << 8) & board.getNotOccupied() & RANK_4;

  while (doublePushes) {
    int to = _popLsb(doublePushes);
    _moves.push_back(Move(to - 16, to, PAWN, Move::DOUBLE_PAWN_PUSH));
  }
}

inline void MoveGen::_genPawnAttacks(const Board &board, Color color) {
    U64 promRank = color == WHITE ? RANK_8 : RANK_1;
    Color otherColor = getOppositeColor(color);
    U64 leftRegularAttacks = color == WHITE ? (board.getPieces(color, PAWN) << 7) & board.getAttackable(otherColor) & ~FILE_H
                                            : (board.getPieces(color, PAWN) >> 9) & board.getAttackable(otherColor) & ~FILE_H;
    U64 leftAttackPromotions = leftRegularAttacks & promRank;
    leftRegularAttacks &= ~promRank;
    int fromAdj = color == WHITE ? -7 : 9;

    U64 leftEnPassant = color == WHITE  ? (board.getPieces(color, PAWN) << 7) & board.getEnPassant() & ~FILE_H
                                        : (board.getPieces(color, PAWN) >> 9) & board.getEnPassant() & ~FILE_H;

  // Add regular attacks (Not promotions or en passants)
  while (leftRegularAttacks) {
    int to = _popLsb(leftRegularAttacks);

    Move move = Move(to + fromAdj, to, PAWN, Move::CAPTURE);
    move.setCapturedPieceType(board.getPieceAtSquare(otherColor, to));

    _moves.push_back(move);
  }

  // Add promotion attacks
  while (leftAttackPromotions) {
    int to = _popLsb(leftAttackPromotions);
    _genPawnPromotions(to + fromAdj, to, Move::CAPTURE, board.getPieceAtSquare(otherColor, to));
  }

  // Add en passant attacks
  // There can only be one en passant square at a time, so no need for loop
  if (leftEnPassant) {
    int to = _popLsb(leftEnPassant);
    _moves.push_back(Move(to + fromAdj, to, PAWN, Move::EN_PASSANT));
  }

    U64 rightRegularAttacks = color == WHITE ? (board.getPieces(color, PAWN) << 9) & board.getAttackable(otherColor) & ~FILE_A
                                            : (board.getPieces(color, PAWN) >> 7) & board.getAttackable(otherColor) & ~FILE_A;
    U64 rightAttackPromotions = rightRegularAttacks & promRank;
    rightRegularAttacks &= ~promRank;

    U64 rightEnPassant = color == WHITE ? (board.getPieces(color, PAWN) << 9) & board.getEnPassant() & ~FILE_A
                                        : (board.getPieces(color, PAWN) >> 7) & board.getEnPassant() & ~FILE_A;
    fromAdj = color == WHITE ? -9 : 7;

  // Add regular attacks (Not promotions or en passants)
  while (rightRegularAttacks) {
    int to = _popLsb(rightRegularAttacks);

    Move move = Move(to + fromAdj, to, PAWN, Move::CAPTURE);
    move.setCapturedPieceType(board.getPieceAtSquare(otherColor, to));

    _moves.push_back(move);
  }

  // Add promotion attacks
  while (rightAttackPromotions) {
    int to = _popLsb(rightAttackPromotions);
    _genPawnPromotions(to + fromAdj, to, Move::CAPTURE, board.getPieceAtSquare(otherColor, to));
  }

  // Add en passant attacks
  // There can only be one en passant square at a time, so no need for loop
  if (rightEnPassant) {
    int to = _popLsb(rightEnPassant);
    _moves.push_back(Move(to + fromAdj, to, PAWN, Move::EN_PASSANT));
  }
}

void MoveGen::_genBlackPawnSingleMoves(const Board &board) {
  U64 movedPawns = board.getPieces(BLACK, PAWN) >> 8;
  movedPawns &= board.getNotOccupied();

  U64 promotions = movedPawns & RANK_1;
  movedPawns &= ~RANK_1;

  // Generate single non promotion moves'
  while (movedPawns) {
    int to = _popLsb(movedPawns);
    _moves.push_back(Move(to + 8, to, PAWN));
  }

  // Generate promotions
  while (promotions) {
    int to = _popLsb(promotions);
    _genPawnPromotions(to + 8, to);
  }
}

void MoveGen::_genBlackPawnDoubleMoves(const Board &board) {
  U64 singlePushes = (board.getPieces(BLACK, PAWN) >> 8) & board.getNotOccupied();
  U64 doublePushes = (singlePushes >> 8) & board.getNotOccupied() & RANK_5;

  while (doublePushes) {
    int to = _popLsb(doublePushes);
    _moves.push_back(Move(to + 16, to, PAWN, Move::DOUBLE_PAWN_PUSH));
  }
}

void MoveGen::_genWhitePawnMoves(const Board &board) {
  _genWhitePawnSingleMoves(board);
  _genWhitePawnDoubleMoves(board);
  _genPawnAttacks(board, board.getActivePlayer());
}

void MoveGen::_genBlackPawnMoves(const Board &board) {
  _genBlackPawnSingleMoves(board);
  _genBlackPawnDoubleMoves(board);
  _genPawnAttacks(board, board.getActivePlayer());
}

void MoveGen::_genWhiteKingMoves(const Board &board) {
  _genKingMoves(board, board.getPieces(WHITE, KING), board.getAttackable(BLACK));

  if (board.whiteCanCastleKs()) {
    _moves.push_back(Move(e1, g1, KING, Move::KSIDE_CASTLE));
  }
  if (board.whiteCanCastleQs()) {
    _moves.push_back(Move(e1, c1, KING, Move::QSIDE_CASTLE));
  }
}

void MoveGen::_genBlackKingMoves(const Board &board) {
  _genKingMoves(board, board.getPieces(BLACK, KING), board.getAttackable(WHITE));

  if (board.blackCanCastleKs()) {
    _moves.push_back(Move(e8, g8, KING, Move::KSIDE_CASTLE));
  }
  if (board.blackCanCastleQs()) {
    _moves.push_back(Move(e8, c8, KING, Move::QSIDE_CASTLE));
  }
}

void MoveGen::_genKingMoves(const Board &board, U64 king, U64 attackable) {
  if (king == 0) {
    return;
  }

  int kingIndex = _bitscanForward(king);

  U64 moves = board.getAttacksForSquare(KING, board.getActivePlayer(), kingIndex);

  _addMoves(board, kingIndex, KING, moves, attackable);
}

inline void MoveGen::_genKingCaps(const Board &board, U64 king, U64 attackable) {
  if (king == 0) {
    return;
  }

  int kingIndex = _bitscanForward(king);

  U64 moves = board.getAttacksForSquare(KING, board.getActivePlayer(), kingIndex);

  _addCaps(board, kingIndex, KING, moves, attackable);
}

void MoveGen::_genWhiteKnightMoves(const Board &board) {
  _genKnightMoves(board, board.getPieces(WHITE, KNIGHT), board.getAttackable(BLACK));
}

void MoveGen::_genBlackKnightMoves(const Board &board) {
  _genKnightMoves(board, board.getPieces(BLACK, KNIGHT), board.getAttackable(WHITE));
}

void MoveGen::_genKnightMoves(const Board &board, U64 knights, U64 attackable) {
  while (knights) {
    int from = _popLsb(knights);

    U64 moves = board.getAttacksForSquare(KNIGHT, board.getActivePlayer(), from);

    _addMoves(board, from, KNIGHT, moves, attackable);
  }
}

inline void MoveGen::_genKnightCaps(const Board &board, U64 knights, U64 attackable) {
  while (knights) {
    int from = _popLsb(knights);

    U64 moves = board.getAttacksForSquare(KNIGHT, board.getActivePlayer(), from);

    _addCaps(board, from, KNIGHT, moves, attackable);
  }
}

void MoveGen::_genWhiteBishopMoves(const Board &board) {
  _genBishopMoves(board, board.getPieces(WHITE, BISHOP), board.getAttackable(BLACK));
}

void MoveGen::_genBlackBishopMoves(const Board &board) {
  _genBishopMoves(board, board.getPieces(BLACK, BISHOP), board.getAttackable(WHITE));
}

void MoveGen::_genBishopMoves(const Board &board, U64 bishops, U64 attackable) {
  while (bishops) {
    int from = _popLsb(bishops);

    U64 moves = board.getAttacksForSquare(BISHOP, board.getActivePlayer(), from);

    _addMoves(board, from, BISHOP, moves, attackable);
  }
}

inline void MoveGen::_genBishopCaps(const Board &board, U64 bishops, U64 attackable) {
  while (bishops) {
    int from = _popLsb(bishops);

    U64 moves = board.getAttacksForSquare(BISHOP, board.getActivePlayer(), from);

    _addCaps(board, from, BISHOP, moves, attackable);
  }
}

void MoveGen::_genWhiteRookMoves(const Board &board) {
  _genRookMoves(board, board.getPieces(WHITE, ROOK), board.getAttackable(BLACK));
}

void MoveGen::_genBlackRookMoves(const Board &board) {
  _genRookMoves(board, board.getPieces(BLACK, ROOK), board.getAttackable(WHITE));
}

void MoveGen::_genRookMoves(const Board &board, U64 rooks, U64 attackable) {
  while (rooks) {
    int from = _popLsb(rooks);

    U64 moves = board.getAttacksForSquare(ROOK, board.getActivePlayer(), from);

    _addMoves(board, from, ROOK, moves, attackable);
  }
}

inline void MoveGen::_genRookCaps(const Board &board, U64 rooks, U64 attackable) {
  while (rooks) {
    int from = _popLsb(rooks);

    U64 moves = board.getAttacksForSquare(ROOK, board.getActivePlayer(), from);

    _addCaps(board, from, ROOK, moves, attackable);
  }
}

void MoveGen::_genWhiteQueenMoves(const Board &board) {
  _genQueenMoves(board, board.getPieces(WHITE, QUEEN), board.getAttackable(BLACK));
}

void MoveGen::_genBlackQueenMoves(const Board &board) {
  _genQueenMoves(board, board.getPieces(BLACK, QUEEN), board.getAttackable(WHITE));
}

void MoveGen::_genQueenMoves(const Board &board, U64 queens, U64 attackable) {
  while (queens) {
    int from = _popLsb(queens);

    U64 moves = board.getAttacksForSquare(QUEEN, board.getActivePlayer(), from);

    _addMoves(board, from, QUEEN, moves, attackable);
  }
}

inline void MoveGen::_genQueenCaps(const Board &board, U64 queens, U64 attackable) {
  while (queens) {
    int from = _popLsb(queens);

    U64 moves = board.getAttacksForSquare(QUEEN, board.getActivePlayer(), from);

    _addCaps(board, from, QUEEN, moves, attackable);
  }
}

void MoveGen::_addMoves(const Board &board, int from, PieceType pieceType, U64 moves, U64 attackable) {
  // Ignore all moves/attacks to kings
  moves &= ~(board.getPieces(board.getInactivePlayer(), KING));

  // Generate non attacks
  U64 nonAttacks = moves & ~attackable;
  while (nonAttacks) {
    int to = _popLsb(nonAttacks);
    _moves.push_back(Move(from, to, pieceType));
  }

  // Generate attacks
  U64 attacks = moves & attackable;
  while (attacks) {
    int to = _popLsb(attacks);

    Move move(from, to, pieceType, Move::CAPTURE);
    move.setCapturedPieceType(board.getPieceAtSquare(board.getInactivePlayer(), to));

    _moves.push_back(move);
  }
}

inline void MoveGen::_addCaps(const Board &board, int from, PieceType pieceType, U64 moves, U64 attackable) {
  // Ignore all moves/attacks to kings
  moves &= ~(board.getPieces(board.getInactivePlayer(), KING));

  // Generate attacks
  U64 attacks = moves & attackable;
  while (attacks) {
    int to = _popLsb(attacks);

    Move move(from, to, pieceType, Move::CAPTURE);
    move.setCapturedPieceType(board.getPieceAtSquare(board.getInactivePlayer(), to));
    _moves.push_back(move);
  }
}
