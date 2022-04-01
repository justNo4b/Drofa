#include "movepicker.h"
#include "eval.h"
#include "defs.h"


MovePicker::MovePicker(const OrderingInfo *orderingInfo, const Board *board, MoveList *moveList, int hMove, Color color, int ply, int pMove){
  _orderingInfo = orderingInfo;
  _moves = moveList;
  _hashMove = hMove;
  _color = color;
  _ply = ply;
  _pMove = pMove;
  _currHead = 0;
  _scoreMoves(board);
}

void MovePicker::_scoreMoves(const Board *board) {
  int Killer1  = _orderingInfo->getKiller1(_ply);
  int Killer2  = _orderingInfo->getKiller2(_ply);
  int Counter  = _orderingInfo->getCounterMoveINT(_color, _pMove);
  int pMoveInx = (_pMove & 0x7) + ((_pMove >> 15) & 0x3f) * 6;

  for (auto &move : *_moves) {
    int moveINT = move.getMoveINT();
    if (_hashMove != 0 && moveINT == _hashMove) {
      move.setValue(moveValue(0, HASH_MOVE));
    } else if (move.getFlags() & Move::CAPTURE) {
      int see   = board->Calculate_SEE(move);
      int value = _ply == MAX_PLY ? see :
                                  (opS(Eval::MATERIAL_VALUES[move.getCapturedPieceType()]) / 4) +
                                  _orderingInfo->getCaptureHistory(move.getPieceType(),move.getCapturedPieceType(), move.getTo());
      if (_ply != MAX_PLY){
        value += see >= 0 ? CAPTURE_BONUS : BAD_CAPTURE;
      }
      move.setValue(moveValue(see, value));
    } else if (move.getFlags() & Move::PROMOTION) {
      move.setValue(moveValue(0, PROMOTION_SORT[move.getPromotionPieceType()]));
    } else if (moveINT == Killer1) {
      move.setValue(moveValue(0, KILLER1_BONUS));
    } else if (moveINT == Killer2) {
      move.setValue(moveValue(0, KILLER2_BONUS));
    } else if (moveINT == Counter){
      move.setValue(moveValue(0, COUNTERMOVE_BONUS));
    } else { // Quiet
      int value = _orderingInfo->getHistory(_color, move.getFrom(), move.getTo()) +
                  _orderingInfo->getCountermoveHistory(_color, pMoveInx, move.getPieceType(), move.getTo());
      move.setValue(moveValue(0, value));
    }
  }
}


bool MovePicker::hasNext() const {
  return _currHead < _moves->size();
}

Move MovePicker::getNext() {
  size_t bestIndex;
  int bestScore = -INF;

  for (size_t i = _currHead; i < _moves->size(); i++) {
    int value = getVal(_moves->at(i).getValue());
    if ( value > bestScore) {
      bestScore = value;
      bestIndex = i;
    }
  }

  std::swap(_moves->at(_currHead), _moves->at(bestIndex));
  return _moves->at(_currHead++);
}

void MovePicker::refreshPicker(){
  _currHead = 0;
}