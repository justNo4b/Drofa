#include "defs.h"
#include "eval.h"
#include "attacks.h"
#include "movegen.h"
#include "endgame.h"


egEvalEntry myEvalHash [EG_HASH_SIZE];


int Eval::evaluateDraw(const Board &board, Color color){
    return 0;
}

int Eval::evaluateMinor_vs_Pawns(const Board &board, Color color){
    int s = Eval::evaluateMain(board, color);
    Color other = getOppositeColor(color);
    // Use main evaluation. However if side with lone knight is 'winning'
    // Squish its score down by diving 128

    s = (s > 0 && ((board.getPieces(color, KNIGHT) | board.getPieces(color, BISHOP)) != 0)) ? s / 128 :
        (s < 0 && ((board.getPieces(other, KNIGHT) | board.getPieces(other, BISHOP)) != 0)) ? s / 128 :
        s;

    return s;
}

int Eval::evaluateQueen_vs_X(const Board &board, Color color){
    int s = EASY_WIN_SCORE;

    // for Q vs X -> X is other non-pawn piece
    // increase eval for keeping kings close and keeping weaker king closer to the edge

    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = psqt > 0 ? getOppositeColor(color) : color;
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(getOppositeColor(weak), KING));

    // 2. Apply bonuses and penalties
    s += 8 - Eval::detail::DISTANCE[weakKing][strongKing];
    s += 8 - _endgedist(weakKing);

    // 3. LoneKingBonus - add some eval if weak side has no pieces
    // Otherwise it wont pick up material
    s += 100 * (_popCount(board.getAllPieces(weak)) == 1);

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;
}

int Eval::evaluateHugeAdvantage(const Board &board, Color color){
    int s = MINIMAL_WON_SCORE;
    return s;
}

inline void Eval::egHashAdd(std::string psFen, egEvalFunction ef){
    ZKey key;
    key.setpKeyFromString(psFen);
    U64 index = key.getCountValue() & (EG_HASH_SIZE - 1);
    myEvalHash[index] = egEvalEntry(key.getCountValue(), ef);
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
    // King vs King + Rook   = win;
    egHashAdd("kr/K", &evaluateQueen_vs_X);
    egHashAdd("k/KR", &evaluateQueen_vs_X);
    // King vs King + Queen  = win
    egHashAdd("kq/K", &evaluateQueen_vs_X);
    egHashAdd("k/KQ", &evaluateQueen_vs_X);
    // ToDo KPK

    // 4-man eval
    // Obviously KN vs KB etc is draw also
    egHashAdd("kn/KN", &evaluateDraw);
    egHashAdd("kb/KB", &evaluateDraw);
    egHashAdd("kn/KB", &evaluateDraw);
    egHashAdd("kb/KN", &evaluateDraw);
    // R vs R is also a draw
    egHashAdd("kr/KR", &evaluateDraw);
    // KB vs KP -> use special eval (draw unless pawns have advantage)
    egHashAdd("kb/KP", &evaluateMinor_vs_Pawns);
    egHashAdd("kp/KB", &evaluateMinor_vs_Pawns);
    // KN vs KP -> use special eval (draw unless pawns have advantage)
    egHashAdd("kn/KP", &evaluateMinor_vs_Pawns);
    egHashAdd("kp/KN", &evaluateMinor_vs_Pawns);
    // King vs King + two knights is a draw
    egHashAdd("k/KNN", &evaluateDraw);
    egHashAdd("knn/K", &evaluateDraw);
    // same if losing side has a minors
    egHashAdd("k/KNN", &evaluateDraw);
    egHashAdd("knn/K", &evaluateDraw);
    // Assume R vs B is always draw
    egHashAdd("kb/KR", &evaluateDraw);
    egHashAdd("kr/KB", &evaluateDraw);
    // Same for R vs N
    egHashAdd("kn/KR", &evaluateDraw);
    egHashAdd("kr/KN", &evaluateDraw);
    // Trivial win for Q vs (R or B or N)
    egHashAdd("kq/KB", &evaluateQueen_vs_X);
    egHashAdd("kb/KQ", &evaluateQueen_vs_X);
    egHashAdd("kq/KN", &evaluateQueen_vs_X);
    egHashAdd("kn/KQ", &evaluateQueen_vs_X);
    egHashAdd("kq/KR", &evaluateQueen_vs_X);
    egHashAdd("kr/KQ", &evaluateQueen_vs_X);


    // 5-man eval
    // lets say
    // King, Rook, Bishop vs King and Rook
    egHashAdd("krb/KR", &evaluateDraw);
    egHashAdd("kr/KRB", &evaluateDraw);
    // King, Rook, Knight vs King and Rook
    egHashAdd("krn/KR", &evaluateDraw);
    egHashAdd("kr/KRN", &evaluateDraw);
}