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
#include "defs.h"
#include "endgame.h"
#include "eval.h"
#include "bitutils.h"
#include "attacks.h"
#include <cstring>

uint8_t KPK_Bitbase[KPK_SIZE / 8];


// Bitbase code is basically copy-paste (with some minor semantic changes) of the Stash code
// see https://github.com/mhouppin/stash-bot

inline unsigned int Bitbase::kpk_get_index(Color color, int bKing, int wKing, int pawn){
    return ((unsigned int)wKing | ((unsigned int)bKing << 6) | ((unsigned int)color << 12)
         | ((unsigned int)_col(pawn) << 13) | ((unsigned int)(6 - _row(pawn)) << 15));
}

bool Bitbase::kpk_is_winning(Color color, int bKing, int wKing, int pawn){
    unsigned int index = kpk_get_index (color, bKing, wKing, pawn);
    return (KPK_Bitbase[index >> 3] & (1 << (index & 7)));
}

void Bitbase::kpk_set(kpk_position *bPosition, unsigned int index){
    const int wKing     = (int) (index & 0x3F);
    const int bKing     = (int) ((index >> 6) & 0x3F);
    const Color color   = (Color) ((index >> 12) & 1);
    const int pawn      = ((index >> 13) & 0x3) +  ((6 - ((index >> 15) & 0x7)) << 3);

    bPosition->stm = color;
    bPosition->kingSq[WHITE] = wKing;
    bPosition->kingSq[BLACK] = bKing;
    bPosition->pawnSq = pawn;

    U64 wKingAttacks = Attacks::getNonSlidingAttacks(KING, wKing);
    U64 bKingAttacks = Attacks::getNonSlidingAttacks(KING, bKing);
    U64 pawnAttacks  = Eval::detail::OUTPOST_PROTECTION[BLACK][pawn];
    int qSquare      = pawn + 8;

    // Kings are overlapping or close -> invalid
    if (Eval::detail::DISTANCE[wKing][bKing] <= 1){
        bPosition->result = KPK_INVALID;
    // King is overlapping with pawn -> invalid
    }else if (wKing == pawn || bKing == pawn){
        bPosition->result = KPK_INVALID;
    // Weak king in check and strong stm -> invalid
    }else if (color == WHITE && (pawnAttacks & _sqBB(bKing))){
        bPosition->result = KPK_INVALID;
    // Can we promote
    }else if (color == WHITE && _relrank(pawn, WHITE) == 6 &&
              wKing != qSquare && bKing != qSquare &&
              (Eval::detail::DISTANCE[qSquare][bKing] > 1 || Eval::detail::DISTANCE[qSquare][wKing] == 1)){
        bPosition->result = KPK_WIN;
    // Is it a stalemate
    }else if (color == BLACK && !(bKingAttacks & ~(wKingAttacks | pawnAttacks))){
        bPosition->result = KPK_DRAW;
    // Can losing side win a pawn
    }else if (color == BLACK && (bKingAttacks & ~ wKingAttacks & _sqBB(pawn))){
        bPosition->result = KPK_DRAW;
    }else{
        bPosition->result = KPK_UNKNOWN;
    }

}

void Bitbase::kpk_classify( kpk_position *position, kpk_position *table){

    const uint8_t goodResult = (position->stm == WHITE) ? KPK_WIN : KPK_DRAW;
    const uint8_t badResult = (position->stm == WHITE) ? KPK_DRAW : KPK_WIN;

    const int wksq = position->kingSq[WHITE];
    const int bksq = position->kingSq[BLACK];
    const Color color = position->stm;
    const int psq = position->pawnSq;

    uint8_t result = KPK_INVALID;
    U64 b = Attacks::getNonSlidingAttacks(KING, position->kingSq[color]);

    // Get all entries for king moves
    while (b)
    {
        if (color == WHITE)
            result |= table[kpk_get_index(BLACK, bksq, _popLsb(b), psq)].result;
        else
            result |= table[kpk_get_index(WHITE, _popLsb(b), wksq, psq)].result;
    }


    // if winning side is stm, get also entries for pawn moves

        if (color == WHITE)
    {
        // Single push

        if (_relrank(psq, WHITE) < 6)
            result |= table[kpk_get_index(BLACK, bksq, wksq, psq + 8)].result;

        // Double push

        if (_relrank(psq, WHITE) == 1 && psq + 8 != wksq && psq + 8 != bksq)
            result |= table[kpk_get_index(BLACK, bksq, wksq, psq + 8 + 8)].result;
    }

    position->result = (result & goodResult    ? goodResult
                      : result & KPK_UNKNOWN ? KPK_UNKNOWN
                                             : badResult);

}


void Bitbase::init_kpk(){
    kpk_position *kpkTable = (kpk_position *) malloc(sizeof(kpk_position) * KPK_SIZE);

    if (kpkTable == NULL)
    {
        std::cout << " Failure to initialize KPK database" << std::endl;
        exit(0);
    }

    unsigned int index;
    bool repeat;

    std::memset(KPK_Bitbase, 0, sizeof(KPK_Bitbase));
    for (index = 0; index < KPK_SIZE; ++index) kpk_set(kpkTable + index, index);

    do {
        repeat = false;
        for (index = 0; index < KPK_SIZE; ++index)
            if (kpkTable[index].result == KPK_UNKNOWN)
            {
                kpk_classify(kpkTable + index, kpkTable);
                repeat |= kpkTable[index].result != KPK_UNKNOWN;
            }
    } while (repeat);


    for (index = 0; index < KPK_SIZE; ++index)
        if (kpkTable[index].result == KPK_WIN) KPK_Bitbase[index / 8] |= 1 << (index % 8);

    free(kpkTable);
}


int Bitbase::eval_by_kpk(const Board &board, Color winningSide){
    int winningKing = _bitscanForward(board.getPieces(winningSide, KING));
    int winningPawn = _bitscanForward(board.getPieces(winningSide, PAWN));
    int losingKing  = _bitscanForward(board.getPieces(getOppositeColor(winningSide), KING));
    Color us = winningSide == board.getActivePlayer() ? WHITE : BLACK;

    winningKing = normalize_square(board, winningSide, winningKing);
    winningPawn = normalize_square(board, winningSide, winningPawn);
    losingKing = normalize_square(board, winningSide, losingKing);

    int score = kpk_is_winning(us, losingKing, winningKing, winningPawn)
                        ? CONFIDENT_WIN_SCORE + _relrank(_bitscanForward(board.getPieces(winningSide, PAWN)), winningSide) * 3
                        : 0;

    return (winningSide == board.getActivePlayer() ? score : -score);
}