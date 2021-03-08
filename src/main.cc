#include "uci.h"
#include "attacks.h"
#include "movepicker.h"
#include "eval.h"
#include "rays.h"
#include "tuning.h"

extern HASH myHASH;

int main() {
  Rays::init();
  PSquareTable::init();
  ZKey::init();
  Attacks::init();
  Eval::init();

  #ifdef _TUNE_
  TunerStart();
  #else
  Uci::init();
  Uci::start();
  #endif

  return 0;
}
