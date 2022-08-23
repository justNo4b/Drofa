#include "defs.h"
#include "eval.h"
#include "attacks.h"
#include "movegen.h"
#include "endgame.h"


egEvalEntry myEvalHash [EG_HASH_SIZE];


int Eval::evaluateDraw(const Board &board, Color color){
    return 0;
}

int Eval::evaluateRookMinor_Rook(const Board &board, Color color){
    // While this endgame is usually a draw, there a small chances to win it
    // According to syzygy ~20% of such positions is a win
    int s = DRAW_WITH_ADVANTAGE;

    // Grab PSQT to determine a losing side
    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = psqt > 0 ? getOppositeColor(color) : color;
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(getOppositeColor(weak), KING));

    // 2. Apply bonuses and penalties
    // The only real way to lose is to get mated, so penalize being on the edge and with enemy king near
    s += 8 - Eval::detail::DISTANCE[weakKing][strongKing];
    s += 8 - _endgedist(weakKing);

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;

}

inline void Eval::egHashAdd(std::string psFen, egEvalFunction ef){
    ZKey key;
    key.setpKeyFromString(psFen);
    U64 index = key.getValue() & (EG_HASH_SIZE - 1);
    if (myEvalHash[index].eFunction != nullptr){
        std::cout << "error collision on " << psFen << std::endl;
        exit(0);
    }
    myEvalHash[index] = egEvalEntry(key.getValue(), ef);
}

void Eval::initEG(){

    // initiate table with zero entries
    for (int i = 0; i < EG_HASH_SIZE; i++){
        myEvalHash[i] = egEvalEntry();
    }

    // Add some generic draws
    // 0. 2-man (KvsK) is draw
    egHashAdd("k/K", &evaluateDraw);

    // 3-man eval:
    // King vs King + Bishop = insufficient material
    egHashAdd("kb/K", &evaluateDraw);
    egHashAdd("k/KB", &evaluateDraw);
    // King vs King + Knight = insufficient material
    egHashAdd("kn/K", &evaluateDraw);
    egHashAdd("k/KN", &evaluateDraw);


    // 4-man eval
    // Obviously KN vs KB etc is draw also
    egHashAdd("kn/KN", &evaluateDraw);
    egHashAdd("kb/KB", &evaluateDraw);
    egHashAdd("kn/KB", &evaluateDraw);
    egHashAdd("kb/KN", &evaluateDraw);
    // R vs R is also a draw
    egHashAdd("kr/KR", &evaluateDraw);
    // King vs King + two knights is a draw
    egHashAdd("k/KNN", &evaluateDraw);
    egHashAdd("knn/K", &evaluateDraw);


    // 5-man eval
    // lets say
    // King, Rook, Bishop vs King and Rook
    egHashAdd("krb/KR", &evaluateRookMinor_Rook);
    egHashAdd("kr/KRB", &evaluateRookMinor_Rook);
    // King, Rook, Knight vs King and Rook
    egHashAdd("krn/KR", &evaluateRookMinor_Rook);
    egHashAdd("kr/KRN", &evaluateRookMinor_Rook);
    // Obvious minors draws
    egHashAdd("knn/KN", &evaluateDraw);
    egHashAdd("kn/KNN", &evaluateDraw);
    egHashAdd("kbb/KB", &evaluateDraw);
    egHashAdd("kb/KBB", &evaluateDraw);
}