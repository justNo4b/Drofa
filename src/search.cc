#include "defs.h"
#include "search.h"
#include "eval.h"
#include "movepicker.h"
#include "generalmovepicker.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <math.h>

//search_constants
//
const int NULL_MOVE_REDUCTION = 3;
const int DELTA_MOVE_CONST = 200;
const int FUTIL_MOVE_CONST = 150;
const int REVF_MOVE_CONST = 200;
//

//
int selDepth = 0; // int that is showing maxDepth with extentions we reached in the search
//

extern int g_TT_MO_hit;
extern HASH myHASH;

// i here is DETPTH
// j here is moveNUM
// we scale R higher for moveNUM than for DEPTH
void Search::init_LMR_array(){
  for (int i = 0; i < 34; i++){
    for (int j = 0; j< 34; j++){
      _lmr_R_array[i][j] = 0.1 + (pow(i, 0.15) * pow(j, 0.15))/1.75;
    }
  }
}

Search::Search(const Board &board, Limits limits, std::vector<ZKey> positionHistory, bool logUci) :
    _positionHistory(positionHistory),
    _orderingInfo(OrderingInfo()),
    _limits(limits),
    _initialBoard(board),
    _logUci(logUci),
    _stop(false),
    _limitCheckCount(0),
    _bestScore(0) {

  std::memset(_sEvalArray, 0, sizeof(_sEvalArray));
  init_LMR_array();
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
  //  std::cout << "Castle_test_ " + std::to_string(k);
    std::cout << "HASH_size " + std::to_string(_lmr_R_array[3][7]);
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

  _sEvalArray[ply] = board.colorIsInCheck(board.getActivePlayer()) ? NOSCORE : Eval::evaluate(board, board.getActivePlayer());

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
  bool pvNode = alpha != beta - 1;
  bool TTmove = false;

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
    TTmove = true;
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
  
  // Extentions are summed up here
  // InCheck extentions - we extend when the sideToMove is inCheck
  // If we are not in fact in check, evaluate a board for pruning later
  // We dont evaluate when in check because we dont prune in check
  int Extension = 0;
  int statEVAL = 0;

  AreWeInCheck = board.colorIsInCheck(board.getActivePlayer());

  if (AreWeInCheck) {
    Extension++;
    _sEvalArray[ply] = NOSCORE;
  }else{
    statEVAL = Eval::evaluate(board, board.getActivePlayer());
    _sEvalArray[ply] = statEVAL;
  }
  // Go into the QSearch if depth is 0
  if ((depth + Extension) == 0) {
    selDepth = std::max(ply, selDepth);
    return _qSearch(board, alpha, beta, ply + 1 );
  }
  // Check if we are improving
  // The idea is if we are not improving in this line
  // We probably can prune a bit more
  bool improving = false;
  if (ply > 2)
    improving = !AreWeInCheck && statEVAL > _sEvalArray[ply - 2];  

  // 1. REVERSE FUTILITY
  // The idea is so if we are very far ahead of beta at low
  // depth, we can just return estimated eval (eval - margin),
  // because beta probably wont be beaten
  // 
  // For now dont Prune in PV, in check, and at high depth
  // btw d < 5 is totally arbitrary, tune it later maybe

  if (!pvNode && Extension == 0 && depth < 5){
      if ((statEVAL - REVF_MOVE_CONST * depth + 100 * improving) >= beta)
      return statEVAL - REVF_MOVE_CONST * depth + 100 * improving;
  }

  // 2. NULL MOVE
  // If we are doing so well, that giving opponent 2
  // moves wont improve his position
  // we can safely prune this position
  //
  // For obvious reasons its turned off with no major pieces
  // and when we are in check
  if (ply > 0 && depth >= 3 && !doNool && !AreWeInCheck && board.isThereMajorPiece()){
          Board movedBoard = board;
          movedBoard.doNool();
          int score = -_negaMax(movedBoard, depth - NULL_MOVE_REDUCTION - depth/4, -beta, -beta +1, ply + 1, true);
          if (score >= beta){
            return beta;
          }
  }

  // 3. UN_HASHED REDUCTION
  // We reduce depth by 1 if the position we currently 
  // analysing isnt hashed.
  // Based on talkchess discussion, replaces 
  // Internal iterative deepening.
  // 
  // The justification is if our hashing is decent, if the 
  // position at high depth isnt here, its probably position 
  // not worth searching
  if (depth >= 5 && !TTmove)
    depth--;

  // No pruning occured, generate moves and recurse
  MoveGen movegen(board, false);
  MoveList legalMoves = movegen.getMoves();
  GeneralMovePicker movePicker
      (&_orderingInfo, const_cast<Board *>(&board), &legalMoves);

  Move bestMove;
  int  LegalMoveCount = 0;
  int  qCount = 0;
  // вероятно не самая эффективная конструкция, но оптимизация потом
  while (movePicker.hasNext()) {

    Move move = movePicker.getNext();

    // LATE MOVE PRUNING
    // If we made many quiet moves in the position already
    // we suppose other moves wont improve our situation
    //
    // Formula from Weiss, weirdly working, searchdepth 
    // is way up, elo gain is not so great

    if (!pvNode && Extension == 0 && qCount > 3 + (depth*depth*2)/(2-improving)){
      continue;
    }

    Board movedBoard = board;
    movedBoard.doMove(move);
    bool doLMR = false;

      if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        LegalMoveCount++;
        int score;

        bool giveCheck = movedBoard.colorIsInCheck(movedBoard.getActivePlayer());
        bool isQuiet = !move.getFlags();
        if (isQuiet)
          qCount++;
        // EXTENDED FUTILITY PRUNING
        // We try to pune a move, if depth is low (1 or 2)
        // Move should not give check, and should not be the first move
        // we also should not be in check
        // We do not prune in the PV nodes.
        // We also do not prune if we are close to the MATE

        if (!pvNode && Extension == 0 && LegalMoveCount > 1 && depth < 3 
        && !giveCheck && alpha < ((LOST_SCORE * -1) - 50)){
          int moveGain = Eval::MATERIAL_VALUES[0][move.getCapturedPieceType()];
          if (statEVAL + FUTIL_MOVE_CONST * depth + moveGain - 100 * improving <= alpha){
              continue;
          }
        }
        _orderingInfo.incrementPly();
        _positionHistory.push_back(board.getZKey());

        //LATE MOVE REDUCTIONS
        //mix of ideas from Weiss code and what is written in the chessprogramming wiki
        //
        //For now we dont reduce in the PV, if depth too low, when extention is triggered
        //and when move give check.
        //This can be a subject for a later tuning
        //
        //Currently we try to reduce 3rd move and beyond and 4th and beyond in the pvNode.

        doLMR = depth > 2 && LegalMoveCount > 2 + pvNode && Extension == 0 && !giveCheck;
        if (doLMR){

          //Basic reduction is done according to the array
          //Initiated at the ini() of the Search Class
          //Now mostly 0 -> 1
          int reduction = _lmr_R_array[std::min(33, depth)][std::min(33, LegalMoveCount)];

          //if move is quiet, reduce a bit more
          if (isQuiet){
            reduction++;
          }

          if (improving){
            reduction--;
          }
          //Avoid to reduce so much that we go to QSearch right away
          int fDepth = std::max(1, depth - 1 - reduction);
          
          //Search with reduced depth around alpha in assumtion
          // that alpha would not be beaten here
          score = -_negaMax(movedBoard, fDepth, -alpha - 1 , -alpha, ply + 1, false);
        }
        
        // Code here is restructured based on Weiss
        // First part is clear here: if we did LMR and score beats alpha
        // We need to do a re-search.
        // 
        // If we did not do LMR: if we are in a non-PV our we already have alpha == beta - 1,
        // and if we are searching 2nd move and so on we already did full window search - 
        // So for both of this cases we do limited window search. 
        //
        // This system is implemented instead of fullDepth = true/false basic approach.
        if (doLMR){
          if (score > alpha){
            score = -_negaMax(movedBoard, depth - 1 + Extension, -alpha - 1, -alpha, ply + 1, false);
          }
        } else if (!pvNode || LegalMoveCount > 1){
          score = -_negaMax(movedBoard, depth - 1 + Extension, -alpha - 1, -alpha, ply + 1, false);
        }

        // If we are in the PV 
        // Search with a full window the first move to calculate bounds
        // or if score improved alpha during the current round of search.
        if  (pvNode) {
          if ((LegalMoveCount == 1) || (score > alpha && score < beta)){
            score = -_negaMax(movedBoard, depth - 1 + Extension, -beta, -alpha, ply + 1, false );  
          }
        }

        _orderingInfo.deincrementPly();
        _positionHistory.pop_back();
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
  int mvpCost = board.MostFancyPieceCost();

  if (standPat >= beta) {
    return beta;
  }
    // DELTA MOVE PRUNING. Prune here if were are very far ahead.
  if (alpha > standPat + mvpCost){
    return alpha;
  }
  
  if (alpha < standPat) {
    alpha = standPat;
  }

  MoveGen movegen(board, true);
  MoveList legalMoves = movegen.getMoves();
  GeneralMovePicker movePicker
      (&_orderingInfo, const_cast<Board *>(&board), &legalMoves);

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