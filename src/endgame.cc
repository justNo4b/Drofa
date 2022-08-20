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

int Eval::evaluateQueen_vs_Pawn(const Board &board, Color color){
    int s = 0;

    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = psqt > 0 ? getOppositeColor(color) : color;
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(getOppositeColor(weak), KING));
    int weakPawn   = _bitscanForward(board.getPieces(weak, PAWN));

    int  pRank = _relrank(weakPawn, weak);
    bool pDrawFiles = board.getPieces(weak, PAWN) & (FILE_A | FILE_C | FILE_F | FILE_H);
    s += 8 -  Eval::detail::DISTANCE[weakKing][strongKing];

    // Position is won, unless pawn isnt on a7, c7, f7, h7, with own king nearby
    // TODO -> more accurate eval, including distance of strong king
    // remember that rank mapping is starting from 0
    if (pRank != 6 ||
        !pDrawFiles ||
        Eval::detail::DISTANCE[weakKing][weakPawn] != 1){
            s += MINIMAL_WON_SCORE;
        }

    // in case drawish position, return distance between kings
    return weak != color ? s : -s;
}

// so far rudimentary version, covers only edge pawn
// as it need to properly eval KBP/K etc
int Eval::evaluateKingPawn_vs_King(const Board &board, Color color){
    int s = 0;

    // 1. Glance at Eval to decide who is winning
    s = Eval::evaluateMain(board, color);
    Color weak  = s > 0 ? getOppositeColor(color) : color;
    Color strong = getOppositeColor(weak);

    U64 strongPawns = board.getPieces(strong, PAWN);
    bool allSidePawns = ((strongPawns & (~FILE_A)) != 0) ||
                        ((strongPawns & (~FILE_H)) != 0);

    if (allSidePawns && (detail::PASSED_PAWN_MASKS[strong][_bitscanForward(strongPawns)] & board.getPieces(weak, KING))){
        s = s / 512;
    }

    return s;
}

int Eval::evaluateBishopPawn_vs_KP(const Board &board, Color color){
    int s = 0;

    // 1. Glance at Eval to decide who is winning
    s = Eval::evaluateMain(board, color);
    Color weak  = s > 0 ? getOppositeColor(color) : color;
    Color strong = getOppositeColor(weak);
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    // We do generalistic eval here (including enemy pawns etc)
    // Scale eval down massively in case for corner bishop case
    U64 strongPawns = board.getPieces(strong, PAWN);
    bool allSidePawns = ((strongPawns & (~FILE_A)) != 0) ||
                        ((strongPawns & (~FILE_H)) != 0);

    // if all strong pawns are on one side line, check square of the bishop
    if (allSidePawns){
        int pCol = _col(_bitscanForward(strongPawns));

        U64 queeningSquare = strong == WHITE ? detail::FILES[pCol] & RANK_8 :
                                               detail::FILES[pCol] & RANK_1;

        // if queeningSquare is unreacheable by bishop
        // and weak king is control it, return scale eval to the oblivion
        // TODO: evaluate properly in case of race for a suare
        if ((_popCount((board.getPieces(strong, BISHOP) | queeningSquare) & WHITE_SQUARES) == 1) &&
            Eval::detail::DISTANCE[weakKing][_bitscanForward(queeningSquare)] <= 1){
                s = s / 512;
            }
    }

    // as we only scaling existing eval, no need to reverse sign
    return s;
}

int Eval::evaluateHugeAdvantage(const Board &board, Color color){
    int s = MINIMAL_WON_SCORE;
    return s;
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
    // King vs King + Rook   = win;
    //egHashAdd("kr/K", &evaluateQueen_vs_X);
    //egHashAdd("k/KR", &evaluateQueen_vs_X);
    // King vs King + Queen  = win
    //egHashAdd("kq/K", &evaluateQueen_vs_X);
    //egHashAdd("k/KQ", &evaluateQueen_vs_X);
    // ToDo KPK, so far only edge case
    //egHashAdd("kp/K", &evaluateKingPawn_vs_King);
    //egHashAdd("k/KP", &evaluateKingPawn_vs_King);

    // 4-man eval
    // Obviously KN vs KB etc is draw also
    egHashAdd("kn/KN", &evaluateDraw);
    egHashAdd("kb/KB", &evaluateDraw);
    egHashAdd("kn/KB", &evaluateDraw);
    egHashAdd("kb/KN", &evaluateDraw);
    // R vs R is also a draw
    egHashAdd("kr/KR", &evaluateDraw);
    // KB vs KP -> use special eval (draw unless pawns have advantage)
    //egHashAdd("kb/KP", &evaluateMinor_vs_Pawns);
    //egHashAdd("kp/KB", &evaluateMinor_vs_Pawns);
    // KN vs KP -> use special eval (draw unless pawns have advantage)
    //egHashAdd("kn/KP", &evaluateMinor_vs_Pawns);
    //egHashAdd("kp/KN", &evaluateMinor_vs_Pawns);
    // King vs King + two knights is a draw
    egHashAdd("k/KNN", &evaluateDraw);
    egHashAdd("knn/K", &evaluateDraw);
    // Assume R vs B is always draw
    egHashAdd("kb/KR", &evaluateDraw);
    egHashAdd("kr/KB", &evaluateDraw);
    // Same for R vs N
    egHashAdd("kn/KR", &evaluateDraw);
    egHashAdd("kr/KN", &evaluateDraw);
    // Trivial win for Q vs (R or B or N)
    //egHashAdd("kq/KB", &evaluateQueen_vs_X);
    //egHashAdd("kb/KQ", &evaluateQueen_vs_X);
    //egHashAdd("kq/KN", &evaluateQueen_vs_X);
    //egHashAdd("kn/KQ", &evaluateQueen_vs_X);
    //egHashAdd("kq/KR", &evaluateQueen_vs_X);
    //egHashAdd("kr/KQ", &evaluateQueen_vs_X);
    // Can be a fortress with Q vs P
    //egHashAdd("kq/KP", &evaluateQueen_vs_Pawn);
    //egHashAdd("kp/KQ", &evaluateQueen_vs_Pawn);
    // TODO: R vs Pawn

    // Bishop+Pawn vs King
    //egHashAdd("kbp/K", &evaluateBishopPawn_vs_KP);
    //egHashAdd("k/KBP", &evaluateBishopPawn_vs_KP);


    // 5-man eval
    // lets say
    // King, Rook, Bishop vs King and Rook
    egHashAdd("krb/KR", &evaluateDraw);
    egHashAdd("kr/KRB", &evaluateDraw);
    // King, Rook, Knight vs King and Rook
    egHashAdd("krn/KR", &evaluateDraw);
    egHashAdd("kr/KRN", &evaluateDraw);
    // Minor vs Pawns endgamee
    //egHashAdd("kb/KPP", &evaluateMinor_vs_Pawns);
    //egHashAdd("kpp/KB", &evaluateMinor_vs_Pawns);
    // Minor vs Pawns endgamee
    //egHashAdd("kn/KPP", &evaluateMinor_vs_Pawns);
    //egHashAdd("kpp/KN", &evaluateMinor_vs_Pawns);
    // BishopPawn vs Pawn
    //egHashAdd("kbp/KP", &evaluateBishopPawn_vs_KP);
    //egHashAdd("kp/KBP", &evaluateBishopPawn_vs_KP);
    // BishopPawnPawn vs King
    //egHashAdd("kbpp/K", &evaluateBishopPawn_vs_KP);
    //egHashAdd("k/KBPP", &evaluateBishopPawn_vs_KP);
    // Queen vs Rook Pawn

}