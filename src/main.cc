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
#include "uci.h"
#include "attacks.h"
#include "movepicker.h"
#include "eval.h"
#include "rays.h"
#include "tuning.h"
#include "bench.h"
#include <cstring>

extern  HASH * myHASH;
OrderingInfo * myOrdering;

int main(int argCount, char* argValue[]) {
  Rays::init();
  PSquareTable::init();
  ZKey::init();
  Attacks::init();
  Eval::init();

  myHASH = new HASH();
  myHASH->HASH_Initalize_MB(16);

  myOrdering = new OrderingInfo();

  #ifdef _TUNE_
  TunerStart();
  #else
  if (argCount > 1 && strcmp("bench", argValue[1]) == 0){
    myBench();
    return 0;
  }else if(argCount > 1 && strcmp("see", argValue[1]) == 0){
    testSEE();
    return 0;
  }else{
    Uci::init();
    Uci::start();
  }

  #endif

  return 0;
}
