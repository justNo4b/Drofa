#include "uci.h"
#include "attacks.h"
#include "movepicker.h"
#include "eval.h"
#include "rays.h"
#include "tuning.h"
#include "bench.h"
#include <cstring>

extern  HASH  * myHASH;
OrderingInfo  * myOrdering;

int main(int argCount, char* argValue[]) {
  Rays::init();
  PSquareTable::init();
  ZKey::init();
  Attacks::init();
  Eval::init();

  myHASH = new HASH();
  myHASH->HASH_Initalize_MB(16);

  myOrdering    = new OrderingInfo();

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
