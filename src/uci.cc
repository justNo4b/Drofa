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
#include <memory>
#include "uci.h"
#include "version.h"
#include "eval.h"
#include "searchdata.h"
#include "timer.h"
#include <iostream>
#include <thread>

extern HASH         * myHASH;
extern OrderingInfo * myOrdering;

int  myTHREADSCOUNT = 1;

OrderingInfo  * cOrdering[MAX_THREADS];
Search        * cSearch[MAX_THREADS];
std::thread     cThread[MAX_THREADS];

Limits uci_timer;

namespace {
Book book;
std::shared_ptr<Search> search;
Board board;
Hist positionHistory = Hist();

void loadBook() {
  std::ifstream bookFile(optionsMap["BookPath"].getValue());
  bool bookOk = bookFile.good();
  bookFile.close();

  if (bookOk) {
    book = Book(optionsMap["BookPath"].getValue());
  } else {
    std::cerr << optionsMap["BookPath"].getValue() << " is inaccessible or doesn't exist" << std::endl;
  }
}

void changeTTsize(){
  int size = atoi(optionsMap["Hash"].getValue().c_str());
  // make sure we do not overstep bounds
  size = std::min(size, MAX_HASH);
  size = std::max(size, MIN_HASH);
  // call TT
  myHASH->HASH_Initalize_MB(size);
}

void changeThreadsNumber(){
  int tNum = atoi(optionsMap["Threads"].getValue().c_str());
  // Make sure threads are within min/max bounds
  tNum = std::min(tNum, MAX_THREADS);
  tNum = std::max(tNum, MIN_THREADS);

  // Change number
  myTHREADSCOUNT = tNum;

  // Create ordering for them
  if (myTHREADSCOUNT > 1){
    for (int i = 1; i < myTHREADSCOUNT; i++){
      cOrdering[i] = new OrderingInfo();
    }
  }
}


void loadCosts(){

uci_timer.width_a  = atoi(optionsMap["width_a"].getValue().c_str());
uci_timer.width    = atoi(optionsMap["width"].getValue().c_str());
uci_timer.t_move   = atoi(optionsMap["t_move"].getValue().c_str());
uci_timer.c_move   = atoi(optionsMap["crit_move"].getValue().c_str());

uci_timer.mtg_incr = atoi(optionsMap["mtg_cycl_incr"].getValue().c_str());
uci_timer.nodes_max = atoi(optionsMap["nodes_max"].getValue().c_str());
uci_timer.nodes_min = atoi(optionsMap["nodes_min"].getValue().c_str());
uci_timer.nodes_div = atoi(optionsMap["nodes_div"].getValue().c_str());
uci_timer.nodes_fact = atoi(optionsMap["nodes_fact"].getValue().c_str());

}


void initOptions() {
  optionsMap["OwnBook"] = Option(false);
  optionsMap["BookPath"] = Option("book.bin", &loadBook);
  optionsMap["Hash"] = Option(MIN_HASH, MIN_HASH, MAX_HASH, &changeTTsize);
  optionsMap["Threads"] = Option(MIN_THREADS, MIN_THREADS, MAX_THREADS, &changeThreadsNumber);
  optionsMap["UCI_Chess960"] = Option(false);


  // Options for tuning is defined here.
  // They are used only if programm is build with "make tune"
  // Tuning versionshould not be the one playing regular games
  // but having this options here allows tuner
  // to change different parameters via communocation
  // with the engine.


  optionsMap["width_a"] =       Option(300, 100, 500, &loadCosts);
  optionsMap["width"] =         Option(175, 50, 300, &loadCosts);
  optionsMap["t_move"] =        Option(20, 8, 40, &loadCosts);
  optionsMap["crit_move"] =     Option(28, 8, 40, &loadCosts);
  optionsMap["mtg_cycl_incr"] = Option(10, 2, 30, &loadCosts);
  optionsMap["nodes_max"] =     Option(85, 50, 100, &loadCosts);
  optionsMap["nodes_min"] =     Option(25, 1, 50, &loadCosts);
  optionsMap["nodes_fact"] =    Option(50, 25, 75, &loadCosts);
  optionsMap["nodes_div"] =     Option(50, 25, 75, &loadCosts);



}

void uciNewGame() {
  board.setToStartPos();
  positionHistory = Hist();
}

void setPosition(std::istringstream &is) {
  std::string token;
  is >> token;

  if (token == "startpos") {
    board.setToStartPos();
  } else {
    std::string fen;

    while (is >> token && token != "moves") {
      fen += token + " ";
    }

    board.setToFen(fen, (optionsMap["UCI_Chess960"].getValue() == "true"));
  }

  while (is >> token) {
    if (token == "moves") {
      continue;
    }

    MoveGen movegen(board, false);
    MoveList * moves = movegen.getMoves();
    for (auto &move : *moves) {
      if (move.getNotation((optionsMap["UCI_Chess960"].getValue() == "true")) == token) {
        board.doMove(move);
        if ((move.getPieceType() == PAWN) || (move.getFlags() & Move::CAPTURE) ){
          positionHistory = Hist();
        }
        positionHistory.Add(board.getZKey().getValue());
        break;
      }
    }
  }
}

void pickBestMove() {
  if (optionsMap["OwnBook"].getValue() == "true" && book.inBook(board)) {
    std::cout << "bestmove " << book.getMove(board).getNotation(board.getFrcMode()) << std::endl;
  } else {
    search->iterDeep();
  }
}

void go(std::istringstream &is) {
  std::string token;

  while (is >> token) {
    if (token == "depth") is >> uci_timer.depth;
    else if (token == "infinite") uci_timer.infinite = true;
    else if (token == "movetime") is >> uci_timer.moveTime;
    else if (token == "nodes") is >> uci_timer.nodes;
    else if (token == "wtime") is >> uci_timer.time[WHITE];
    else if (token == "btime") is >> uci_timer.time[BLACK];
    else if (token == "winc") is >> uci_timer.increment[WHITE];
    else if (token == "binc") is >> uci_timer.increment[BLACK];
    else if (token == "movestogo") is >> uci_timer.movesToGo;
  }

// if we have > 1 threads, run some additional threads
  if (myTHREADSCOUNT > 1){
    for (int i = 1; i < myTHREADSCOUNT; i++){
      // copy board stuff
      Board b = board;
      Limits l = uci_timer;
      Hist h = positionHistory;

      // clear killers for every ordering
      cOrdering[i]->clearKillers();
      // create search and assign to the thread
      cSearch[i] = new Search(b, l, h, cOrdering[i], false);
      cThread[i] = std::thread(&Search::iterDeep, cSearch[i]);
    }
  }

  myOrdering->clearKillers();
  search = std::make_shared<Search>(board, uci_timer, positionHistory, myOrdering);

  std::thread searchThread(&pickBestMove);
  searchThread.detach();


}

unsigned long long perft(const Board &board, int depth) {
  if (depth <= 0) {
    return 1;
  } else if (depth == 1) {
    return MoveGen(board, false).getMoves()->size();
  }

  MoveGen movegen(board, false);
  MoveList * moves = movegen.getMoves();
  unsigned long long nodes = 0;
  for (auto &move : *moves) {
    Board movedBoard = board;
    movedBoard.doMove(move);

    nodes += perft(movedBoard, depth - 1);
  }

  return nodes;
}

void perftDivide(int depth) {
  unsigned long long total = 0;

  MoveGen movegen(board, false);

  std::cout << std::endl;
  auto start = std::chrono::steady_clock::now();
  MoveList * moves = movegen.getMoves();
  for (auto &move : *moves) {
    Board movedBoard = board;
    movedBoard.doMove(move);

    unsigned long long perftRes = perft(movedBoard, depth - 1);
    total += perftRes;

    std::cout << move.getNotation(board.getFrcMode()) << ": " << perftRes << std::endl;
  }
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::cout << std::endl << "==========================" << std::endl;
  std::cout << "Total time (ms) : " << static_cast<int>(elapsed.count() * 1000) << std::endl;
  std::cout << "Nodes searched  : " << total << std::endl;
  std::cout << "Nodes / second  : " << static_cast<int>(total / elapsed.count()) << std::endl;
}

void printEngineInfo() {
  std::cout << "id name Drofa " << VER_MAJ << "." << VER_MIN << "." << VER_PATCH << std::endl;
  std::cout << "id author Rhys Rustad-Elliott and Alexander Litov" << std::endl;
#ifdef _TUNE_
  std::cout << "This is _TUNE_ build, it can be slower" << std::endl;
#endif

  std::cout << std::endl;

  for (auto optionPair : optionsMap) {
    std::cout << "option ";
    std::cout << "name " << optionPair.first << " ";
    std::cout << "type " << optionPair.second.getType() << " ";
    std::cout << "default " << optionPair.second.getDefaultValue();

    if (optionPair.second.getType() == "spin") {
      std::cout << " ";
      std::cout << "min " << optionPair.second.getMin() << " ";
      std::cout << "max " << optionPair.second.getMax();
    }
    std::cout << std::endl;
  }
  std::cout << "uciok" << std::endl;
}

void setOption(std::istringstream &is) {
  std::string token;
  std::string optionName;

  is >> token >> optionName; // Advance past "name"

  if (optionsMap.find(optionName) != optionsMap.end()) {
    is >> token >> token; // Advance past "value"
    optionsMap[optionName].setValue(token);
  } else {
    std::cout << "Invalid option" << std::endl;
  }
}

void loop() {
  std::cout << "Drofa " << VER_MAJ << "." << VER_MIN << "." << VER_PATCH;
  std::cout << " by Rhys Rustad-Elliott and Litov Alexander";
  std::cout << " (built " << __DATE__ << " " << __TIME__ << ")" << std::endl;

#ifdef _TUNE_
  std::cout << "This is _TUNE_ build, it can be slower" << std::endl;
#endif

#ifdef __DEBUG__
  std::cout << "***DEBUG BUILD (This will be slow)***" << std::endl;
#endif

  board.setToStartPos();

  std::string line;
  std::string token;
  // load non-std uci options
  loadCosts();
  // dump shit we are tuning

  std::cout << "width_a" << ", int, " << optionsMap["width_a"].getDefaultValue() << ", " << optionsMap["width_a"].getMin() << ", " << optionsMap["width_a"].getMax() << ", 5, 0.002"<< std::endl;
  std::cout << "width" << ", int, " << optionsMap["width"].getDefaultValue() << ", " << optionsMap["width"].getMin() << ", " << optionsMap["width"].getMax() << ", 5, 0.002"<< std::endl;
  std::cout << "t_move" << ", int, " << optionsMap["t_move"].getDefaultValue() << ", " << optionsMap["t_move"].getMin() << ", " << optionsMap["t_move"].getMax() << ", 1, 0.002"<< std::endl;
  std::cout << "crit_move" << ", int, " << optionsMap["crit_move"].getDefaultValue() << ", " << optionsMap["crit_move"].getMin() << ", " << optionsMap["crit_move"].getMax() << ", 1, 0.002"<< std::endl;
  std::cout << "mtg_cycl_incr" << ", int, " << optionsMap["mtg_cycl_incr"].getDefaultValue() << ", " << optionsMap["mtg_cycl_incr"].getMin() << ", " << optionsMap["mtg_cycl_incr"].getMax() << ", 1, 0.002"<< std::endl;

  std::cout << "nodes_max" << ", int, " << optionsMap["nodes_max"].getDefaultValue() << ", " << optionsMap["nodes_max"].getMin() << ", " << optionsMap["nodes_max"].getMax() << ", 1, 0.002"<< std::endl;
  std::cout << "nodes_min" << ", int, " << optionsMap["nodes_min"].getDefaultValue() << ", " << optionsMap["nodes_min"].getMin() << ", " << optionsMap["nodes_min"].getMax() << ", 1, 0.002"<< std::endl;
  std::cout << "nodes_fact" << ", int, " << optionsMap["nodes_fact"].getDefaultValue() << ", " << optionsMap["nodes_fact"].getMin() << ", " << optionsMap["nodes_fact"].getMax() << ", 1, 0.002"<< std::endl;
  std::cout << "nodes_div" << ", int, " << optionsMap["nodes_div"].getDefaultValue() << ", " << optionsMap["nodes_div"].getMin() << ", " << optionsMap["nodes_div"].getMax() << ", 1, 0.002"<< std::endl;


  while (std::getline(std::cin, line)) {
    std::istringstream is(line);
    is >> token;

    if (token == "uci") {
      printEngineInfo();
    } else if (token == "ucinewgame") {
      uciNewGame();
    } else if (token == "isready") {
      std::cout << "readyok" << std::endl;
    } else if (token == "stop") {
      if (search) search->stop();
    } else if (token == "go") {
      go(is);
    } else if (token == "quit") {
      if (search) search->stop();
      return;
    } else if (token == "position") {
      setPosition(is);
    } else if (token == "setoption") {
      setOption(is);
    }

      // Non UCI commands
    else if (token == "printboard") {
      std::cout << std::endl << board.getStringRep() << std::endl;
    } else if (token == "printmoves") {
      MoveGen movegen(board, false);
      MoveList * moves = movegen.getMoves();
      for (auto &move : *moves) {
        std::cout << move.getNotation(board.getFrcMode()) << " ";
      }
      std::cout << std::endl;
    } else if (token == "perft") {
      int depth = 1;
      is >> depth;
      perftDivide(depth);
    } else {
      std::cout << "what?" << std::endl;
    }
  }
}
}

void Uci::init() {
  initOptions();
}

void Uci::start() {
  loop();
}
