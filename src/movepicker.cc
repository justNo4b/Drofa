#include "movepicker.h"
#include "eval.h"
#include "defs.h"

extern HASH myHASH;
// Indexed by [victimValue][attackerValue]


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
      if (board->Calculate_SEE(move) >= 0){
        move.setValue(CAPTURE_BONUS + _mvvLvaTable[move.getCapturedPieceType()][move.getPieceType()]);
      }else{
        move.setValue(BAD_CAPTURE + _mvvLvaTable[move.getCapturedPieceType()][move.getPieceType()]);
      }
    } else if (move.getFlags() & Move::PROMOTION) {
      move.setValue(PROMOTION_SORT[move.getPromotionPieceType()]);
    } else if (moveINT == Killer1) {
      move.setValue(KILLER1_BONUS);
    } else if (moveINT == Killer2) {
      move.setValue(KILLER2_BONUS);
    } else { // Quiet
      move.setValue(_orderingInfo->getCounterHistory(_pMove, move.getPieceType(), move.getTo()) + _orderingInfo->getHistory(_color, move.getFrom(), move.getTo()));
      if (moveINT == Counter){
        move.setValue( move.getValue() + COUNTERMOVE_BONUS );
      }
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