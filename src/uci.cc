#include <memory>
#include "uci.h"
#include "version.h"
#include "eval.h"
#include "poshistory.h"
#include <iostream>
#include <thread>

extern HASH         * myHASH;
extern OrderingInfo * myOrdering;

int myTHREADSCOUNT = 1;

OrderingInfo  * cOrdering[MAX_THREADS];

Search        * cSearch[MAX_THREADS];
Poshistory    * cPosHist[MAX_THREADS];
std::thread     cThread[MAX_THREADS];

namespace {
Book book;
std::shared_ptr<Search> search;
Board board;
Poshistory *basepHist = new Poshistory();

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
}

#ifdef _TUNE_
void loadCosts(){

Eval::SetupTuning(OPENING, PAWN, atoi(optionsMap["vPawnOP"].getValue().c_str()));
Eval::SetupTuning(ENDGAME, PAWN, atoi(optionsMap["vPawnEG"].getValue().c_str()));

Eval::SetupTuning(OPENING, KNIGHT, atoi(optionsMap["vKnightOP"].getValue().c_str()));
Eval::SetupTuning(ENDGAME, KNIGHT, atoi(optionsMap["vKnightEG"].getValue().c_str()));

Eval::SetupTuning(OPENING, BISHOP, atoi(optionsMap["vBishopOP"].getValue().c_str()));
Eval::SetupTuning(ENDGAME, BISHOP, atoi(optionsMap["vBishopEG"].getValue().c_str()));

Eval::SetupTuning(OPENING, ROOK, atoi(optionsMap["vRookOP"].getValue().c_str()));
Eval::SetupTuning(ENDGAME, ROOK, atoi(optionsMap["vRookEG"].getValue().c_str()));

Eval::SetupTuning(OPENING, QUEEN, atoi(optionsMap["vQueenOP"].getValue().c_str()));
Eval::SetupTuning(ENDGAME, QUEEN, atoi(optionsMap["vQueenEG"].getValue().c_str()));
}
#endif

void initOptions() {
  optionsMap["OwnBook"] = Option(false);
  optionsMap["BookPath"] = Option("book.bin", &loadBook);
  optionsMap["Hash"] = Option(MIN_HASH, MIN_HASH, MAX_HASH, &changeTTsize);
  optionsMap["Threads"] = Option(MIN_THREADS, MIN_THREADS, MAX_THREADS, &changeThreadsNumber);


  // Options for tuning is defined here.
  // They are used only if programm is build with "make tune"
  // Tuning versionshould not be the one playing regular games
  // but having this options here allows tuner
  // to change different parameters via communocation
  // with the engine.

#ifdef _TUNE_
  optionsMap["vPawnOP"] =   Option(100, 25, 2048, &loadCosts);
  optionsMap["vPawnEG"] =   Option(100, 25, 2048, &loadCosts);
  optionsMap["vKnightOP"] = Option(300, 25, 2048, &loadCosts);
  optionsMap["vKnightEG"] = Option(300, 25, 2048, &loadCosts);
  optionsMap["vBishopOP"] = Option(315, 25, 2048, &loadCosts);
  optionsMap["vBishopEG"] = Option(315, 25, 2048, &loadCosts);
  optionsMap["vRookOP"] =   Option(500, 25, 2048, &loadCosts);
  optionsMap["vRookEG"] =   Option(500, 25, 2048, &loadCosts);
  optionsMap["vQueenOP"] =  Option(900, 25, 2048, &loadCosts);
  optionsMap["vQueenEG"] =  Option(900, 25, 2048, &loadCosts);
#endif

}

void uciNewGame() {
  board.setToStartPos();
  Poshistory *basepHist = new Poshistory();
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

    board.setToFen(fen);
  }

  while (is >> token) {
    if (token == "moves") {
      continue;
    }

    MoveGen movegen(board, false);
    for (auto move : movegen.getMoves()) {
      if (move.getNotation() == token) {
        board.doMove(move);
        if ((move.getPieceType() == PAWN) || (move.getFlags() & Move::CAPTURE) ){
          basepHist->ZeroingTables();
        }
        basepHist->AddNode(board.getZKey().getValue(), move.getMoveINT());
        break;
      }
    }
  }
}

void pickBestMove() {
  if (optionsMap["OwnBook"].getValue() == "true" && book.inBook(board)) {
    std::cout << "bestmove " << book.getMove(board).getNotation() << std::endl;
  } else {
    search->iterDeep();
  }
}

void go(std::istringstream &is) {
  std::string token;
  Search::Limits limits;

  while (is >> token) {
    if (token == "depth") is >> limits.depth;
    else if (token == "infinite") limits.infinite = true;
    else if (token == "movetime") is >> limits.moveTime;
    else if (token == "nodes") is >> limits.nodes;
    else if (token == "wtime") is >> limits.time[WHITE];
    else if (token == "btime") is >> limits.time[BLACK];
    else if (token == "winc") is >> limits.increment[WHITE];
    else if (token == "binc") is >> limits.increment[BLACK];
    else if (token == "movestogo") is >> limits.movesToGo;
  }

// if we have > 1 threads, run some additional threads
  if (myTHREADSCOUNT > 1){
    for (int i = 1; i < myTHREADSCOUNT; i++){
      // copy board stuff
      Board b = board;
      Search::Limits l = limits;

      // create new search and start
      cOrdering[i] = new OrderingInfo();
      cPosHist[i] = new Poshistory();
      cSearch[i] = new Search(b, l, cOrdering[i], cPosHist[i], false);
      cThread[i] = std::thread(&Search::iterDeep, cSearch[i]);
    }
  }

  myOrdering->clearKillers();
  basepHist->RemoveLast();
  search = std::make_shared<Search>(board, limits, myOrdering, basepHist);

  std::thread searchThread(&pickBestMove);
  searchThread.detach();


}

unsigned long long perft(const Board &board, int depth) {
  if (depth <= 0) {
    return 1;
  } else if (depth == 1) {
    return MoveGen(board, false).getMoves().size();
  }

  MoveGen movegen(board, false);

  unsigned long long nodes = 0;
  for (auto move : movegen.getMoves()) {
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
  for (auto move : movegen.getMoves()) {
    Board movedBoard = board;
    movedBoard.doMove(move);

    unsigned long long perftRes = perft(movedBoard, depth - 1);
    total += perftRes;

    std::cout << move.getNotation() << ": " << perftRes << std::endl;
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
  std::cout << "id authors Rhys Rustad-Elliott and Alexander Litov" << std::endl;
#ifdef _TUNE_
  std::cout << "This is _TUNE_ build, it can be slower" << std::endl;
#endif

  std::cout << std::endl;

  for (auto optionPair : optionsMap) {
    std::cout << "option ";
    std::cout << "name " << optionPair.first << " ";
    std::cout << "type " << optionPair.second.getType() << " ";
    std::cout << "default " << optionPair.second.getDefaultValue() << " ";

    if (optionPair.second.getType() == "spin") {
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
      for (auto move : MoveGen(board, false).getMoves()) {
        std::cout << move.getNotation() << " ";
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
