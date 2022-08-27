#include "defs.h"
#include "eval.h"
#include "attacks.h"
#include "movegen.h"
#include "endgame.h"
#include "eval.h"

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
    s += 8 - _edgedist(weakKing);

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;

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
    s += 8 - _edgedist(weakKing);

    // 3. LoneKingBonus - add some eval if weak side has no pieces
    // Otherwise it wont pick up material
    s += 100 * (_popCount(board.getAllPieces(weak)) == 1);

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;
}

int Eval::evaluateRook_vs_Bishop(const Board &board, Color color){
    // This endgame is very drawish
    // Only about 18% of positions are win.
    int s = 0;
    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = psqt > 0 ? getOppositeColor(color) : color;
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(getOppositeColor(weak), KING));

    // 1. Punish positions where king is on the edge
    s += 21 - 7 * _edgedist(weakKing);
    // 2. Small bonus for our king being close to the enemy
    s += 7 - Eval::detail::DISTANCE[weakKing][strongKing];
    // 2. If king is on the edge, do additional penalty if king is close to the corner same color as bishop
    if (WHITE_SQUARES & board.getPieces(weak, BISHOP)){
        s += 10 * (Eval::detail::DISTANCE[weakKing][a8] <= 1);
        s += 10 * (Eval::detail::DISTANCE[weakKing][h1] <= 1);
    }else{
        s += 10 * (Eval::detail::DISTANCE[weakKing][a1] <= 1);
        s += 10 * (Eval::detail::DISTANCE[weakKing][h8] <= 1);
    }

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;
}

int Eval::evaluateRook_vs_Knight(const Board &board, Color color){
    // This endgame is somewhat winnable if king and knight are far apart
    // About 30% of positions are won
    int s = DRAW_WITH_ADVANTAGE;

    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = psqt > 0 ? getOppositeColor(color) : color;
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int weakKnight = _bitscanForward(board.getPieces(weak, KNIGHT));

    // 2. There are two basic patterns in a win - we want weak king to be in a side
    // And king and knight far apart (even better if knight is on the edge also)
    s += 12 - 4 * _edgedist(weakKing);
    int kn_distance = Eval::detail::DISTANCE[weakKing][weakKnight];
    s += (kn_distance - 1) * 6;
    if (kn_distance >= 3 && _edgedist(weakKnight) <= 1) s += 6;

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;
}

int Eval::evaluateRook_vs_Pawn(const Board &board, Color color){
    // Quite tricky endgame to evaluate presicely
    // Draw + weakside win is about 20% here
    int s = 0;

    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak     = psqt > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int weakPawn   = _bitscanForward(board.getPieces(weak, PAWN));
    int strongKing = _bitscanForward(board.getPieces(strong, KING));
    int strongRook = _bitscanForward(board.getPieces(strong, ROOK));

    // 2. If strong king is on the path of the pawn, it is always a win.
    if (Eval::detail::PASSED_PAWN_MASKS[weak][weakPawn] & board.getPieces(strong, KING)){
        s = MINIMAL_WON_SCORE;
        // adjust it with king - pawn distance to ensure pawn will get captured
        s += 8 - Eval::detail::DISTANCE[weakPawn][strongKing];
    }// 3. Position is also won when enemy king is far from own pawn and our rook (so it cant win a tempo)
    else if ((Eval::detail::DISTANCE[weakPawn][weakKing] >= 3 + (weak == color)) &&
              Eval::detail::DISTANCE[strongRook][weakKing] >= 3){
        s = MINIMAL_WON_SCORE;
        // adjust it with king - pawn distance to ensure pawn will get captured
        s += 8 - Eval::detail::DISTANCE[weakPawn][strongKing];
    } // 4. Draw case: strong king is far away AND behind, pawn is advanced,
      // weak king is supporting it
    else if (Eval::detail::DISTANCE[weakPawn][weakKing] == 1 &&
             (Eval::detail::DISTANCE[weakPawn][strongKing] > 2 + (strong == color)) &&
             _relrank(weakPawn, weak) > 3 &&
             _relrank(strongKing, weak) < _relrank(weakPawn, weak)){

        s = 20 - Eval::detail::DISTANCE[weakPawn][strongKing];
    } // result is unclear, but likely to be drawish
    else {
        s = 100 - 8 * (Eval::detail::DISTANCE[strongKing][weakPawn] - // 1
                       Eval::detail::DISTANCE[weakKing][weakPawn] -   // 7
                       (8 - _relrank(weakPawn, weak)));
    }


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

int Eval::evaluateBishopPawn_vs_Bishop(const Board &board, Color color){
    // This endgame is drawish, if weak side was able to secure
    // with a king square on the pawns path that is inaccessible by bishop
    // Simply unwinnable when OCB
    int s = evaluateMain(board, color);

    // 1. OCB endgame.
    bool isOCB = _popCount((board.getPieces(color, BISHOP) | board.getPieces(getOppositeColor(color), BISHOP)) & WHITE_SQUARES) == 1;

    if (isOCB) s = s / 128;

    // 2. Check if king is in perfect defensive position
    Color weak     = s > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int strongPawn = _bitscanForward(board.getPieces(strong, PAWN));
    U64 pawnPath   = Eval::detail::FORWARD_BITS[strong][strongPawn];

    // If weak king and strong bishop are on the opposite colors
    // and king is on the pawn path scale eval down
    if ((_popCount((board.getPieces(weak, KING) | board.getPieces(strong, BISHOP)) & WHITE_SQUARES) == 1) &&
        ((pawnPath & board.getPieces(weak, KING)) != 0)){
            s =  s / 128;
        }

    return s;
}

int Eval::evaluateBishopPawn_vs_Knight(const Board &board, Color color){
    // This endgame is drawish, if weak side was able to secure
    // with a king square on the pawns path that is inaccessible by bishop
    // Simply unwinnable when OCB
    int s = evaluateMain(board, color);

    // 2. Check if king is in perfect defensive position
    Color weak     = s > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int strongPawn = _bitscanForward(board.getPieces(strong, PAWN));
    U64 pawnPath   = Eval::detail::FORWARD_BITS[strong][strongPawn];

    // If weak king and strong bishop are on the opposite colors
    // and king is on the pawn path scale eval down
    if ((_popCount((board.getPieces(weak, KING) | board.getPieces(strong, BISHOP)) & WHITE_SQUARES) == 1) &&
        ((pawnPath & board.getPieces(weak, KING)) != 0)){
            s = s / 128;
        }

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
    // R vs R and Q vs Q is also a draw
    egHashAdd("kr/KR", &evaluateDraw);
    egHashAdd("kq/KQ", &evaluateDraw);
    // King vs King + two knights is a draw
    egHashAdd("k/KNN", &evaluateDraw);
    egHashAdd("knn/K", &evaluateDraw);
    // Trivial win for Q vs (R or B or N)
    egHashAdd("kq/KB", &evaluateQueen_vs_X);
    egHashAdd("kb/KQ", &evaluateQueen_vs_X);
    egHashAdd("kq/KN", &evaluateQueen_vs_X);
    egHashAdd("kn/KQ", &evaluateQueen_vs_X);
    egHashAdd("kq/KR", &evaluateQueen_vs_X);
    egHashAdd("kr/KQ", &evaluateQueen_vs_X);
    // Not so clear with Q vs P
    egHashAdd("kq/KP", &evaluateQueen_vs_Pawn);
    egHashAdd("kp/KQ", &evaluateQueen_vs_Pawn);
    // Rook vs Minors
    egHashAdd("kr/KN", &evaluateRook_vs_Knight);
    egHashAdd("kn/KR", &evaluateRook_vs_Knight);
    egHashAdd("kr/KB", &evaluateRook_vs_Bishop);
    egHashAdd("kb/KR", &evaluateRook_vs_Bishop);
    // Rook vs Pawns
    egHashAdd("kr/KP", &evaluateRook_vs_Pawn);
    egHashAdd("kp/KR", &evaluateRook_vs_Pawn);

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
    // Common endgames
    egHashAdd("kbp/KB", &evaluateBishopPawn_vs_Bishop);
    egHashAdd("kb/KBP", &evaluateBishopPawn_vs_Bishop);
    egHashAdd("kbp/KN", &evaluateBishopPawn_vs_Knight);
    egHashAdd("kn/KBP", &evaluateBishopPawn_vs_Knight);
}