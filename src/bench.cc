#include "bench.h"
#include "board.h"
#include "search.h"
#include <iostream>
#include <memory>




void myBench(){
    std::cout << "Bench started" << std::endl;
    Board board = Board();
    int nodes_total = 0;
    int time_total = 0;

    Search::Limits limits;
    limits.depth = BENCH_SEARCH_DEPTH;
    Hist history = Hist();
    std::shared_ptr<Search> search;


    for (int i = 0; i < BENCH_POS_NUMBER; i++){
        int curNodes = 0;
        int curTime  = 0;
        board = Board(BENCH_POSITION[i]);
        search = std::make_shared<Search>(board, limits, history, false);
        search->iterDeep();
        curNodes = search->getNodes();
        nodes_total += curNodes;
        std::cout << "["<< i + 1 << "] Nodes: " << curNodes <<std::endl;
    }

    std::cout << "Total nodes: " << nodes_total <<std::endl;
};