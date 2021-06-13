#include "defs.h"
#include "search.h"
#include "eval.h"
#include "movepicker.h"
#include <cstring>
#include <thread>
#include <algorithm>
#include <iostream>
#include <math.h>



extern int g_TT_MO_hit;
extern int myTHREADSCOUNT;
extern Search * cSearch[MAX_THREADS];
extern std::thread cThread[MAX_THREADS];
extern HASH * myHASH;


void Search::init_LMR_array(){

  // 1. Initialization of the LMR_array.
  // Original formula, came up after plotting 
  // Weiss formula and trying to came up with 
  // something similar, but based on the pow (x,y)
  // function for easier tuning later.
  // i here is DETPTH
  // j here is moveNUM

  for (int i = 0; i < 34; i++){
    for (int j = 0; j < 34; j++){
      _lmr_R_array[i][j] = (int) (0.1 + (pow(i, 0.15) * pow(j, 0.15))/1.75);
    }
  }
  // 2. Initialization of the LMP array.
  // Current formula is completely based on the 
  // Weiss chess engine.
  for (int i = 0; i < 99; i++){
    _lmp_Array[i][0] = (int) ((3 + pow( i, 2) * 2) / 2);
    _lmp_Array[i][1] = (int) (3 + pow( i, 2) * 2);
  }

}

Search::Search(const Board &board, Limits limits, Hist positionHistory, bool logUci) :
    _orderingInfo(OrderingInfo()),
    _limits(limits),
    _initialBoard(board),
    _logUci(logUci),
    _stop(false),
    _limitCheckCount(0),
    _nodes(0),
    _bestScore(0)
     {

  std::memset(_sEvalArray, 0, sizeof(_sEvalArray));
  init_LMR_array();
  _wasThoughtProlonged = false;
  _posHist = positionHistory;
  if (_limits.infinite) { // Infinite search
    _searchDepth = INF;
    _timeAllocated = INF;
  } else if (_limits.depth != 0) { // Depth search
    _searchDepth = _limits.depth;
    _timeAllocated = INF;
  } else if (_limits.moveTime != 0) {
    _searchDepth = MAX_SEARCH_DEPTH;
    _timeAllocated = _limits.moveTime;
  } else if (_limits.time[_initialBoard.getActivePlayer()] != 0) { 
    _setupTimer(board, 0);
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
  _selDepth = 0;
  _lastPlyTime = 0;
  int aspWindow = 25;
  int aspDelta  = 50;

  for (int currDepth = 1; currDepth <= _searchDepth; currDepth++) {
    _badHistMargin = -3 * pow(currDepth, 2);

    int aspAlpha = LOST_SCORE;
    int aspBeta  =-LOST_SCORE;
    if (currDepth > 6){
      aspAlpha = _bestScore - aspWindow;
      aspBeta  = _bestScore + aspWindow;
    }

    while (true){

    int score = _rootMax(_initialBoard, aspAlpha, aspBeta, currDepth, 0);

    if (_stop) break;
    if (score <= aspAlpha){
      aspAlpha = std::max(aspAlpha - aspDelta, LOST_SCORE);
    }else if( score >= aspBeta){
      aspBeta  = std::min(aspBeta + aspDelta, -LOST_SCORE);
    }else{
      break;
    }

    aspDelta += aspDelta * 2 / 3;
    }


    
    if (_stop) break;
    int elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start).count();
    _lastPlyTime =  elapsed - _lastPlyTime;
    // If limits were exceeded in the search, break without logging UCI info (search was incomplete)


    if (_logUci) {
      _logUciInfo(_getPv(), currDepth, _bestScore, _nodes, elapsed);
    }

    if (_wasThoughtProlonged)  break;
    // If the last search has exceeded or hit 50% of the allocated time, stop searching
    if (elapsed >= (_timeAllocated / 2)) break;
  }

  if (_logUci) std::cout << "bestmove " << getBestMove().getNotation() << std::endl;

  if (_logUci){

    //send all other thread stop signal
    for (int i = 1; i < myTHREADSCOUNT; i++){
      if ( cSearch[i] != nullptr){
        cSearch[i]->stop();
      }
    }

    // wait for extra threads
    for (int i = 1; i < myTHREADSCOUNT; i++){
      if (cThread[i].joinable()){
        cThread[i].join();
      }
    }

    // threads finished, delete extensive Searches
    for (int i = 1; i < myTHREADSCOUNT; i++){
      delete cSearch[i];
      cSearch[i] = nullptr;
    }
}
  
}

MoveList Search::_getPv() {
  MoveList pv;
  for (int i = 0; i < _ourPV.length; i++){
    pv.push_back(Move(_ourPV.pVmoves[i]));
  }

  return pv;
}

void Search::_logUciInfo(const MoveList &pv, int depth, int bestScore, U64 nodes, int elapsed) {
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
  // Avoid _selDepth being smaller than depth when entire path to score is in TT
  _selDepth = std::max(depth, _selDepth);

  //collect info about nodes from all Threads
  for (int i = 1; i < myTHREADSCOUNT; i++){
    if (cSearch[i] != nullptr){
      nodes += cSearch[i]->getNodes(); 
    }
  }
  
  std::cout << "info depth " + std::to_string(depth) + " ";
  std::cout << "seldepth " + std::to_string(_selDepth) + " ";
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

int Search::getNodes(){
  return _nodes;
}

int Search::getBestScore(){
  return _bestScore;
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

void Search::_setupTimer(const Board &board, int curPlyNum){

    int ourTime = _limits.time[_initialBoard.getActivePlayer()];
    int opponentTime = _limits.time[_initialBoard.getInactivePlayer()];
    int moveNum = board._getGameClock() / 2;
    int ourIncrement = _limits.increment[_initialBoard.getActivePlayer()];

    int tWidth_a = 30;
    int tWidth = 175;
    int tMove = 20;
    int criticalMove = 28;

    double tCoefficient = 0;

    // Divide up the remaining time (If movestogo not specified we are in 
    // sudden death)
    if (_limits.movesToGo == 0) { 
      tCoefficient = 10 * (tWidth_a / pow((tWidth + pow((moveNum - tMove), 2)), 1.5));
      _timeAllocated = ourTime * tCoefficient;
      if (moveNum > criticalMove) _timeAllocated = ourTime / 10 + ourIncrement;
      _timeAllocated = std::min(_timeAllocated, ourTime + ourIncrement - 10);
    } else {
      // when movetogo is specified, use different coefficients
      tWidth_a = 75;
      tWidth = 200;
      tMove = 35;
      criticalMove = 20;
      
      tCoefficient = 10 * (tWidth_a / pow((tWidth + pow((moveNum - tMove), 2)), 1.5));
      _timeAllocated = ourTime * tCoefficient;
      if (moveNum > criticalMove) _timeAllocated = ourTime / 10 + ourIncrement;
      _timeAllocated = std::min(_timeAllocated, ourTime + ourIncrement - 10);
    }

    // Depth is infinity in a timed search (ends when time runs out)
    _searchDepth = MAX_SEARCH_DEPTH;
    _ourTimeLeft = ourTime - _timeAllocated;
}

inline void Search::_updateAlpha(bool isQuiet, const Move move, Color color, int depth){
  if (isQuiet){
    _orderingInfo.incrementHistory(color, move.getFrom(), move.getTo(), depth);
  }
}

inline void Search::_updateBeta(bool isQuiet, const Move move, Color color, int pMove, int ply, int depth){
	if (isQuiet) {
    _orderingInfo.updateKillers(ply, move);
    _orderingInfo.incrementHistory(color, move.getFrom(), move.getTo(), depth);
    _orderingInfo.updateCounterMove(color, pMove, move.getMoveINT());
  }
}

inline bool Search::_isRepetitionDraw(U64 currKey){
  for (int i = _posHist.head - 1; i >= 0; i--){
    if (_posHist.hisKey[i] == currKey){
      return true;
    }
  }
  return false;
}

int Search::_rootMax(const Board &board, int alpha, int beta, int depth, int ply) {
  _nodes++;
  
  MoveGen movegen(board, false);
  MoveList legalMoves = movegen.getMoves();
  pV rootPV = pV();

  _sEvalArray[ply] = board.colorIsInCheck(board.getActivePlayer()) ? NOSCORE : Eval::evaluate(board, board.getActivePlayer());

  // If no legal moves are available, just return, setting bestmove to a null move
  if (legalMoves.empty()) {
    _bestMove = Move();
    _bestScore = LOST_SCORE;
    return 0;
  }

const HASH_Entry probedHASHentry = myHASH->HASH_Get(board.getZKey().getValue());
int hashMove = probedHASHentry.Flag != NONE ? probedHASHentry.move : 0;
  MovePicker movePicker
      (&_orderingInfo, &board, &legalMoves, hashMove, board.getActivePlayer(), 0, 0);

  int currScore;

  Move bestMove;
  bool fullWindow = true;
  while (movePicker.hasNext()) {
    Move move = movePicker.getNext();

    Board movedBoard = board;
    movedBoard.doMove(move);
    if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        if (fullWindow) {
          currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -beta, -alpha, ply + 1, false, move.getMoveINT(), false);
        } else {
          currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -alpha - 1, -alpha, ply +1, false, move.getMoveINT(), false);
          if (currScore > alpha) currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -beta, -alpha, ply + 1, false, move.getMoveINT(), false);
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

  if (!_stop && !(bestMove.getFlags() & Move::NULL_MOVE)) {
    myHASH->HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), EXACT, alpha, depth, ply);
    _bestMove = bestMove;
    _bestScore = alpha;
  }

  return alpha;
}

// this is basically my main search
// 
int Search::_negaMax(const Board &board, pV *up_pV, int depth, int alpha, int beta, int ply, bool doNool, int pMove, bool sing) {
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
  // and 50 - move rule draw
  if (board.getHalfmoveClock() >= 100 || _isRepetitionDraw(board.getZKey().getValue())) {
    // cut pV out if we found draw
    up_pV->length = 0;
    return 0;
  }

  int alphaOrig = alpha;
  Move hashedMove = Move(0);
  // Check transposition table cache
  // If TT is causing a cuttoff, we update 
  // move ordering stuff

  const HASH_Entry probedHASHentry = myHASH->HASH_Get(board.getZKey().getValue());

  if (probedHASHentry.Flag != NONE){
    TTmove = true;
    hashedMove = Move(probedHASHentry.move);
    if (probedHASHentry.depth >= depth && !pvNode && !sing){
      int hashScore = probedHASHentry.score;

      if (abs(hashScore) > WON_IN_X){
        hashScore = (hashScore > 0) ? (hashScore - ply) :  (hashScore + ply);   
      }
      if (probedHASHentry.Flag == EXACT){
        _updateAlpha(hashedMove.isQuiet(), hashedMove, board.getActivePlayer(), depth);
        return hashScore;
      }
      if (probedHASHentry.Flag == ALPHA && hashScore <= alpha){ 
        _updateAlpha(hashedMove.isQuiet(), hashedMove, board.getActivePlayer(), depth);
        return alpha;
      }
      if (probedHASHentry.Flag == BETA && hashScore >= beta){
        _updateBeta(hashedMove.isQuiet(), hashedMove, board.getActivePlayer(), pMove, ply, depth);
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
    _selDepth = std::max(ply, _selDepth);
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
      statEVAL + RAZORING_MARGIN < beta && !sing){
        return _qSearch(board, alpha, beta, ply + 1);
      }


  // 2. REVERSE FUTILITY
  // The idea is so if we are very far ahead of beta at low
  // depth, we can just return estimated eval (eval - margin),
  // because beta probably will be beaten
  // 
  // For now dont Prune in PV, in check, and at high depth
  // btw d < 5 is totally arbitrary, tune it later maybe

  if (!pvNode && !AreWeInCheck && depth < 6 && !sing){
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
  bool failedNull = false;
  if (!pvNode && ply > 0 && depth >= 3 &&
      !doNool && !AreWeInCheck && board.isThereMajorPiece() &&
       statEVAL >= beta && !sing){
          Board movedBoard = board;
          movedBoard.doNool();
          int fDepth = depth - NULL_MOVE_REDUCTION - depth/4 - std::min((statEVAL - beta)/128, 4); 
          int score = -_negaMax(movedBoard, &thisPV, fDepth , -beta, -beta +1, ply + 1, true, 0, false);
          if (score >= beta){
            return beta;
          }
          // if we arrived here NULL was tried and failed
          failedNull = true;
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
  //
  // Drofa dont do this reduction after NullMove, because 
  // we already reduced a lot, and reducing further may reduce quality of
  // the NM_Search
  if (depth >= 5 && !TTmove && !doNool && !sing)
    depth--;

  // No pruning occured, generate moves and recurse
  MoveGen movegen(board, false);
  MoveList legalMoves = movegen.getMoves();
  MovePicker movePicker
      (&_orderingInfo, &board, &legalMoves, hashedMove.getMoveINT(), board.getActivePlayer(), ply, pMove);

  Move bestMove;
  int  LegalMoveCount = 0;
  int  qCount = 0;
  // вероятно не самая эффективная конструкция, но оптимизация потом
  while (movePicker.hasNext()) {

    Move move = movePicker.getNext();
    if (move == probedHASHentry.move && sing){
      continue;
    }
    bool isQuiet = move.isQuiet();

    // 5. LATE MOVE PRUNING
    // If we made many quiet moves in the position already
    // we suppose other moves wont improve our situation
    //
    // Weirdly working, searchdepth is way up, elo gain is not so great

    if (!pvNode 
        && !AreWeInCheck 
        && alpha < WON_IN_X){

      if (qCount > _lmp_Array[depth][improving]) break;

      if (depth <= 6 
          && LegalMoveCount > 1
          && isQuiet 
          && board.Calculate_SEE(move) < -51 * depth) continue;
    }

    Board movedBoard = board;
    movedBoard.doMove(move);
    bool doLMR = false;

      if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        LegalMoveCount++;
        int score;

        bool giveCheck = movedBoard.colorIsInCheck(movedBoard.getActivePlayer());
        int  moveHistory  = isQuiet ? _orderingInfo.getHistory(board.getActivePlayer(), move.getFrom(), move.getTo()) : 0;
        bool badHistory = (isQuiet && moveHistory < _badHistMargin);                
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

        if (depth > 8 &&
            !AreWeInCheck &&
            probedHASHentry.Flag != ALPHA &&
            probedHASHentry.depth >= depth - 2 &&
            probedHASHentry.move == move.getMoveINT() &&
            abs(probedHASHentry.score) < WON_IN_X / 4){
              int sDepth = depth / 2;
              int sBeta = probedHASHentry.score - depth * 2;
              Board sBoard = board;
              int score = _negaMax(sBoard, &thisPV, sDepth, sBeta - 1, sBeta, ply, false, pMove, true);
              if (sBeta > score){
                tDepth++;
              }
            }

        // 6. EXTENDED FUTILITY PRUNING
        // We try to pune a move, if depth is low (1 or 2)
        // Move should not give check, shoudnt be a promotion and should not be the first move
        // we also should not be in check and close to the MATE score
        // We do not prune in the PV nodes.

        if (!pvNode && !AreWeInCheck && LegalMoveCount > 1 && tDepth < 3 
        && (!giveCheck || badHistory) && alpha < WON_IN_X && !(move.getFlags() & Move::PROMOTION)){
          int moveGain = isQuiet ? 0 : opS(Eval::MATERIAL_VALUES[move.getCapturedPieceType()]);
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
          reduction += isQuiet;

          // if we failed NULL, likely most of our Quiet moves are crap, so reduce them even more
          // qCount > 3 is actually seems to be optimal
          reduction += isQuiet && qCount > 3 && failedNull;

          // reduce more if move has a bad history
          reduction += isQuiet && moveHistory < _badHistMargin;

          //if we are improving, reduce a bit less (from Weiss)
          reduction -= improving;
          
          // reduce less when move is a Queen promotion
          reduction -= (move.getFlags() & Move::PROMOTION) && (move.getPromotionPieceType() == QUEEN);

          // Reduce less for CounterMove and both Killers
          reduction -= (move.getMoveINT() == _orderingInfo.getCounterMoveINT(board.getActivePlayer(), pMove) ||
                        move == _orderingInfo.getKiller1(ply) ||  move == _orderingInfo.getKiller2(ply));  

          // We finished reduction tweaking, calculate final depth and search
          // Avoid reduction being less than 0
          reduction = std::max(0, reduction);
          //Avoid to reduce so much that we go to QSearch right away
          int fDepth = std::max(1, tDepth - 1 - reduction);
          
          //Search with reduced depth around alpha in assumtion
          // that alpha would not be beaten here
          score = -_negaMax(movedBoard, &thisPV, fDepth, -alpha - 1 , -alpha, ply + 1, false, move.getMoveINT(), false);
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
            score = -_negaMax(movedBoard, &thisPV, tDepth - 1 + AreWeInCheck, -alpha - 1, -alpha, ply + 1, false, move.getMoveINT(), false);
          }
        } else if (!pvNode || LegalMoveCount > 1){
          score = -_negaMax(movedBoard, &thisPV, tDepth - 1 + AreWeInCheck, -alpha - 1, -alpha, ply + 1, false, move.getMoveINT(), false);
        }

        // If we are in the PV 
        // Search with a full window the first move to calculate bounds
        // or if score improved alpha during the current round of search.
        if  (pvNode) {
          if ((LegalMoveCount == 1) || (score > alpha && score < beta)){
            score = -_negaMax(movedBoard, &thisPV, tDepth - 1 + AreWeInCheck, -beta, -alpha, ply + 1, false, move.getMoveINT(), false);  
          }
        }
        
        _posHist.Remove();
        // Beta cutoff
        if (score >= beta) {
          // Add this move as a new killer move and update history if move is quiet
          _updateBeta(isQuiet, move, board.getActivePlayer(), pMove, ply, depth);
          // Add a new tt entry for this node
          if (!_stop){
            myHASH->HASH_Store(board.getZKey().getValue(), move.getMoveINT(), BETA, score, depth, ply);
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
          _updateAlpha(isQuiet, move, board.getActivePlayer(), depth);
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
        myHASH->HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), ALPHA, alpha, depth, ply);
      } else {
        myHASH->HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), EXACT, alpha, depth, ply);
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
  MovePicker movePicker
      (&_orderingInfo, &board, &legalMoves, 0, board.getActivePlayer(), 99, 0);

  // If node is quiet, just return eval
  if (!movePicker.hasNext()) {
    return standPat;
  }


  while (movePicker.hasNext()) {
    Move move = movePicker.getNext();
    
    // in qSearch if Value < 0 it means it is a bad capture
    // and we should prune it
    if (move.getValue() < 0){
      break;
    }

    int moveGain = opS(Eval::MATERIAL_VALUES[move.getCapturedPieceType()]);
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
