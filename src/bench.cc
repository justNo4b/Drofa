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
#include "bench.h"
#include "searchdata.h"
#include "search.h"
#include "move.h"
#include "timer.h"
#include <iostream>
#include <memory>


extern HASH         * myHASH;
extern OrderingInfo * myOrdering;

void myBench(){
    std::cout << "Bench started..." << std::endl;
    std::chrono::time_point<std::chrono::steady_clock> timer_start = std::chrono::steady_clock::now();
    Board board = Board();
    int nodes_total = 0;

    Limits limits;
    limits.depth = BENCH_SEARCH_DEPTH;
    Hist history = Hist();
    std::shared_ptr<Search> search;

    for (int i = 0; i < BENCH_POS_NUMBER; i++){
        int curNodes = 0;
        board = Board(BENCH_POSITION[i], false);
        search = std::make_shared<Search>(board, limits, history, myOrdering, false);
        search->iterDeep();
        curNodes = search->getNodes();
        nodes_total += curNodes;
        myHASH->HASH_Clear();
        myOrdering->clearAllHistory();
        printf("Position [# %2d] Best: %6s %5i cp  Nodes: %12i", i + 1,search->getBestMove().getNotation(board.getFrcMode()).c_str(),
                 search->getBestScore(), curNodes);
        std::cout << std::endl;
    }

    int elapsed =std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timer_start).count();
    std::cout << "==============================================================="<<std::endl;
    printf("OVERALL: %12d nodes %8d nps\n", nodes_total, (int) (1000.0f * nodes_total / (elapsed + 1)));
    std::cout << std::flush;
};


void testSEE(){
    for (int j = 0; j < 9; j++){
        Board board = Board(SEE_POSITION[j], false);
        Move move = SEE_MOVE[j];
        int i = board.Calculate_SEE(move);
        bool k = board.SEE_GreaterOrEqual(move, 0);
        std::cout << i << " " << k << std::endl;
    }

}