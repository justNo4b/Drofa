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

MoveList * MoveGen::getMoves() {
  return  &_moves;
}

void MoveGen::_genMoves(const Board &board) {
    _moves.reserve(MOVELIST_RESERVE_SIZE);
    Color color = board.getActivePlayer();
    Color otherColor = getOppositeColor(color);

    _genPawnMoves(board, color);
    _genPawnAttacks(board, color);

    for (auto pt : {ROOK, KNIGHT, BISHOP, QUEEN}){
        _genPieceMoves(board, pt, board.getPieces(color, pt), board.getAttackable(otherColor));
    }
    _genKingMoves(board, color, board.getPieces(color, KING), board.getAttackable(otherColor));
}

void MoveGen::_genCaptures(const Board &board) {
    _moves.reserve(MOVELIST_RESERVE_SIZE);
    Color color = board.getActivePlayer();
    Color otherColor = getOppositeColor(color);

    _genPawnAttacks(board, color);
    _getPromQonly(board, color);
    for (auto pt : {ROOK, KNIGHT, BISHOP, QUEEN, KING}){
        _genPieceCaps(board, pt, board.getPieces(color, pt), board.getAttackable(otherColor));
    }
}

inline void MoveGen::_genPawnPromotions(unsigned int from, unsigned int to, unsigned int flags, PieceType capturedPieceType) {
    Move promotionBase = Move(from, to, PAWN, flags | Move::PROMOTION);
    if (flags & Move::CAPTURE) {
        promotionBase.setCapturedPieceType(capturedPieceType);
    }

    for (auto ptype : {ROOK, KNIGHT, BISHOP, QUEEN}){
        Move ptPromos = promotionBase;
        ptPromos.setPromotionPieceType(ptype);
        _moves.push_back(ptPromos);
    }
}

void MoveGen::_genPawnMoves(const Board &board, Color color) {
  U64 movedPawns = color == WHITE ? board.getPieces(color, PAWN) << 8
                                  : board.getPieces(color, PAWN) >> 8;
  movedPawns &= board.getNotOccupied();
  U64 doublePushes = color == WHITE ? (movedPawns << 8) & board.getNotOccupied() & DOUBLE_PUSH_RANK[color]
                                    : (movedPawns >> 8) & board.getNotOccupied() & DOUBLE_PUSH_RANK[color];

  U64 promotions = movedPawns & PROMOTION_RANK[color];
  movedPawns &= ~PROMOTION_RANK[color];
  int fromAdj = color == WHITE ? -8 : 8;

  // Generate single non promotion moves
  while (movedPawns) {
    int to = _popLsb(movedPawns);
    _moves.push_back(Move(to + fromAdj, to, PAWN));
  }

  // Generate promotions
  while (promotions) {
    int to = _popLsb(promotions);
    _genPawnPromotions(to + fromAdj, to);
  }

  while (doublePushes) {
    int to = _popLsb(doublePushes);
    _moves.push_back(Move(to + (2 * fromAdj), to, PAWN, Move::DOUBLE_PAWN_PUSH));
  }
}

inline void MoveGen::_getPromQonly(const Board &board, Color color){
  U64 promotions = color == WHITE ? board.getPieces(color, PAWN) << 8 : board.getPieces(color, PAWN) >> 8;
  promotions &= board.getNotOccupied();
  promotions &= PROMOTION_RANK[color];
  int fromAdj = color == WHITE ? -8 : 8;

  // Generate promotions
  while (promotions) {
    int to = _popLsb(promotions);
    Move m = Move(to + fromAdj, to, PAWN, Move::PROMOTION);
    m.setPromotionPieceType(QUEEN);
    _moves.push_back(m);
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

inline void MoveGen::_genKingMoves(const Board &board, Color color, U64 king, U64 attackable) {
    // Add normal moves
    int kingIndex = _bitscanForward(king);
    U64 moves = board.getAttacksForSquare(KING, board.getActivePlayer(), kingIndex);
    _addMoves(board, kingIndex, KING, moves, attackable);

    // Add Castlings
    switch (color)
    {
    case WHITE:
        if (board.whiteCanCastleKs()) {
            _moves.push_back(Move(e1, g1, KING, Move::KSIDE_CASTLE));
        }
        if (board.whiteCanCastleQs()) {
            _moves.push_back(Move(e1, c1, KING, Move::QSIDE_CASTLE));
        }
        break;

    case BLACK:
        if (board.blackCanCastleKs()) {
            _moves.push_back(Move(e8, g8, KING, Move::KSIDE_CASTLE));
        }
        if (board.blackCanCastleQs()) {
            _moves.push_back(Move(e8, c8, KING, Move::QSIDE_CASTLE));
        }
        break;
    }
}

inline void MoveGen::_genPieceMoves(const Board &board, PieceType pType, U64 pieces, U64 attackable){
    while (pieces) {
        int from = _popLsb(pieces);

        U64 moves = board.getAttacksForSquare(pType, board.getActivePlayer(), from);

        _addMoves(board, from, pType, moves, attackable);
    }
}

inline void MoveGen::_genPieceCaps(const Board &board, PieceType pType, U64 pieces, U64 attackable){
    while (pieces) {
        int from = _popLsb(pieces);

        U64 moves = board.getAttacksForSquare(pType, board.getActivePlayer(), from);

        _addCaps(board, from, pType, moves, attackable);
    }
}

inline void MoveGen::_addMoves(const Board &board, int from, PieceType pieceType, U64 moves, U64 attackable) {
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
  // Generate attacks
  U64 attacks = moves & attackable;
  while (attacks) {
    int to = _popLsb(attacks);

    Move move(from, to, pieceType, Move::CAPTURE);
    move.setCapturedPieceType(board.getPieceAtSquare(board.getInactivePlayer(), to));
    _moves.push_back(move);
  }
}
