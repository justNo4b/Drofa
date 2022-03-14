#include "defs.h"
#include "search.h"
#include "eval.h"
#include "movepicker.h"
#include "searchdata.h"
#include <cstring>
#include <thread>
#include <algorithm>
#include <iostream>
#include <math.h>


extern int myTHREADSCOUNT;
extern OrderingInfo   * cOrdering[MAX_THREADS];
extern Search         * cSearch[MAX_THREADS];
extern std::thread      cThread[MAX_THREADS];
extern HASH           * myHASH;


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
  for (int i = 0; i < MAX_PLY; i++){
    _lmp_Array[i][0] = (int) ((3 + pow( i, 2) * 2) / 2);
    _lmp_Array[i][1] = (int) (3 + pow( i, 2) * 2);
  }

}

Search::Search(const Board &board, Limits limits, Hist positionHistory, OrderingInfo *info, bool logUci) :
    _orderingInfo(*info),
    _timer(limits, board.getActivePlayer(), board._getGameClock() / 2),
    _initialBoard(board),
    _logUci(logUci),
    _stop(false),
    _limitCheckCount(0),
    _nodes(0),
    _bestScore(0)
     {

  _sStack = SEARCH_Data();
  _posHist = positionHistory;
  init_LMR_array();
}

void Search::iterDeep() {

  _nodes = 0;
  _selDepth = 0;
  std::memset(_rootNodesSpent, 0, sizeof(_rootNodesSpent));
  _timer.startIteration();
  int targetDepth = _timer.getSearchDepth();
  int aspWindow = 25;
  int aspDelta  = 50;

    for (int currDepth = 1; currDepth <= targetDepth; currDepth++) {

        int aspAlpha = LOST_SCORE;
        int aspBeta  =-LOST_SCORE;
        if (currDepth > 6){
            aspAlpha = _bestScore - aspWindow;
            aspBeta  = _bestScore + aspWindow;
        }

        while (true){

            int score = _rootMax(_initialBoard, aspAlpha, aspBeta, currDepth);

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

        // Iteration finished normally
        // Check and adjust time we should spend, and print UCI info

        if (_stop) break;

        int elapsed = 0;
        bool shouldStop = _timer.finishOnThisDepth(&elapsed, _nodes, _rootNodesSpent[_bestMove.getPieceType()][_bestMove.getTo()]);
        if (_logUci) {
            _logUciInfo(_getPv(), currDepth, _bestScore, _nodes, elapsed);
        }

        if (shouldStop) break;

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

    //all done, delete excessive OrderingInfos
    for (int i = 1; i < myTHREADSCOUNT; i++){
      delete cOrdering[i];
      cOrdering[i] = nullptr;
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

bool Search::_checkLimits() {

  if (--_limitCheckCount > 0) {
    return false;
  }

  _limitCheckCount = 2048;
  return _timer.checkLimits(_nodes);
}

inline void Search::_updateBeta(bool isQuiet, const Move move, Color color, int pMove, int ply, int depth){
	if (isQuiet) {
    _orderingInfo.updateKillers(ply, move);
    _orderingInfo.incrementHistory(color, move.getFrom(), move.getTo(), depth);
    _orderingInfo.updateCounterMove(color, pMove, move.getMoveINT());
    _orderingInfo.incrementCounterHistory(color, pMove, move.getPieceType(), move.getTo(), depth);
  }else{
    _orderingInfo.incrementCapHistory(move.getPieceType(), move.getCapturedPieceType(), move.getTo(), depth);
  }
}

inline bool Search::_isRepetitionDraw(U64 currKey, int untillFifty){
  for (int i = _posHist.head - 2; (i >= 0 || i > _posHist.head - 2 - untillFifty); i-=2){
    if (_posHist.hisKey[i] == currKey){
      return true;
    }
  }
  return false;
}

int Search::_rootMax(const Board &board, int alpha, int beta, int depth) {
  _nodes++;

  MoveGen movegen(board, false);
  MoveList * legalMoves = movegen.getMoves();
  pV rootPV = pV();

  _sStack.AddEval(board.colorIsInCheck(board.getActivePlayer()) ? NOSCORE : Eval::evaluate(board, board.getActivePlayer()));

  // If no legal moves are available, just return, setting bestmove to a null move
  if (legalMoves->empty()) {
    _bestMove = Move();
    _bestScore = LOST_SCORE;
    return 0;
  }

  const HASH_Entry probedHASHentry = myHASH->HASH_Get(board.getZKey().getValue());
  int hashMove = probedHASHentry.Flag != NONE ? probedHASHentry.move : 0;
  MovePicker movePicker(&_orderingInfo, &board, legalMoves, hashMove, board.getActivePlayer(), 0, 0);

  int currScore;

  Move bestMove;
  bool fullWindow = true;
  while (movePicker.hasNext()) {
    Move move = movePicker.getNext();

    Board movedBoard = board;
    movedBoard.doMove(move);
    _sStack.AddMove(move.getMoveINT());

    if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        U64 nodesStart = _nodes;

        if (fullWindow) {
          currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -beta, -alpha, false, false);
        } else {
          currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -alpha - 1, -alpha,  false, true);
          if (currScore > alpha) currScore = -_negaMax(movedBoard, &rootPV, depth - 1, -beta, -alpha, false, false);
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
        _rootNodesSpent[move.getPieceType()][move.getTo()] += _nodes - nodesStart;

    }
    _sStack.Remove();
  }

  if (!_stop && !(bestMove.getFlags() & Move::NULL_MOVE)) {
    myHASH->HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), EXACT, alpha, depth, 0);
    _bestMove = bestMove;
    _bestScore = alpha;
  }

  return alpha;
}

int Search::_negaMax(const Board &board, pV *up_pV, int depth, int alpha, int beta, bool sing, bool cutNode) {

  _nodes++;
  bool AreWeInCheck;
  bool pvNode = alpha != beta - 1;
  bool TTmove = false;
  bool quietTT = false;
  bool nmpTree = _sStack.nmpTree;
  int score;
  int ply = _sStack.ply;
  int pMove = _sStack.moves[ply - 1];
  int alphaOrig = alpha;
  int statEVAL = 0;
  Move hashedMove = Move(0);
  pV   thisPV = pV();
  Color behindColor = _sStack.sideBehind;

  // Check if we are out of time
  if (_stop || _checkLimits()) {
    up_pV->length = 0;
    _stop = true;
    return 0;
  }

  // Check for threefold repetition draws and 50 - move rule draw
  // cut pV out if we found draw
  if (board.getHalfmoveClock() >= 100 || _isRepetitionDraw(board.getZKey().getValue(), board.getHalfmoveClock())) {
    up_pV->length = 0;
    return 0;
  }

  // Check transposition table cache
  // If TT is causing a cuttoff, we update move ordering stuff
  const HASH_Entry probedHASHentry = myHASH->HASH_Get(board.getZKey().getValue());
  if (probedHASHentry.Flag != NONE){
    TTmove = true;
    hashedMove = Move(probedHASHentry.move);
    quietTT = hashedMove.isQuiet();
    if (probedHASHentry.depth >= depth && !pvNode && !sing){
      int hashScore = probedHASHentry.score;

      if (abs(hashScore) > WON_IN_X){
        hashScore = (hashScore > 0) ? (hashScore - ply) :  (hashScore + ply);
      }
      if (probedHASHentry.Flag == EXACT){
        return hashScore;
      }
      if (probedHASHentry.Flag == ALPHA && hashScore <= alpha){
        return alpha;
      }
      if (probedHASHentry.Flag == BETA && hashScore >= beta){
        _updateBeta(quietTT, hashedMove, board.getActivePlayer(), pMove, ply, depth);
        return beta;
      }
    }
  }

  // Check our InCheck status
  AreWeInCheck = board.colorIsInCheck(board.getActivePlayer());

  // Go into the QSearch if depth is 0 and we are not in check
  // Cut out pV and update our seldepth before dropping into qSearch
  if ((depth <= 0 && !AreWeInCheck) || ply >= MAX_PLY) {
    _selDepth = std::max(ply, _selDepth);
    up_pV->length = 0;
    return _qSearch(board, alpha, beta);
  }

  // Statically evaluate our position
  // Do the Evaluation, unless we are in check or prev move was NULL
  // If last Move was Null, just negate prev eval and add 2x tempo bonus (10)
  if (AreWeInCheck) {
    _sStack.AddEval(NOSCORE);
  }else {
    statEVAL = Eval::evaluate(board, board.getActivePlayer());
    _sStack.AddEval(statEVAL);
  }

  // Check if we are improving
  // The idea is if we are not improving in this line we probably can prune a bit more
  bool improving = false;
  if (ply > 2) improving = !AreWeInCheck && statEVAL > _sStack.statEval[ply - 2];

  // Clear Killers for the children node
  _orderingInfo.clearChildrenKillers(ply);

  // Check if we are doing pre-move pruning techniques
  // We do not do them InCheck, in pvNodes and when proving singularity
  bool isPrune = !pvNode && !AreWeInCheck && !sing;

  // 1. RAZORING
  // In the very leaf nodes (d == 1) with stat eval << beta we can assume that no
  // Quiet move can beat it and drop to the QSearch immidiately
  if (isPrune && depth == 1 && (statEVAL + RAZORING_MARGIN < beta)){
        return _qSearch(board, alpha, beta);
      }

  // 2. REVERSE FUTILITY
  // The idea is so if we are very far ahead of beta at low
  // depth, we can just return estimated eval (eval - margin),
  // because beta probably will be beaten
  if (isPrune && depth < 6 && ((statEVAL - REVF_MOVE_CONST * depth + 100 * improving) >= beta)){
      return statEVAL - REVF_MOVE_CONST * depth + 100 * improving;
  }

  // 3. NULL MOVE
  // If we are doing so well, that giving opponent 2 moves wont improve his position
  // we can safely prune this position.
  // Apart from usual  stuff we do not use NMP when there is only Kings and Pawns
  // and when last move was also null
  // Drofa also track status of the Null move failure
  bool failedNull = false;
  if (isPrune && depth >= 3 && pMove != 0 && statEVAL >= beta &&
      board.isThereMajorPiece()){
          Board movedBoard = board;
          _posHist.Add(board.getZKey().getValue());
          _sStack.AddNullMove(getOppositeColor(board.getActivePlayer()));
          movedBoard.doNool();
          int fDepth = depth - NULL_MOVE_REDUCTION - depth / 4 - std::min((statEVAL - beta) / 128, 4);
          int score = -_negaMax(movedBoard, &thisPV, fDepth , -beta, -beta +1, false, false);
          _posHist.Remove();
          _sStack.RemoveNull(behindColor, nmpTree);
          if (score >= beta){
            return beta;
          }
          failedNull = true;
  }

  // 4. UN_HASHED REDUCTION
  // We reduce depth by 1 if the position we currently analysing isnt hashed.
  // Based on talkchess discussion, replaces Internal iterative deepening.
  // The justification is if our hashing is decent, if the
  // position at high depth isnt here, its probably position not worth searching
  //
  // Drofa dont do this reduction after NullMove, because we already reduced a lot,
  // and reducing further may reduce quality of the NM_Search
  if (depth >= 5 && !TTmove && pMove != 0 && !sing)
    depth--;

  // No pruning occured, generate moves and recurse
  MoveGen movegen(board, false);
  MoveList * legalMoves = movegen.getMoves();
  MovePicker movePicker(&_orderingInfo, &board, legalMoves, hashedMove.getMoveINT(), board.getActivePlayer(), ply, pMove);

  // Probcut
  if (!pvNode &&
       depth >= 5 &&
       !(quietTT && failedNull) &&
       alpha < WON_IN_X){
        int pcBeta = beta + 200;
        while (movePicker.hasNext()){
            Move move = movePicker.getNext();

            // exit when there is no more captures
            if (move.getValue() <= 300000){
                movePicker.refreshPicker();
                break;
            }

            // skip quiet TT moves
            if (move == probedHASHentry.move && move.isQuiet()){
                continue;
            }

            // make a move
            Board movedBoard = board;
            movedBoard.doMove(move);
            if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
                // see if qSearch holds
                int qScore = - _qSearch(movedBoard, -pcBeta, -pcBeta + 1);

                // if it holds, do proper reduced search
                if(qScore >= pcBeta){
                    _posHist.Add(board.getZKey().getValue());
                    _sStack.AddMove(move.getMoveINT());

                    int sScore = -_negaMax(movedBoard, &thisPV, depth - 4, -pcBeta, -pcBeta + 1, false, !cutNode);

                    _posHist.Remove();
                    _sStack.Remove();

                    if (sScore >= pcBeta){
                        return beta;
                    }
                }
            }
        }
    }

  Move bestMove;
  int  LegalMoveCount = 0;
  int  qCount = 0;
  bool singularExists = false;
  int  pMoveIndx = pTypeToPair((pMove & 0x7), ((pMove >> 15) & 0x3f));

  while (movePicker.hasNext()) {
    Move move = movePicker.getNext();
    if (move == probedHASHentry.move && sing){
      continue;
    }
    bool isQuiet = move.isQuiet();
    qCount += isQuiet;

    if (alpha < WON_IN_X
        && LegalMoveCount >= 1){

      // 5. LATE MOVE PRUNING
      // If we made many quiet moves in the position already
      // we suppose other moves wont improve our situation
      if (qCount > _lmp_Array[depth][(improving || pvNode)]) break;

      // 6. EXTENDED FUTILITY PRUNING
      // We try to pune a move, if depth is low (1 or 2)
      // Move should not give check, shoudnt be a promotion and should not be the first move
      // we also should not be in check and close to the MATE score
      // We do not prune in the PV nodes.

      if (!AreWeInCheck &&
          depth < 3 &&
          !(move.getFlags() & Move::PROMOTION) &&
          statEVAL + board.Calculate_MoveGain(move) + FUTIL_MOVE_CONST * depth - 100 * improving <= alpha){
            continue;
      }

      // 7. SEE pruning of quiet moves
      // At shallow depth prune highlyish -negative SEE-moves
      if (depth <= 10
          && isQuiet
          && board.Calculate_SEE(move) < -51 * depth) continue;
    }

    Board movedBoard = board;
    movedBoard.doMove(move);
    myHASH->HASH_Prefetch(movedBoard.getZKey().getValue());
    bool doLMR = false;

      if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){
        LegalMoveCount++;
        int score;

        bool giveCheck = movedBoard.colorIsInCheck(movedBoard.getActivePlayer());
        int  moveHistory  = isQuiet ?
                            _orderingInfo.getHistory(board.getActivePlayer(), move.getFrom(), move.getTo()) :
                            _orderingInfo.getCaptureHistory(move.getPieceType(), move.getCapturedPieceType(), move.getTo());
        int cmHistory     = isQuiet ? _orderingInfo.getCountermoveHistory(board.getActivePlayer(), pMoveIndx, move.getPieceType(), move.getTo()) : 0;
        int tDepth = depth;
        // 6. EXTENTIONS
        //
        // 6.0 InCheck extention
        // Extend when the side to move is in check
        if (AreWeInCheck){
          tDepth++;
        }

        // 6.1. Passed pawn push extention
        // In the late game  we fear that we may miss
        // some pawn promotions near the leafs of the search tree
        // Thus we extend in the endgame pushes of the non-blocked
        // passers that are near the middle of the board
        // Extend more if null move failed
        if (depth <= 8 && board.isEndGamePosition() && move.isItPasserPush(board)){
              tDepth += 1 + failedNull;
            }

        // 6.2 Singular move extention
        // At high depth if we have the TT move, and we are certain
        // that non other moves are even close to it, extend this move
        if (depth > 8 &&
            !AreWeInCheck &&
            probedHASHentry.Flag != ALPHA &&
            probedHASHentry.depth >= depth - 2 &&
            probedHASHentry.move == move.getMoveINT() &&
            abs(probedHASHentry.score) < WON_IN_X / 4){
              int sDepth = depth / 2;
              int sBeta = probedHASHentry.score - depth * 2;
              Board sBoard = board;
              int score = _negaMax(sBoard, &thisPV, sDepth, sBeta - 1, sBeta, true, cutNode);
              if (sBeta > score){
                tDepth += 1 + failedNull;
                singularExists = true;
              }
            }

        // 6.3 Last capture extention
        // In the endgame positions we extend any non-pawn captures
        // It seems benefitial as we calculate resulting endgame more accurately
        if (!isQuiet && board.isEndGamePosition() &&
            move.getCapturedPieceType() != PAWN){
              tDepth++;
            }

        _posHist.Add(board.getZKey().getValue());
        _sStack.AddMove(move.getMoveINT());

        // 8. LATE MOVE REDUCTIONS
        // mix of ideas from Weiss code, own ones and what is written in the chessprogramming wiki
        doLMR = tDepth > 2 && LegalMoveCount > 2 + pvNode;
        if (doLMR){

          //Basic reduction is done according to the array
          int reduction = _lmr_R_array[std::min(33, tDepth)][std::min(33, LegalMoveCount)];

          // Reduction tweaks
          // We generally want to guess if the move will not improve alpha and guess right to do no re-searches

          // if move is quiet, reduce a bit more (from Weiss)
          reduction += isQuiet;

          //reduce more when side to move is in check
          reduction += AreWeInCheck;

          // if we failed NULL, likely most of our Quiet moves are crap, so reduce them even more
          // qCount > 3 is actually seems to be optimal
          reduction += isQuiet && qCount > 3 && failedNull;

          // Reduce more for late quiets if TTmove exists and it is non-Quiet move
          reduction += isQuiet && !quietTT && TTmove;

          // Reduce more when side-to-move was behind prior to NMP on the previous NMP try
          // Basically copy-pasted Koivisto idea
          reduction += isQuiet && nmpTree && board.getActivePlayer() == behindColor;

          // Reduce more in the cut-nodes - used by SF/Komodo/etc
          reduction += cutNode;

          // if we are improving, reduce a bit less (from Weiss)
          reduction -= improving;

          // reduce less when a move is giving check
          reduction -= giveCheck;

          // reduce less for a position where singular move exists
          reduction -= singularExists;

          // reduce more/less based on the hitory
          reduction -= moveHistory / 8192;
          reduction -= cmHistory  / 12288;

          // reduce less when move is a Queen promotion
          reduction -= (move.getFlags() & Move::PROMOTION) && (move.getPromotionPieceType() == QUEEN);

          // Reduce less for CounterMove and both Killers
          reduction -= 2 * (move.getMoveINT() == _orderingInfo.getCounterMoveINT(board.getActivePlayer(), pMove) ||
                            move == _orderingInfo.getKiller1(ply) ||  move == _orderingInfo.getKiller2(ply));

          // We finished reduction tweaking, calculate final depth and search
          // Idea from SF - > allow extending if our reductions are very negative
          int minReduction = (!isQuiet && LegalMoveCount <= 6) ? -2 :
                             (cutNode || pvNode) ? -1 : 0;

          reduction = std::max(minReduction, reduction);
          //Avoid to reduce so much that we go to QSearch right away
          int fDepth = std::max(1, tDepth - 1 - reduction);

          //Search with reduced depth around alpha in assumtion
          // that alpha would not be beaten here
          score = -_negaMax(movedBoard, &thisPV, fDepth, -alpha - 1 , -alpha, false, true);
        }

        // Code here is restructured based on Weiss
        // First part is clear here: if we did LMR and score beats alpha
        // We need to do a re-search.
        //
        // If we did not do LMR: if we are in a non-PV our we already have alpha == beta - 1,
        // and if we are searching 2nd move and so on we already did full window search -
        // So for both of this cases we do limited window search.
        if (doLMR){
          if (score > alpha){
            score = -_negaMax(movedBoard, &thisPV, tDepth - 1, -alpha - 1, -alpha, false, !cutNode);
          }
        } else if (!pvNode || LegalMoveCount > 1){
          score = -_negaMax(movedBoard, &thisPV, tDepth - 1, -alpha - 1, -alpha, false, !cutNode);
        }

        // If we are in the PV
        // Search with a full window the first move to calculate bounds
        // or if score improved alpha during the current round of search.
        if  (pvNode) {
          if ((LegalMoveCount == 1) || (score > alpha && score < beta)){
            score = -_negaMax(movedBoard, &thisPV, tDepth - 1, -beta, -alpha, false, false);
          }
        }

        _posHist.Remove();
        _sStack.Remove();
        // Beta cutoff
        if (score >= beta) {
          // Add this move as a new killer move and update history if move is quiet
          _updateBeta(isQuiet, move, board.getActivePlayer(), pMove, ply, (depth + 2 * (statEVAL < alpha)));
          // Add a new tt entry for this node
          if (!_stop && !sing){
            myHASH->HASH_Store(board.getZKey().getValue(), move.getMoveINT(), BETA, score, depth, ply);
          }
          // we updated beta and in the pVNode so we should update our pV
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
          alpha = score;
          bestMove = move;
          // we updated alpha and in the pVNode so we should update our pV
          if (pvNode && !_stop){
            up_pV->length = thisPV.length + 1;
            up_pV->pVmoves[0] = move.getMoveINT();
            // memcpy - (куда, откуда, длина)
            std::memcpy(up_pV->pVmoves + 1, thisPV.pVmoves, sizeof(int) * thisPV.length);
          }

        }else{
          // Beta was not beaten and we dont improve alpha in this case we lower our search history values
          int dBonus = std::max(0, depth - (statEVAL < alpha) - (!TTmove && depth >= 4));
          if (isQuiet){
            _orderingInfo.decrementHistory(board.getActivePlayer(), move.getFrom(), move.getTo(), dBonus);
            _orderingInfo.decrementCounterHistory(board.getActivePlayer(), pMoveIndx, move.getPieceType(), move.getTo(), dBonus);
          }else{
            _orderingInfo.decrementCapHistory(move.getPieceType(), move.getCapturedPieceType(), move.getTo(), dBonus);
          }
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
  if (!_stop && !sing){
      if (alpha <= alphaOrig) {
        myHASH->HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), ALPHA, alpha, depth, ply);
      } else {
        myHASH->HASH_Store(board.getZKey().getValue(), bestMove.getMoveINT(), EXACT, alpha, depth, ply);
      }
  }

  return alpha;
}

int Search::_qSearch(const Board &board, int alpha, int beta) {
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
  MoveList * legalMoves = movegen.getMoves();
  MovePicker movePicker
      (&_orderingInfo, &board, legalMoves, 0, board.getActivePlayer(), MAX_PLY, 0);

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

    // Use Halogen futility variation
    if (!(move.getFlags() & Move::PROMOTION) && standPat + move.getValue() + DELTA_MOVE_CONST < alpha)
      break;

    Board movedBoard = board;
    movedBoard.doMove(move);
      if (!movedBoard.colorIsInCheck(movedBoard.getInactivePlayer())){

          int score = -_qSearch(movedBoard, -beta, -alpha);

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
