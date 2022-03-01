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
  int Killer1 = _orderingInfo->getKiller1(_ply);
  int Killer2 = _orderingInfo->getKiller2(_ply);
  int Counter = _orderingInfo->getCounterMoveINT(_color, _pMove);

  for (auto &move : *_moves) {
    int moveINT = move.getMoveINT();
    if (_hashMove != 0 && moveINT == _hashMove) {
      move.setValue(INF);
    } else if (move.getFlags() & Move::CAPTURE) {
      int see   = board->Calculate_SEE(move);
      int value = _ply == MAX_PLY ? (see + 1) :
                                  opS(Eval::MATERIAL_VALUES[move.getCapturedPieceType()]) +
                                  _orderingInfo->getCaptureHistory(move.getPieceType(),move.getCapturedPieceType(), move.getTo());
      if (_ply != MAX_PLY){
        value += see >= 0 ? CAPTURE_BONUS : BAD_CAPTURE;
      }
      move.setValue(value);
    } else if (move.getFlags() & Move::PROMOTION) {
      int value = _ply == MAX_PLY ? 0 : PROMOTION_SORT[move.getPromotionPieceType()];
      move.setValue(value);
    } else if (moveINT == Killer1) {
      move.setValue(KILLER1_BONUS);
    } else if (moveINT == Killer2) {
      move.setValue(KILLER2_BONUS);
    } else if (moveINT == Counter){
      move.setValue(COUNTERMOVE_BONUS);
    } else { // Quiet
      move.setValue(_orderingInfo->getHistory(_color, move.getFrom(), move.getTo()) +
                    _orderingInfo->getCountermoveHistory(_color, _pMove, move.getPieceType(), move.getTo()));
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
    if (_moves->at(i).getValue() > bestScore) {
      bestScore = _moves->at(i).getValue();
      bestIndex = i;
    }
  }

  std::swap(_moves->at(_currHead), _moves->at(bestIndex));
  return _moves->at(_currHead++);
}

void MovePicker::refreshPicker(){
  _currHead = 0;
}