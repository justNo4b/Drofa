#include "uci.h"
#include "attacks.h"
#include "movepicker.h"
#include "eval.h"
#include "rays.h"
#include "tuning.h"
#include "bench.h"
#include <cstring>

extern HASH * myHASH;

int main(int argCount, char* argValue[]) {
  Rays::init();
  PSquareTable::init();
  ZKey::init();
  Attacks::init();
  Eval::init();

  myHASH = new HASH();
  myHASH->HASH_Initalize_MB(16);

  #ifdef _TUNE_
  TunerStart();
  #else
  if (argCount > 1 && strcmp("bench", argValue[1]) == 0){
    myBench();
    return 0;
  }else if(argCount > 1 && strcmp("see", argValue[1]) == 0){
    testSEE();
    return 0;
  }else if(argCount > 1 && strcmp("filter", argValue[1]) == 0){
    std::string myFen;
    std::ifstream file(argValue[2]);
    std::ofstream ofile(argValue[3]);
    Board board = Board();
    Search::Limits limits;
    Hist history = Hist();
    std::shared_ptr<Search> search = std::make_shared<Search>(board, limits, history, false);

    while (!file.eof()){ //
        std::getline(file, myFen);
        board = Board(myFen);
        int qSscore = search->_qSearch(board);
        int EvalScore = Eval::evaluate(board, board.getActivePlayer());
        if (qSscore == EvalScore){
              ofile << myFen << std::endl;
        }
    }


    file.close();
    ofile.close();

  }else{
    Uci::init();
    Uci::start();
  }

  #endif

  return 0;
}
