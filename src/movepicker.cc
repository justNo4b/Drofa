#include "movepicker.h"
#include "eval.h"
#include "defs.h"

extern HASH myHASH;
// Indexed by [victimValue][attackerValue]


MovePicker::MovePicker(const OrderingInfo *orderingInfo, MoveList *moveList, U64 key, Color color, int ply){
  _orderingInfo = orderingInfo;
  _moves = moveList;
  _posKey = key;
  _color = color;
  _ply = ply;
//  _pMove = pMove;
  _currHead = 0;
  _scoreMoves();
}

void MovePicker::_scoreMoves() {
  int pvMove;
  HASH_Entry pvEntry = myHASH.HASH_Get(_posKey);
  if (pvEntry.Flag != NONE){
    pvMove = pvEntry.move;
  }else{
    pvMove = 0;
  }
  int Killer1 = _orderingInfo->getKiller1(_ply);
  int Killer2 = _orderingInfo->getKiller2(_ply);

  for (auto &move : *_moves) {
    int moveINT = move.getMoveINT();
    if (moveINT == pvMove) {
      move.setValue(INF);
    } else if (move.getFlags() & Move::CAPTURE) {
      move.setValue(CAPTURE_BONUS + _mvvLvaTable[move.getCapturedPieceType()][move.getPieceType()]);
    } else if (move.getFlags() & Move::PROMOTION) {
      move.setValue(PROMOTION_BONUS + Eval::getMaterialValue(ENDGAME, move.getPromotionPieceType()));
    } else if (moveINT == Killer1) {
      move.setValue(KILLER1_BONUS);
    } else if (moveINT == Killer2) {
      move.setValue(KILLER2_BONUS);
    } else { // Quiet
      move.setValue(QUIET_BONUS + _orderingInfo->getHistory(_color, move.getFrom(), move.getTo()));
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