#include "defs.h"
#include "search.h"
#include "eval.h"
#include "movepicker.h"
#include "generalmovepicker.h"
#include "qsearchmovepicker.h"
#include <algorithm>
#include <iostream>

//search_constants
//
const int NULL_MOVE_REDUCTION = 3;
const int DELTA_MOVE_CONST = 200;
//

//
int selDepth = 0; // int that is showing maxDepth with extentions we reached in the search
//

extern int g_TT_MO_hit;
extern HASH myHASH;

Search::Search(const Board &board, Limits limits, std::vector<ZKey> positionHistory, bool logUci) :
    _positionHistory(positionHistory),
    _orderingInfo(OrderingInfo()),
    _limits(limits),
    _initialBoard(board),
    _logUci(logUci),
    _stop(false),
    _limitCheckCount(0),
    _bestScore(0) {

  if (_limits.infinite) { // Infinite search
    _searchDepth = INF;
    _timeAllocated = INF;
  } else if (_limits.depth != 0) { // Depth search
    _searchDepth = _limits.depth;
    _timeAllocated = INF;
  } else if (_limits.moveTime != 0) {
    _searchDepth = MAX_SEARCH_DEPTH;
    _timeAllocated = _limits.moveTime;
  } else if (_limits.time[_initialBoard.getActivePlayer()] != 0) { // Time search
    int ourTime = _limits.time[_initialBoard.getActivePlayer()];
    int opponentTime = _limits.time[_initialBoard.getInactivePlayer()];

    // Divide up the remaining time (If movestogo not specified we are in 
    // sudden death)
    if (_limits.movesToGo == 0) {
      // Allocate less time for this search if our opponent's time is greater
      // than our time by scaling movestogo by the ratio between our time
      // and our opponent's time (ratio max forced to 2.0, min forced to 1.0)
      double timeRatio = std::max((double) (ourTime / opponentTime), 1.0);

      int movesToGo = (int) (SUDDEN_DEATH_MOVESTOGO * std::min(2.0, timeRatio));
      _timeAllocated = ourTime / movesToGo;
    } else {
      // A small constant (3) is added to _limits.movesToGo when dividing to
      // ensure we don't go over time when movesToGo is small
      _timeAllocated = ourTime / (_limits.movesToGo + 3);
    }

    // Use all of the increment to think
    _timeAllocated += _limits.increment[_initialBoard.getActivePlayer()];

    // Depth is infinity in a timed search (ends when time runs out)
    _searchDepth = MAX_SEARCH_DEPTH;
  } else { // No limits specified, use default depth
    _searchDepth = DEFAULT_SEARCH_DEPTH;
    _timeAllocated = INF;
  }

  // Debug_evaluation_paste_below:
    int k = myHASH.HASH_Size();
  // std::cout << "Castle_test_ " + std::to_string(k);
    std::cout << "HASH_size " + std::to_string(k);
    std::cout << std::endl;
}

void Search::iterDeep() {
  _start = std::chrono::steady_clock::now();
  _nodes = 0;
  selDepth = 0;
  for (int currDepth = 1; currDepth <= _searchDepth; currDepth++) {
    _rootMax(_initialBoard, currDepth, 0);

    int elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();

    // If limits were exceeded in the search, break without logging UCI info (search was incomplete)
    if (_stop) break;

    if (_logUci) {
      _logUciInfo(_getPv(currDepth), currDepth, _bestScore, _nodes, elapsed);
    }

    // If the last search has exceeded or hit 50% of the allocated time, stop searching
    if (elapsed >= (_timeAllocated / 2)) break;
  }

  if (_logUci) std::cout << "bestmove " << getBestMove().getNotation() << std::endl;
}

MoveList Search::_getPv(int length) {
  MoveList pv;
  Board currBoard = _initialBoard;
  HASH_Entry currHASH;
  int currLength = 0;

  while (currLength++ < length) {
    currHASH = myHASH.HASH_Get(currBoard.getZKey().getValue());
    if (currHASH.Flag == NONE){
      break;
    }
    pv.push_back(Move(currHASH.move));
    currBoard.doMove(Move(currHASH.move));
  }

  return pv;
}

void Search::_logUciInfo(const MoveList &pv, int depth, int bestScore, int nodes, int elapsed) {
  std::string pvString;
  for (auto move : pv) {
    pvString += move.getNotation() + " ";
  }

  std::string scoreString;
  if (bestScore == LOST_SCORE) {
    scoreString = "mate " + std::to_string(pv.size());
  } else if (_bestScore == -LOST_SCORE) {
    scoreString = "mate -" + std::to_string(pv.size());
  } else {
    scoreString = "cp " + std::to_string(bestScore);
  }

  // Avoid divide by zero errors with nps
  elapsed++;
  // Avoid selDepth being smaller than depth when entire path to score is in TT
  selDepth = std::max(depth, selDepth);
  
  std::cout << "info depth " + std::to_string(depth) + " ";
  std::cout << "seldepth " + std::to_string(selDepth) + " ";
  std::cout << "nodes " + std::to_string(nodes) + " ";
  std::cout << "score " + scoreString + " ";
  std::cout << "nps " + std::to_string(nodes * 1000 / elapsed) + " ";
  std::cout << "time " + std::to_string(elapsed) + " ";
  std::cout << "pv " + pvString;
  //std::cout << "MO_hits " + std::to_string(g_TT_MO_hit);
  std::cout << std::endl;
}

void Search::stop() {
  _stop = true;
}

Move Search::getBestMove() {
  return _bestMove;
}

// эта штука - запрос к времени раз в сколько-то позиций
bool Search::_checkLimits() {

  if (--_limitCheckCount > 0) {
    return false;
  }

  _limitCheckCount = 4096;

  int elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();

  if (_limits.nodes != 0 && (_nodes >= _limits.nodes)) return true;
  if (elapsed >= (_timeAllocated)) return true;

  return false;
}

void Search::_rootMax(const Board &board, int depth, int ply) {
  _nodes++;
  
  MoveGen movegen(board, false);
  MoveList legalMoves = movegen.getMoves();

  // If no legal moves are available, just return, setting bestmove to a null move
  if (legalMoves.empty()) {
    _bestMove = Move();
    _bestScore = LOST_SCORE;
    return;
  }

  GeneralMovePicker movePicker
      (&_orderingInfo, const_cast<Board *>(&board), &legalMoves);

  int alpha = LOST_SCORE;
  int beta = -LOST_SCORE;

  int currScore;

  Move bestMove;
  bool fullWindow = true;
  while (movePicker.hasNext()) {
    Move move = movePicker.getNext();

    Board movedBoard = board;
    movedBoard.doMove(move);
    if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        _orderingInfo.incrementPly();
        if (fullWindow) {
          currScore = -_negaMax(movedBoard, depth - 1, -beta, -alpha, ply + 1, false);
        } else {
          currScore = -_negaMax(movedBoard, depth - 1, -alpha - 1, -alpha, ply +1, false);
          if (currScore > alpha) currScore = -_negaMax(movedBoard, depth - 1, -beta, -alpha, ply + 1, false);
        }
        _orderingInfo.deincrementPly();

        if (_stop || _checkLimits()) {
          _stop = true;
          break;
        }

        // If the current score is better than alpha, or this is the first move in the loop
        if (currScore > alpha) {
          fullWindow = false;
          bestMove = move;
          alpha = currScore;

          // Break if we've found a checkmate
        }
    
    }

  }

  // If the best move was not set in the main search loop
  // alpha was not raised at any point, just pick the first move
  // avaliable (arbitrary) to avoid putting a null move in the
  // transposition table
  if (bestMove.getFlags() & Move::NULL_MOVE) {
    bestMove = legalMoves.at(0);
  }


  if (!_stop) {
    myHASH.HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), EXACT, alpha, depth, ply);
    _bestMove = bestMove;
    _bestScore = alpha;
  }
}

// this is basically my main search
// 
int Search::_negaMax(const Board &board, int depth, int alpha, int beta, int ply, bool doNool) {
  // Check search limits
  _nodes++;
  bool AreWeInCheck;
  int score;
  
  if (_stop || _checkLimits()) {
    _stop = true;
    return 0;
  }

  // Check for threefold repetition draws
  if (std::find(_positionHistory.begin(), _positionHistory.end(), board.getZKey()) != _positionHistory.end()) {
    return 0;
  }

  // Check for 50 move rule draws
  if (board.getHalfmoveClock() >= 100) {
    
    return 0;
  }

  int alphaOrig = alpha;
  // Check transposition table cache

  const HASH_Entry probedHASHentry = myHASH.HASH_Get(board.getZKey().getValue());

  if (probedHASHentry.Flag != NONE){
    if (probedHASHentry.depth >= depth){
      int hashScore = probedHASHentry.score;
      if (abs(hashScore)+50 > LOST_SCORE * -1){
        if (hashScore > 0){
          hashScore -= ply;
        }
        if (hashScore < 0){
          hashScore += ply;
        }

      }
      if (probedHASHentry.Flag == EXACT){
        return hashScore;
      }
      if (probedHASHentry.Flag == ALPHA && hashScore <= alpha){
        return alpha;
      }
      if (probedHASHentry.Flag == BETA && hashScore >= beta){
        return beta;
      }
    }
  }
  // Transposition table lookups are inconclusive, try null move
  
  AreWeInCheck = board.colorIsInCheck(board.getActivePlayer());
  if (ply > 0 && depth >= 3 && !doNool && !AreWeInCheck && board.isThereMajorPiece()){
          Board movedBoard = board;
          movedBoard.doNool();
          int score = -_negaMax(movedBoard, depth - NULL_MOVE_REDUCTION, -beta, -beta +1, ply + 1, true );
          if (score >= beta){
            return beta;
          }
  }

  // Transposition table lookups are inconclusive, generate moves and recurse
  MoveGen movegen(board, false);
  MoveList legalMoves = movegen.getMoves();
  int Extension = 0;

  // Extentions are summed up here
  // InCheck extentions - we extend when the sideToMove is inCheck
  if (AreWeInCheck) {
    Extension++;
  }

  // Eval if depth is 0
  if ((depth + Extension) == 0) {
    selDepth = std::max(ply, selDepth);
    return _qSearch(board, alpha, beta, ply + 1 );
  }

  GeneralMovePicker movePicker
      (&_orderingInfo, const_cast<Board *>(&board), &legalMoves);

  Move bestMove;
  bool fullWindow = true;
  int  LegalMoveCount = 0;
  // вероятно не самая эффективная конструкция, но оптимизация потом
  while (movePicker.hasNext()) {

    Move move = movePicker.getNext();
    Board movedBoard = board;
    movedBoard.doMove(move);
    
      if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        LegalMoveCount++;
        int score;
        _orderingInfo.incrementPly();

        if (fullWindow) {
          score = -_negaMax(movedBoard, depth - 1 + Extension, -beta, -alpha, ply + 1, false );
        } else {
          score = -_negaMax(movedBoard, depth - 1 + Extension, -alpha - 1, -alpha, ply + 1, false);
          if (score > alpha) score = -_negaMax(movedBoard, depth - 1 + Extension, -beta, -alpha, ply + 1, false );
        }
        _orderingInfo.deincrementPly();

        // Beta cutoff
        if (score >= beta) {
          // Add this move as a new killer move and update history if move is quiet
          if (!(move.getFlags() & Move::CAPTURE)) {
          _orderingInfo.updateKillers(_orderingInfo.getPly(), move);
          }

          // Add a new tt entry for this node
          if (!_stop){
            myHASH.HASH_Store(board.getZKey().getValue(), move.getMoveINT(), BETA, score, depth, ply);
          }
          return beta;
        }

        // Check if alpha raised (new best move)
        if (score > alpha) {
          if (!(move.getFlags() & Move::CAPTURE)){
              _orderingInfo.incrementHistory(board.getActivePlayer(), move.getFrom(), move.getTo(), depth);
          }
          fullWindow = false;
          alpha = score;
          bestMove = move;
        }
      } 

  }

  // Check for checkmate and stalemate
  if (LegalMoveCount == 0) {
    score = AreWeInCheck ? LOST_SCORE + ply : 0; // LOST_SCORE = checkmate, 0 = stalemate (draw)
    return score;
  }

  // If the best move was not set in the main search loop
  // alpha was not raised at any point, just pick the first move
  // avaliable (arbitrary) to avoid putting a null move in the
  // transposition table
  if (bestMove.getFlags() & Move::NULL_MOVE) {
    bestMove = legalMoves.at(0);
  }

  // Store bestScore in transposition table
  if (!_stop){
      if (alpha <= alphaOrig) {
        myHASH.HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), ALPHA, alpha, depth, ply);
      } else {
        myHASH.HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), EXACT, alpha, depth, ply);
      }
  }



  return alpha;
}
// end basic search

// qsearch
int Search::_qSearch(const Board &board, int alpha, int beta, int ply) {
  // Check search limits
   _nodes++;
  if (_stop || _checkLimits()) {
    _stop = true;
    return 0;
  }

  int standPat = Eval::evaluate(board, board.getActivePlayer());

  if (standPat >= beta) {
    return beta;
  }
  
  if (alpha < standPat) {
    alpha = standPat;
  }

  MoveGen movegen(board, true);
  MoveList legalMoves = movegen.getMoves();
  QSearchMovePicker movePicker(&legalMoves);

  // If node is quiet, just return eval
  if (!movePicker.hasNext()) {
    return standPat;
  }


  while (movePicker.hasNext()) {
    Move move = movePicker.getNext();

    // DELTA MOVE PRUNING. Prune here if were are very far ahead.
    int moveGain = Eval::MATERIAL_VALUES[0][move.getCapturedPieceType()];
    if (standPat + moveGain + DELTA_MOVE_CONST < alpha)
      continue;

    Board movedBoard = board;
    movedBoard.doMove(move);
      if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){

          int score = -_qSearch(movedBoard, -beta, -alpha, ply + 1);

          if (score >= beta) {
            return beta;
          }
          if (score > alpha) {
            alpha = score;
          }
        }


  }
  return alpha;
}

//end