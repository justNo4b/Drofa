#include "defs.h"
#include "eval.h"
#include "attacks.h"
#include "movegen.h"


int Eval::evaluateEndgame(const Board &board, Color color){
    return 0;
}

void Eval::initEG(){
    // Veryfy for a change;

    ZKey test;;
    test.setpKeyFromString("k/KB");
    //  kp/K
    // 8/1k6/8/8/8/5P2/3K4/8 w - - 0 1
    Board t("8/1k6/8/8/8/5B2/3K4/8 w - - 0 1");

    std::cout << "kpk: " << test.getValue() << " board: " << t.getpCountKey().getValue() << std::endl;

}