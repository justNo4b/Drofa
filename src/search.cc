#include "defs.h"
#include "search.h"
#include "eval.h"
#include "movepicker.h"
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
const int RAZORING_MARGIN = 650;
//

//
int selDepth = 0; // int that is showing maxDepth with extentions we reached in the search
//

extern int g_TT_MO_hit;
extern HASH myHASH;


void Search::init_LMR_array(){

  // 1. Initialization of the LMR_array.
  // Original formula, came up after plotting 
  // Weiss formula and trying to came up with 
  // something similar, but based on the pow (x,y)
  // function for easier tuning later.
  // i here is DETPTH
  // j here is moveNUM

  for (int i = 0; i < 34; i++){
    for (int j = 0; j< 34; j++){
      _lmr_R_array[i][j] = 0.1 + (pow(i, 0.15) * pow(j, 0.15))/1.75;
    }
  }
  // 2. Initialization of the LMP array.
  // Current formula is completely based on the 
  // Weiss chess engine.
  for (int i = 0; i < 99; i++){
    _lmp_Array[i] = 3 + pow( i, 2) * 2;
  }

}

Search::Search(const Board &board, Limits limits, Hist positionHistory, bool logUci) :
    _orderingInfo(OrderingInfo()),
    _limits(limits),
    _initialBoard(board),
    _logUci(logUci),
    _stop(false),
    _limitCheckCount(0),
    _bestScore(0) {

  std::memset(_sEvalArray, 0, sizeof(_sEvalArray));
  init_LMR_array();
  _wasThoughtProlonged = false;
  _posHist = positionHistory;
  int ourTime = 0;
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
    ourTime = _limits.time[_initialBoard.getActivePlayer()];
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
      // We substract 100 ms from time_allocated to make sure
      // We dont get out of time
      _timeAllocated = ourTime / (_limits.movesToGo);
      _timeAllocated-= 100;
    }

    // Use all of the increment to think
    _timeAllocated += _limits.increment[_initialBoard.getActivePlayer()];

    // Depth is infinity in a timed search (ends when time runs out)
    _searchDepth = MAX_SEARCH_DEPTH;
    _ourTimeLeft = ourTime - _timeAllocated;
  } else { // No limits specified, use default depth
    _searchDepth = DEFAULT_SEARCH_DEPTH;
    _timeAllocated = INF;
  }


  // Debug_evaluation_paste_below:
  //  std::cout << "Castle_test_ " + std::to_string(k);
  //  std::cout << "HASH_size " + std::to_string(myHASH.HASH_Size());
  //  std::cout << std::endl;
}

void Search::iterDeep() {
  _start = std::chrono::steady_clock::now();
  _nodes = 0;
  selDepth = 0;
  _lastPlyTime = 0;
  for (int currDepth = 1; currDepth <= _searchDepth; currDepth++) {
    _curMaxDepth = currDepth;
    _rootMax(_initialBoard, currDepth, 0);

    int elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
    _lastPlyTime =  elapsed - _lastPlyTime;
    // If limits were exceeded in the search, break without logging UCI info (search was incomplete)
    if (_stop) break;

    if (_logUci) {
      _logUciInfo(_getPv(), currDepth, _bestScore, _nodes, elapsed);
    }

    if (_wasThoughtProlonged)  break;
    // If the last search has exceeded or hit 50% of the allocated time, stop searching
    if (elapsed >= (_timeAllocated / 2)) break;
  }

  if (_logUci) std::cout << "bestmove " << getBestMove().getNotation() << std::endl;
}

MoveList Search::_getPv() {
  MoveList pv;
  for (int i = 0; i < _ourPV.length; i++){
    pv.push_back(Move(_ourPV.pVmoves[i]));
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
  std::cout << "nps " + std::to_string((nodes / elapsed)* 1000)  + " ";
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

  // when searching at a time control we will try to use time efficiatnly.
  // If we already started the search, but it took way longer than expected
  // we actually do not want to lose all of our effort
  // So we check if we have enough time to actually finish it
  // If we have much time left, we will allocate some more
  // time to finish search and set a flag that search was prolonged
  // so we didnt prolong it again.

  if (_wasThoughtProlonged && elapsed >= (_timeAllocated)){
    return true;
  } else  if (elapsed >= (_timeAllocated)){

    // if we have so much time left that we supposedly
    // can search last ply ~25 times at least
    // we can prolong thought here.
    if (_ourTimeLeft > _lastPlyTime * 20 + 30 ){
      _timeAllocated += _lastPlyTime * 2;
      _wasThoughtProlonged = true;
      return false;
    }else{
      return true;
    }

  } 

  return false;
}

inline void Search::_updateAlpha(const Move move, Color color, int depth){
  if (!(move.getFlags() & 0x63)){
    _orderingInfo.incrementHistory(color, move.getFrom(), move.getTo(), depth);
  }
}

inline void Search::_updateBeta(const Move move, Color color, int pMove, int ply, int depth){
	if (!(move.getFlags() & 0x63)) {
    _orderingInfo.updateKillers(ply, move);
    _orderingInfo.incrementHistory(color, move.getFrom(), move.getTo(), depth);
    _orderingInfo.updateCounterMove(color, pMove, move.getMoveINT());
  }
}

inline bool Search::_isRepetitionDraw(const U64 currKey, const int clock){
  for (int i = _posHist.head - 2; (i >= 0 && i >= _posHist.head - clock); i-=2){
    if (_posHist.hisKey[i] == currKey){
      return true;
    }
  }
  return false;
}

void Search::_rootMax(const Board &board, int depth, int ply) {
  _nodes++;
  
  MoveGen movegen(board, false);
  MoveList legalMoves = movegen.getMoves();
  pV rootPV = pV();

  _sEvalArray[ply] = board.colorIsInCheck(board.getActivePlayer()) ? NOSCORE : Eval::evaluate(board, board.getActivePlayer());

  // If no legal moves are available, just return, setting bestmove to a null move
  if (legalMoves.empty()) {
    _bestMove = Move();
    _bestScore = LOST_SCORE;
    return;
  }

  MovePicker movePicker
      (&_orderingInfo, &legalMoves, board.getZKey().getValue(), board.getActivePlayer(), 0, 0);

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
        if (fullWindow) {
          currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -beta, -alpha, ply + 1, false, move.getMoveINT());
        } else {
          currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -alpha - 1, -alpha, ply +1, false, move.getMoveINT());
          if (currScore > alpha) currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -beta, -alpha, ply + 1, false, move.getMoveINT());
        }

        if (_stop || _checkLimits()) {
          _stop = true;
          break;
        }

        // If the current score is better than alpha, or this is the first move in the loop
        if (currScore > alpha) {
          fullWindow = false;
          bestMove = move;
          alpha = currScore;
          _ourPV.length = rootPV.length + 1;
          _ourPV.pVmoves[0] = move.getMoveINT();
          // memcpy - (куда, откуда, длина)
          std::memcpy(_ourPV.pVmoves + 1, rootPV.pVmoves, sizeof(int) * rootPV.length);
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
int Search::_negaMax(const Board &board, pV *up_pV, int depth, int alpha, int beta, int ply, bool doNool, int pMove) {
  // Check search limits
  _nodes++;
  bool AreWeInCheck;
  int score;
  bool pvNode = alpha != beta - 1;
  bool TTmove = false;
  pV   thisPV = pV();

  if (_stop || _checkLimits()) {
    up_pV->length = 0;
    _stop = true;
    return 0;
  }

  // Check for threefold repetition draws
  if (_isRepetitionDraw(board.getZKey().getValue(), board.getHalfmoveClock())) {
    // cut pV out if we found rep
    up_pV->length = 0;
    return 0;
  }

  // Check for 50 move rule draws
  if (board.getHalfmoveClock() >= 100) {
    // cut pV out if we found 50-move draw
    up_pV->length = 0;
    return 0;
  }

  int alphaOrig = alpha;
  // Check transposition table cache
  // If TT is causing a cuttoff, we update 
  // move ordering stuff

  const HASH_Entry probedHASHentry = myHASH.HASH_Get(board.getZKey().getValue());

  if (probedHASHentry.Flag != NONE){
    TTmove = true;
    if (probedHASHentry.depth >= depth && !pvNode){
      int hashScore = probedHASHentry.score;

      if (abs(hashScore) > WON_IN_X){
        hashScore = (hashScore > 0) ? (hashScore - ply) :  (hashScore + ply);   
      }
      if (probedHASHentry.Flag == EXACT){
        _updateAlpha(Move(probedHASHentry.move), board.getActivePlayer(), depth);
        return hashScore;
      }
      if (probedHASHentry.Flag == ALPHA && hashScore <= alpha){ 
        _updateAlpha(Move(probedHASHentry.move), board.getActivePlayer(), depth);
        return alpha;
      }
      if (probedHASHentry.Flag == BETA && hashScore >= beta){
        _updateBeta(Move(probedHASHentry.move), board.getActivePlayer(), pMove, ply, depth);
        return beta;
      }
    }
  }
  
  // Extentions are summed up here
  // InCheck extentions - we extend when the sideToMove is inCheck
  // If we are not in fact in check, evaluate a board for pruning later
  // We dont evaluate when in check because we dont prune in check
  int statEVAL = 0;

  AreWeInCheck = board.colorIsInCheck(board.getActivePlayer());

  // Go into the QSearch if depth is 0
  if (depth <= 0 && !AreWeInCheck) {
    selDepth = std::max(ply, selDepth);
    // cut our pv if we are dropping in the qSearch
    up_pV->length = 0;
    return _qSearch(board, alpha, beta, ply + 1 );
  }

  if (AreWeInCheck) {
    _sEvalArray[ply] = NOSCORE;
  }else if (pMove == 0){
    // last Move was Null, so we can omit stat eval here
    // just negate prev eval and add 2x tempo bonus (10)
    statEVAL = -_sEvalArray[ply - 1] + 10;
    _sEvalArray[ply] = statEVAL;
  }else{
    statEVAL = Eval::evaluate(board, board.getActivePlayer());
    _sEvalArray[ply] = statEVAL;
  }

  // Check if we are improving
  // The idea is if we are not improving in this line
  // We probably can prune a bit more
  bool improving = false;
  if (ply > 2)
    improving = !AreWeInCheck && statEVAL > _sEvalArray[ply - 2];  

  // 1. RAZORING
  // In the very leaf nodes (d == 1)
  // with stat eval << beta we can assume that no 
  // Quiet move can beat it and drop to the QSearch 
  // immidiately

  if (!pvNode && !AreWeInCheck && depth == 1 &&
      statEVAL + RAZORING_MARGIN < beta){
        int qVal = _qSearch(board, alpha, beta, ply + 1);
        return std::max (qVal, statEVAL + RAZORING_MARGIN);
      }


  // 2. REVERSE FUTILITY
  // The idea is so if we are very far ahead of beta at low
  // depth, we can just return estimated eval (eval - margin),
  // because beta probably will be beaten
  // 
  // For now dont Prune in PV, in check, and at high depth
  // btw d < 5 is totally arbitrary, tune it later maybe

  if (!pvNode && !AreWeInCheck && depth < 5){
      if ((statEVAL - REVF_MOVE_CONST * depth + 100 * improving) >= beta)
      return statEVAL - REVF_MOVE_CONST * depth + 100 * improving;
  }

  // 3. NULL MOVE
  // If we are doing so well, that giving opponent 2
  // moves wont improve his position
  // we can safely prune this position
  //
  // For obvious reasons its turned off with no major pieces,
  // when we are in check, and at pvNodes
  if (!pvNode && ply > 0 && depth >= 3 &&
      !doNool && !AreWeInCheck && board.isThereMajorPiece() &&
       statEVAL >= beta){
          Board movedBoard = board;
          movedBoard.doNool();
          int fDepth = depth - NULL_MOVE_REDUCTION - depth/4 - std::min((statEVAL - beta)/128, 4); 
          int score = -_negaMax(movedBoard, &thisPV, fDepth , -beta, -beta +1, ply + 1, true, 0);
          if (score >= beta){
            return beta;
          }
  }

  // 4. UN_HASHED REDUCTION
  // We reduce depth by 1 if the position we currently 
  // analysing isnt hashed.
  // Based on talkchess discussion, replaces Internal
  // iterative deepening.
  // 
  // The justification is if our hashing is decent, if the 
  // position at high depth isnt here, its probably position 
  // not worth searching
  if (depth >= 5 && !TTmove)
    depth--;

  // No pruning occured, generate moves and recurse
  MoveGen movegen(board, false);
  MoveList legalMoves = movegen.getMoves();
  MovePicker movePicker
      (&_orderingInfo, &legalMoves, board.getZKey().getValue(), board.getActivePlayer(), ply, pMove);

  Move bestMove;
  int  LegalMoveCount = 0;
  int  qCount = 0;
  // вероятно не самая эффективная конструкция, но оптимизация потом
  while (movePicker.hasNext()) {

    Move move = movePicker.getNext();

    // 5. LATE MOVE PRUNING
    // If we made many quiet moves in the position already
    // we suppose other moves wont improve our situation
    //
    // Weirdly working, searchdepth is way up, elo gain is not so great

    if (!pvNode && !AreWeInCheck 
      && qCount > _lmp_Array[depth]/(2-improving) && alpha < WON_IN_X ){
      break;
    }

    Board movedBoard = board;
    movedBoard.doMove(move);
    bool doLMR = false;

      if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        LegalMoveCount++;
        int score;

        bool giveCheck = movedBoard.colorIsInCheck(movedBoard.getActivePlayer());
        bool isQuiet = !(move.getFlags() & 0x63);
        bool badHistory = (isQuiet 
                            && _orderingInfo.getHistory(board.getActivePlayer(), move.getFrom(), move.getTo()) < -3*depth*depth);                
        if (isQuiet)
          qCount++;
        int tDepth = depth;
        // 6. EXTENTIONS
        // 
        // 6.1. Passed pawn push extention
        // In the late game  we fear that we may miss
        // some pawn promotions near the leafs of the search tree
        // Thus we extend in the endgame pushes of the non-blocked 
        // passers that are near the middle of the board
        if (depth < 5 && board.isEndGamePosition() && move.isItPasserPush(board)){
              tDepth++;
            }
        

        // 6. EXTENDED FUTILITY PRUNING
        // We try to pune a move, if depth is low (1 or 2)
        // Move should not give check, shoudnt be a promotion and should not be the first move
        // we also should not be in check and close to the MATE score
        // We do not prune in the PV nodes.

        if (!pvNode && !AreWeInCheck && LegalMoveCount > 1 && tDepth < 3 
        && (!giveCheck || badHistory) && alpha < WON_IN_X && !(move.getFlags() & Move::PROMOTION)){
          int moveGain = isQuiet ? 0 : Eval::MATERIAL_VALUES[0][move.getCapturedPieceType()];
          if (statEVAL + FUTIL_MOVE_CONST * tDepth + moveGain - 100 * improving <= alpha){
              continue;
          }
        }
        _posHist.Add(board.getZKey().getValue());

        //7. LATE MOVE REDUCTIONS
        //mix of ideas from Weiss code and what is written in the chessprogramming wiki
        //
        //For now we dont reduce if depth too low, when we are in check
        //and when move give check (with good history).
        //Currently we try to reduce 3rd move and beyond and 4th and beyond in the pvNode.
        //Considering tactical blunders are often in Drofa, this should be subject of
        //modification/tuning


        doLMR = tDepth > 2 && LegalMoveCount > 2 + pvNode && !AreWeInCheck && (!giveCheck || badHistory);
        if (doLMR){

          //Basic reduction is done according to the array
          //Initiated at the ini() of the Search Class
          //Now mostly 0 -> 1
          int reduction = _lmr_R_array[std::min(33, tDepth)][std::min(33, LegalMoveCount)];

          // Reduction tweaks
          // We generally want to guess if the move will not improve alpha
          // and guess right to do no re-searches

          // if move is quiet, reduce a bit more (from Weiss)
          if (isQuiet){
            reduction++;
          }
          //if we are improving, reduce a bit less (from Weiss)
          if (improving){
            reduction--;
          }
          
          // reduce less when move is a Queen promotion
          if ((move.getFlags() & Move::PROMOTION) && (move.getPromotionPieceType() == QUEEN)){
            reduction--;
          }

          // Reduce less for CounterMove and both Killers
          if (move.getMoveINT() == _orderingInfo.getCounterMoveINT(board.getActivePlayer(), pMove) ||
          move == _orderingInfo.getKiller1(ply) ||  move == _orderingInfo.getKiller2(ply)){
            reduction--;
          }

          // reduce more if move has a bad history
          if (isQuiet && 
              _orderingInfo.getHistory(board.getActivePlayer(), move.getFrom(), move.getTo()) < -3*_curMaxDepth*_curMaxDepth){
                reduction++;
          }  

          // We finished reduction tweaking, calculate final depth and search
          // Avoid reduction being less than 0
          reduction = std::max(0, reduction);
          //Avoid to reduce so much that we go to QSearch right away
          int fDepth = std::max(1, tDepth - 1 - reduction);
          
          //Search with reduced depth around alpha in assumtion
          // that alpha would not be beaten here
          score = -_negaMax(movedBoard, &thisPV, fDepth, -alpha - 1 , -alpha, ply + 1, false, move.getMoveINT());
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
            score = -_negaMax(movedBoard, &thisPV, tDepth - 1 + AreWeInCheck, -alpha - 1, -alpha, ply + 1, false, move.getMoveINT());
          }
        } else if (!pvNode || LegalMoveCount > 1){
          score = -_negaMax(movedBoard, &thisPV, tDepth - 1 + AreWeInCheck, -alpha - 1, -alpha, ply + 1, false, move.getMoveINT());
        }

        // If we are in the PV 
        // Search with a full window the first move to calculate bounds
        // or if score improved alpha during the current round of search.
        if  (pvNode) {
          if ((LegalMoveCount == 1) || (score > alpha && score < beta)){
            score = -_negaMax(movedBoard, &thisPV, tDepth - 1 + AreWeInCheck, -beta, -alpha, ply + 1, false, move.getMoveINT());  
          }
        }
        
        _posHist.Remove();
        // Beta cutoff
        if (score >= beta) {
          // Add this move as a new killer move and update history if move is quiet
          _updateBeta(move, board.getActivePlayer(), pMove, ply, depth);
          // Add a new tt entry for this node
          if (!_stop){
            myHASH.HASH_Store(board.getZKey().getValue(), move.getMoveINT(), BETA, score, depth, ply);
          }
          // we updated alpha and in the pVNode
          // so we should update our pV
          if (pvNode && !_stop){
            up_pV->length = thisPV.length + 1;
            up_pV->pVmoves[0] = move.getMoveINT();
            // memcpy - (куда, откуда, длина)
            std::memcpy(up_pV->pVmoves + 1, thisPV.pVmoves, sizeof(int) * thisPV.length);            
          }

          return beta;
        }

        // Check if alpha raised (new best move)
        if (score > alpha) {
          _updateAlpha(move, board.getActivePlayer(), depth);
          alpha = score;
          bestMove = move;
          // we updated alpha and in the pVNode
          // so we should update our pV
          if (pvNode && !_stop){
            up_pV->length = thisPV.length + 1;
            up_pV->pVmoves[0] = move.getMoveINT();
            // memcpy - (куда, откуда, длина)
            std::memcpy(up_pV->pVmoves + 1, thisPV.pVmoves, sizeof(int) * thisPV.length);            
          }

        }else{
          // Beta was not beaten and we dont improve alpha
          // In this case we lower our search history values
          // In order to improve ordering if some move was beaten at very high depth
          _orderingInfo.decrementHistory(board.getActivePlayer(), move.getFrom(), move.getTo(), depth);
        }
      } 

  }

  // Check for checkmate and stalemate
  if (LegalMoveCount == 0) {
    score = AreWeInCheck ? LOST_SCORE + ply : 0; // LOST_SCORE = checkmate, 0 = stalemate (draw)
    up_pV->length = 0;
    return score;
  }

  // If the best move was not set in the main search loop
  // alpha was not raised at any point, just return alpha 
  // (ie do not write in the TT)
  if (bestMove.getFlags() & Move::NULL_MOVE) {
    return alpha;
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
  MovePicker movePicker
      (&_orderingInfo, &legalMoves, board.getZKey().getValue(), board.getActivePlayer(), 99, 0);

  // If node is quiet, just return eval
  if (!movePicker.hasNext()) {
    return standPat;
  }


  while (movePicker.hasNext()) {
    Move move = movePicker.getNext();

    // DELTA MOVE PRUNING. Prune here if were are very far ahead.
    
    int moveGain = Eval::MATERIAL_VALUES[0][move.getCapturedPieceType()];
    if (!(move.getFlags() & Move::PROMOTION) && standPat + moveGain + DELTA_MOVE_CONST < alpha)
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