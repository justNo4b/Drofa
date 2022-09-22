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
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
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
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
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

int Eval::evaluateHugeAdvantage(const Board &board, Color color){
    int s = EASY_WIN_SCORE;

    // This function will evaluate huge advantage wins, such as QQ, QR, RR, etc
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(strong, KING));

    // 2. Apply bonuses and penalties
    s += 8 - Eval::detail::DISTANCE[weakKing][strongKing];
    s += 8 - _edgedist(weakKing);

    // 3. Bonuses for extra pieces
    s += _popCount(board.getPieces(strong, QUEEN)) * egS(MATERIAL_VALUES[QUEEN]);
    s += _popCount(board.getPieces(strong, ROOK)) * egS(MATERIAL_VALUES[ROOK]) * 3; // for fun prefer RR >> QR >> QQ
    s += _popCount(board.getPieces(strong, BISHOP)) * egS(MATERIAL_VALUES[BISHOP]);
    s += _popCount(board.getPieces(strong, KNIGHT)) * egS(MATERIAL_VALUES[KNIGHT]);

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;

}

int Eval::evaluateBN_Mating(const Board &board, Color color){
    int s = CONFIDENT_WIN_SCORE;

    // 1. Galnce at PSQT, to see who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(strong, KING));
    int cSq1       = 0;
    int cSq2       = 0;

    // 2. Find which square we should force weak king in
    if (board.getPieces(strong, BISHOP) & WHITE_SQUARES){
        cSq1 = h1;
        cSq2 = a8;
    }else{
        cSq1 = h8;
        cSq2 = a1;
    }

    // 3. Apply bonuses and penalties
    int closeCorner = std::min(Eval::detail::DISTANCE[weakKing][cSq1], Eval::detail::DISTANCE[weakKing][cSq2]);
    s += 8 - Eval::detail::DISTANCE[weakKing][strongKing];
    s += 64 - closeCorner * closeCorner;

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;
}

int Eval::evaluateKnights_vs_Pawn(const Board &board, Color color){
    // Drawish, but mating is possible if pawns isnt too far advanced.
    int s = DRAW_WITH_ADVANTAGE;

    // 1. Galnce at PSQT, to see who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(strong, KING));
    int weakPawn   = _bitscanForward(board.getPieces(weak, PAWN));

    // Evaluate higher when king is on the edge, and lower for each step pawn is closer to promotion
    s += 64 - _edgedist(weakKing) * _edgedist(weakKing);
    s += 8 - Eval::detail::DISTANCE[weakKing][strongKing];
    s -= 49 - _relrank(weakPawn, weak) * _relrank(weakPawn, weak);

    // if sideToMove is Losing, reverse sign
    return weak != color ? s : -s;
}

int Eval::evaluateRook_vs_Bishop(const Board &board, Color color){
    // This endgame is very drawish
    // Only about 18% of positions are win.
    int s = 0;
    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
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
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
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
    Color weak     = egS(psqt) > 0 ? getOppositeColor(color) : color;
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
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
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
    int scale = 1;
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));

    // 1. OCB endgame.
    bool isOCB = _popCount((board.getPieces(color, BISHOP) | board.getPieces(getOppositeColor(color), BISHOP)) & WHITE_SQUARES) == 1;

    if (isOCB) scale = 128;

    // 2. Check if king is in perfect defensive position
    Color weak     = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int strongPawn = _bitscanForward(board.getPieces(strong, PAWN));
    U64 pawnPath   = Eval::detail::FORWARD_BITS[strong][strongPawn];

    // If weak king and strong bishop are on the opposite colors
    // and king is on the pawn path scale eval down
    if ((_popCount((board.getPieces(weak, KING) | board.getPieces(strong, BISHOP)) & WHITE_SQUARES) == 1) &&
        ((pawnPath & board.getPieces(weak, KING)) != 0)){
            scale = 128;
        }

    return scale;
}

int Eval::evaluateBishopPawn_vs_Knight(const Board &board, Color color){
    // This endgame is drawish, if weak side was able to secure
    // with a king square on the pawns path that is inaccessible by bishop
    // Simply unwinnable when OCB
    int scale = 1;

    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));

    // 2. Check if king is in perfect defensive position
    Color weak     = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int strongPawn = _bitscanForward(board.getPieces(strong, PAWN));
    U64 pawnPath   = Eval::detail::FORWARD_BITS[strong][strongPawn];

    // If weak king and strong bishop are on the opposite colors
    // and king is on the pawn path scale eval down
    if ((_popCount((board.getPieces(weak, KING) | board.getPieces(strong, BISHOP)) & WHITE_SQUARES) == 1) &&
        ((pawnPath & board.getPieces(weak, KING)) != 0)){
            scale = 128;
        }

    return scale;
}

int Eval::evaluateQueen_vs_RookPawn(const Board &board, Color color){
    // This endgame is hard to win so we assume basic scale is /2 here
    int scale = 2;

    // Standart fortress position:
    // pawn on 2nd, Rook is defended by pawn, Kind is near a pawn in the back
    // make sure opponent king cant reach backline

    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak     = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong   = getOppositeColor(weak);
    int weakKing   = _bitscanForward(board.getPieces(weak, KING));
    int strongKing = _bitscanForward(board.getPieces(strong, KING));
    int weakPawn   = _bitscanForward(board.getPieces(weak, PAWN));

    if (_relrank(weakPawn, weak) == 1 &&
        _relrank(weakKing, weak) <= 1 &&
        _relrank(strongKing, weak) >= 2 &&
        detail::DISTANCE[weakPawn][weakKing] == 1 &&
        detail::OUTPOST_PROTECTION[strong][weakPawn] & board.getPieces(weak, ROOK)){
            scale = 128;
        }

    return scale;
}

int Eval::evaluateRookPawn_vs_Bishop(const Board &board, Color color){
    // This is generally a win, but there are a few fortress positions
    // They involve far-advanced H or A pawn and bishop of the opposite color
    // of quening square
    int scale = 1;

    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong = getOppositeColor(weak);
    int strongPawn  = _bitscanForward(board.getPieces(strong, PAWN));
    int weakKing    = _bitscanForward(board.getPieces(weak, KING));
    U64 bishAttacks = Attacks::detail::_getBishopAttacks(_bitscanForward(board.getPieces(weak, BISHOP)), 0);
    U64 qSquareBB   = detail::FORWARD_BITS[strong][strongPawn] & PROMOTION_RANK[strong];

    // 1. Fortress is achieved if Pawn is on 6th rank, on A or H files
    // King is in the corner, to stop a pawn and bishop is controlling Pawn path
    if (_relrank(strongPawn, strong) == 5 &&
        (board.getPieces(strong, PAWN) & (FILE_H | FILE_A)) &&
        (detail::FORWARD_BITS[strong][strongPawn] & board.getPieces(weak, KING)) &&
        (detail::FORWARD_BITS[strong][strongPawn] & bishAttacks) &&
        (_popCount((board.getPieces(weak, BISHOP) | qSquareBB) & WHITE_SQUARES) == 1)){
            scale = 128;
        }
    // 2. On the fifth rank with pretty much same conditions (use relaxed condition for a king) there is often a fortress
    // but it is not guaranteed.
    // We scale eval down here, but to be higher than RvsB, to ensure it is actually will try to win this
    if (_relrank(strongPawn, strong) == 4 &&
        (board.getPieces(strong, PAWN) & (FILE_H | FILE_A)) &&
        (detail::DISTANCE[strongPawn][weakKing] <= 3) &&
        (detail::DISTANCE[_bitscanForward(qSquareBB)][weakKing] <= 2) &&
        (_popCount((board.getPieces(weak, BISHOP) | qSquareBB) & WHITE_SQUARES) == 1)){
        scale = 4;
    }

    return scale;
}

int Eval::evaluateRookPawn_vs_Rook(const Board &board, Color color){
    int scale = 1;
    // Very difficult endgames.
    // Here we try to cover a few basic cases

    // 1. Quick glance at PSQT to decide who is winning
    int psqt = board.getPSquareTable().getScore(color) - board.getPSquareTable().getScore(getOppositeColor(color));
    Color weak = egS(psqt) > 0 ? getOppositeColor(color) : color;
    Color strong = getOppositeColor(weak);
    int weakKing    = _bitscanForward(board.getPieces(weak, KING));
    int weakRook    = _bitscanForward(board.getPieces(weak, ROOK));
    int strongPawn  = _bitscanForward(board.getPieces(strong, PAWN));
    U64 qSquareBB   = detail::FORWARD_BITS[strong][strongPawn] & PROMOTION_RANK[strong];

    // 1. Drawish rule -> Philidor defence
    // If pawn is not advanced, place rook at 6th rank and king on pawn path for a draw

    if ((_relrank(strongPawn, strong) < 5) &&
        (_relrank(weakRook, strong) == 5) &&
        (_relrank(weakKing, strong) > 5) &&
        (detail::PASSED_PAWN_MASKS[strong][strongPawn] & board.getPieces(weak, KING))){
            scale = 128;
    }

    // 2. Drawish rule -> checks from the back
    // If our king is covering Q square and pawn on the 6th with enemy king behind own pawn
    // try to check from behind
    if ((_relrank(strongPawn, strong) == 5) &&
        (_relrank(weakRook, strong) <= 1) &&
        (_relrank(weakKing, strong) > 5) &&
        (detail::PASSED_PAWN_MASKS[strong][strongPawn] & board.getPieces(weak, KING))){
            scale = 128;
    }

    // 3. If the weak king is in front of a pawn, and pawn is not a far passer
    // it is a draw very likely
    if ((_relrank(strongPawn, strong) <= 3) &&
        (detail::FORWARD_BITS[strong][strongPawn] & board.getPieces(weak, KING))){
        scale = 8;
    }

    // 4. Depends on side to move for pawn on the 7th
    // If strong side is to move, they win, otherwise draw
    if ((_relrank(strongPawn, strong) == 6) &&
        (_relrank(weakRook, strong) <= 1) &&
        (qSquareBB & board.getPieces(weak, KING)) &&
        (color == weak)){
            scale = 128;
    }

    // 4. When a pawn is on 7th with own rook in front of it,
    // it is a draw when we attack pawn from the back and our king is safe from check_into_Promo
    if((_relrank(strongPawn, strong) == 6) &&
       (detail::FORWARD_BITS[strong][strongPawn] & board.getPieces(strong, ROOK)) &&
       (detail::FORWARD_BITS[weak][strongPawn] & board.getPieces(weak, ROOK)) &&
       (qSquareBB & SIDE_FILES) &&
       (_relrank(weakKing, strong) == 6) &&
       ((FILE_B | FILE_G | SIDE_FILES) & board.getPieces(weak, KING))
       ){
            scale = 128;
    }

    return scale;

}

int Eval::evaluateKingPawn_vs_King(const Board &board, Color color){
    int s = 0;
    // 1. For KPK winning side is the side with a pawn.
    // Galncing at PSQT can be unreliable here
    Color strong = board.getPieces(color, PAWN) > 0 ? color : getOppositeColor(color);
    // Evaluate with the help of a bitbase
    s = Bitbase::eval_by_kpk(board, strong);

    // Score negation is already handled inside bitbase code
    return s;
}

inline void Eval::egHashAdd(std::string psFen, egEvalFunction ef, egEntryType et){
    ZKey key;
    key.setpKeyFromString(psFen);
    U64 index = key.getValue() & (EG_HASH_SIZE - 1);
    if (myEvalHash[index].eFunction != nullptr){
        std::cout << "error collision on " << psFen << std::endl;
        exit(0);
    }
    myEvalHash[index] = egEvalEntry(key.getValue(), ef, et);
}

void Eval::initEG(){

    // initiate table with zero entries
    for (int i = 0; i < EG_HASH_SIZE; i++){
        myEvalHash[i] = egEvalEntry();
    }

    // Add some generic draws
    // 0. 2-man (KvsK) is draw
    egHashAdd("k/K", &evaluateDraw, RETURN_SCORE);

    // 3-man eval:
    // King vs King + Bishop = insufficient material
    egHashAdd("kb/K", &evaluateDraw, RETURN_SCORE);
    egHashAdd("k/KB", &evaluateDraw, RETURN_SCORE);
    // King vs King + Knight = insufficient material
    egHashAdd("kn/K", &evaluateDraw, RETURN_SCORE);
    egHashAdd("k/KN", &evaluateDraw, RETURN_SCORE);
    // King vs King + Rook   = win;
    egHashAdd("kr/K", &evaluateQueen_vs_X, RETURN_SCORE);
    egHashAdd("k/KR", &evaluateQueen_vs_X, RETURN_SCORE);
    // King vs King + Queen  = win
    egHashAdd("kq/K", &evaluateQueen_vs_X, RETURN_SCORE);
    egHashAdd("k/KQ", &evaluateQueen_vs_X, RETURN_SCORE);
    // KPK is evaluated by bitbase (basically Stash code for Bitbase)
    egHashAdd("kp/K", &evaluateKingPawn_vs_King, RETURN_SCORE);
    egHashAdd("k/KP", &evaluateKingPawn_vs_King, RETURN_SCORE);

    // 4-man eval
    // Obviously KN vs KB etc is draw also
    egHashAdd("kn/KN", &evaluateDraw, RETURN_SCORE);
    egHashAdd("kb/KB", &evaluateDraw, RETURN_SCORE);
    egHashAdd("kn/KB", &evaluateDraw, RETURN_SCORE);
    egHashAdd("kb/KN", &evaluateDraw, RETURN_SCORE);
    // R vs R and Q vs Q is also a draw
    egHashAdd("kr/KR", &evaluateDraw, RETURN_SCORE);
    egHashAdd("kq/KQ", &evaluateDraw, RETURN_SCORE);
    // King vs King + two knights is a draw
    egHashAdd("k/KNN", &evaluateDraw, RETURN_SCORE);
    egHashAdd("knn/K", &evaluateDraw, RETURN_SCORE);
    // Trivial win for Q vs (R or B or N)
    egHashAdd("kq/KB", &evaluateQueen_vs_X, RETURN_SCORE);
    egHashAdd("kb/KQ", &evaluateQueen_vs_X, RETURN_SCORE);
    egHashAdd("kq/KN", &evaluateQueen_vs_X, RETURN_SCORE);
    egHashAdd("kn/KQ", &evaluateQueen_vs_X, RETURN_SCORE);
    egHashAdd("kq/KR", &evaluateQueen_vs_X, RETURN_SCORE);
    egHashAdd("kr/KQ", &evaluateQueen_vs_X, RETURN_SCORE);
    // Not so clear with Q vs P
    egHashAdd("kq/KP", &evaluateQueen_vs_Pawn, RETURN_SCORE);
    egHashAdd("kp/KQ", &evaluateQueen_vs_Pawn, RETURN_SCORE);
    // Rook vs Minors
    egHashAdd("kr/KN", &evaluateRook_vs_Knight, RETURN_SCORE);
    egHashAdd("kn/KR", &evaluateRook_vs_Knight, RETURN_SCORE);
    egHashAdd("kr/KB", &evaluateRook_vs_Bishop, RETURN_SCORE);
    egHashAdd("kb/KR", &evaluateRook_vs_Bishop, RETURN_SCORE);
    // Rook vs Pawns
    egHashAdd("kr/KP", &evaluateRook_vs_Pawn, RETURN_SCORE);
    egHashAdd("kp/KR", &evaluateRook_vs_Pawn, RETURN_SCORE);
    // ToDo - > lone minor vs pawns scaling (in main eval)

    //Some easy wins with huge advantage
    egHashAdd("kqq/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KQQ", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("kqr/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KQR", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("kqb/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KQB", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("kqn/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KQN", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("kqp/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KQP", &evaluateHugeAdvantage, RETURN_SCORE);

    egHashAdd("krr/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KRR", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("krb/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KRB", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("krn/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KRN", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("krp/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KRP", &evaluateHugeAdvantage, RETURN_SCORE);

    egHashAdd("kbb/K", &evaluateHugeAdvantage, RETURN_SCORE);
    egHashAdd("k/KBB", &evaluateHugeAdvantage, RETURN_SCORE);

    egHashAdd("kbn/K", &evaluateBN_Mating, RETURN_SCORE);
    egHashAdd("k/KBN", &evaluateBN_Mating, RETURN_SCORE);

    // 5-man eval
    // lets say
    // King, Rook, Bishop vs King and Rook
    egHashAdd("krb/KR", &evaluateRookMinor_Rook, RETURN_SCORE);
    egHashAdd("kr/KRB", &evaluateRookMinor_Rook, RETURN_SCORE);
    // King, Rook, Knight vs King and Rook
    egHashAdd("krn/KR", &evaluateRookMinor_Rook, RETURN_SCORE);
    egHashAdd("kr/KRN", &evaluateRookMinor_Rook, RETURN_SCORE);
    // Obvious minors draws
    egHashAdd("knn/KN", &evaluateDraw, RETURN_SCORE);
    egHashAdd("kn/KNN", &evaluateDraw, RETURN_SCORE);
    egHashAdd("kbb/KB", &evaluateDraw, RETURN_SCORE);
    egHashAdd("kb/KBB", &evaluateDraw, RETURN_SCORE);
    // Common endgames
    egHashAdd("kbp/KB", &evaluateBishopPawn_vs_Bishop, RETURN_SCALE);
    egHashAdd("kb/KBP", &evaluateBishopPawn_vs_Bishop, RETURN_SCALE);
    egHashAdd("kbp/KN", &evaluateBishopPawn_vs_Knight, RETURN_SCALE);
    egHashAdd("kn/KBP", &evaluateBishopPawn_vs_Knight, RETURN_SCALE);
    // Evaluate Q vs RP fortress
    egHashAdd("krp/KQ", &evaluateQueen_vs_RookPawn, RETURN_SCALE);
    egHashAdd("kq/KRP", &evaluateQueen_vs_RookPawn, RETURN_SCALE);
    // Evaluate RP vs B fortress
    egHashAdd("krp/KB", &evaluateRookPawn_vs_Bishop, RETURN_SCALE);
    egHashAdd("kb/KRP", &evaluateRookPawn_vs_Bishop, RETURN_SCALE);
    // Evaluate NN vs P winning chances
    //egHashAdd("knn/KP", &evaluateKnights_vs_Pawn, RETURN_SCORE);
    //egHashAdd("kp/KNN", &evaluateKnights_vs_Pawn, RETURN_SCORE);
    // Evaluate RP vs R endgames
    egHashAdd("krp/KR", &evaluateRookPawn_vs_Rook, RETURN_SCALE);
    egHashAdd("kr/KRP", &evaluateRookPawn_vs_Rook, RETURN_SCALE);
}