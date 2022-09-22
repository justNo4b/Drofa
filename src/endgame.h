#ifndef ENDGAME_H
#define ENDGAME_H

#include "defs.h"
#include "board.h"
#include "bitutils.h"


#define EG_HASH_SIZE        (8192)
#define LARGEST_SP_EVAL     (5)
#define KPK_SIZE            (2 * 24 * 64 * 64)
#define KPK_INVALID         (0)
#define KPK_UNKNOWN         (1)
#define KPK_DRAW            (2)
#define KPK_WIN             (4)


typedef int (*egEvalFunction) (const Board &board, Color color);

enum egEntryType{
    RETURN_SCORE,
    RETURN_SCALE
};

struct egEvalEntry{
    U64            key;
    egEvalFunction eFunction;
    egEntryType    evalType;

    egEvalEntry(): key(0), eFunction(nullptr), evalType(RETURN_SCALE)  {};
    egEvalEntry(U64 k, egEvalFunction ef, egEntryType et): key(k), eFunction(ef), evalType(et) {};
};

struct kpk_position{
    Color   stm;
    int     kingSq[2];
    int     pawnSq;
    uint8_t result;
};

// constant scores


const int MIN_MATE_SCORE         = 29000;
const int MINIMAL_WON_SCORE      =  7500;
const int EASY_WIN_SCORE         =  9500;
const int CONFIDENT_WIN_SCORE    =  8500;

const int DRAW_ZONE_ABS          = 20;
const int DRAW_WITH_ADVANTAGE    = 10;


inline int normalize_square(const Board &board, Color winning, int sqv){

 if (_col(_bitscanForward(board.getPieces(winning, PAWN))) >= 4){
    sqv ^= 7;
 }

 return (sqv ^ (a8 * winning));
}

namespace Bitbase{


inline uint kpk_get_index(Color, int, int, int);

bool kpk_is_winning(Color, int, int, int);


void kpk_set(kpk_position*, uint);
void kpk_classify(kpk_position *, kpk_position *);
void init_kpk();

int eval_by_kpk(const Board &, Color);
}


#endif