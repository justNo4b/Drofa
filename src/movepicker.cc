/*
    Drofa - UCI compatable chess engine
        Copyright (C) 2017 - 2019  Rhys Rustad-Elliott
                      2020 - 2023  Litov Alexander
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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
      move.setValue(INF);
    } else if (move.getFlags() & Move::CAPTURE) {
      int hist  = _orderingInfo->getCaptureHistory(move.getPieceType(),move.getCapturedPieceType(), move.getTo());
      int value = _ply == MAX_PLY ? board->Calculate_SEE(move) : opS(Eval::MATERIAL_VALUES[move.getCapturedPieceType()]) + hist;
      if (_ply != MAX_PLY){
        int th = -((hist / 8192) * 100);
        value += board->SEE_GreaterOrEqual(move, th) ? CAPTURE_BONUS : BAD_CAPTURE;
      }
      move.setValue(value);
    } else if (move.getFlags() & Move::PROMOTION) {
      move.setValue(PROMOTION_SORT[move.getPromotionPieceType()]);
    } else if (moveINT == Killer1) {
      move.setValue(KILLER1_BONUS);
    } else if (moveINT == Killer2) {
      move.setValue(KILLER2_BONUS);
    } else if (moveINT == Counter){
      move.setValue(COUNTERMOVE_BONUS);
    } else { // Quiet
      move.setValue(_orderingInfo->getHistory(_color, move.getFrom(), move.getTo()) +
                    _orderingInfo->getCountermoveHistory(_color, pMoveInx, move.getPieceType(), move.getTo()));
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