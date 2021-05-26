#ifndef TUNINGFEATURES_H
#define TUNINGFEATURES_H

#include "defs.h"
#include "eval.h"
#include "outposts.h"

#define BIG_FEATURE_NUMBER (48)

enum TuningType{
    ALL,
    OP_ONLY,
    EG_ONLY
};

struct tFeature {
    std::string     name;
    bool            isArray;
    const int      *startValue;
    int             valuesTotal;
    int             padding;
    TuningType      type;

    tFeature () : name("0"), isArray(false), startValue(nullptr),  valuesTotal(1), padding(1), type(ALL) {};
    tFeature (std::string n, bool b, const int * s, int v, int p, TuningType t) : 
                  name(n), isArray(b), startValue(s), valuesTotal(v), padding(p), type(t) {};

};

tFeature myFeatures [BIG_FEATURE_NUMBER] = {
    tFeature("PawnValue", false, &Eval::MATERIAL_VALUES[PAWN], 1, 1, ALL),
    tFeature("RookValue", false, &Eval::MATERIAL_VALUES[ROOK], 1, 1, ALL),
    tFeature("KnightValue", false, &Eval::MATERIAL_VALUES[KNIGHT], 1, 1, ALL),
    tFeature("BishopValue", false, &Eval::MATERIAL_VALUES[BISHOP], 1, 1, ALL),
    tFeature("QueenValue", false, &Eval::MATERIAL_VALUES[QUEEN], 1, 1, ALL),
    tFeature("KING_HIGH_DANGER", false, &Eval::KING_HIGH_DANGER, 1, 1, ALL),
    tFeature("KING_MED_DANGER", false, &Eval::KING_MED_DANGER, 1, 1, ALL),
    tFeature("KING_LOW_DANGER", false, &Eval::KING_LOW_DANGER, 1, 1, ALL),
    tFeature("KING_SAFE", false, &Eval::KING_SAFE, 1, 1, ALL),
    tFeature("BISHOP_PAIR_BONUS", false, &Eval::BISHOP_PAIR_BONUS, 1, 1, ALL),
    tFeature("PAWN_SUPPORTED", false, &Eval::PAWN_SUPPORTED, 1, 1, ALL),
    tFeature("DOUBLED_PAWN_PENALTY", false, &Eval::DOUBLED_PAWN_PENALTY, 1, 1, ALL),
    tFeature("ISOLATED_PAWN_PENALTY", false, &Eval::ISOLATED_PAWN_PENALTY, 1, 1, ALL),
    tFeature("PAWN_BLOCKED", false, &Eval::PAWN_BLOCKED, 1, 1, ALL),
    tFeature("PASSER_BLOCKED", false, &Eval::PASSER_BLOCKED, 1, 1, ALL),
    tFeature("PAWN_DISTORTION", false, &Eval::PAWN_DISTORTION, 1, 1, ALL),
    tFeature("BISHOP_RAMMED_PENALTY", false, &Eval::BISHOP_RAMMED_PENALTY, 1, 1, ALL),
    tFeature("BISHOP_CENTER_CONTROL", false, &Eval::BISHOP_CENTER_CONTROL, 1, 1, ALL),
    tFeature("MINOR_BEHIND_PAWN", false, &Eval::MINOR_BEHIND_PASSER, 1, 1, ALL),
    tFeature("MINOR_BEHIND_PASSER", false, &Eval::MINOR_BEHIND_PASSER, 1, 1, ALL),
    tFeature("KING_AHEAD_PASSER", false, &Eval::KING_AHEAD_PASSER, 1, 1, ALL),
    tFeature("KING_EQUAL_PASSER", false, &Eval::KING_EQUAL_PASSER, 1, 1, ALL),
    tFeature("KING_BEHIND_PASSER", false, &Eval::KING_BEHIND_PASSER, 1, 1, ALL),
    tFeature("KING_OPEN_FILE", false, &Eval::KING_OPEN_FILE, 1, 1, ALL),
    tFeature("KING_OWN_SEMI_FILE", false, &Eval::KING_OWN_SEMI_FILE, 1, 1, ALL),
    tFeature("KING_ENEMY_SEMI_LINE", false, &Eval::KING_ENEMY_SEMI_LINE, 1, 1, ALL),
    tFeature("PASSED_PAWN_RANKS", true, Eval::PASSED_PAWN_RANKS, 8, 4, ALL),
    tFeature("PASSED_PAWN_FILES", true, Eval::PASSED_PAWN_FILES, 8, 4, ALL),
    tFeature("KING_PASSER_DISTANCE_FRIENDLY", true, Eval::KING_PASSER_DISTANCE_FRIENDLY, 9, 4, ALL),
    tFeature("KING_PASSER_DISTANCE_ENEMY", true, Eval::KING_PASSER_DISTANCE_ENEMY, 9, 4, ALL),
    tFeature("ROOK_OPEN_FILE_BONUS", true, Eval::ROOK_OPEN_FILE_BONUS, 2, 10, ALL),
    tFeature("ROOK_SEMI_FILE_BONUS", true, Eval::ROOK_SEMI_FILE_BONUS, 2, 10, ALL),
    tFeature("HANGING_PIECE", true, Eval::HANGING_PIECE, 5, 10, ALL),
    tFeature("BISHOP_MOBILITY", true, Eval::BISHOP_MOBILITY, 14, 7, ALL),
    tFeature("KNIGHT_MOBILITY", true, Eval::KNIGHT_MOBILITY, 9, 5, ALL),
    tFeature("KING_MOBILITY", true, Eval::KING_MOBILITY, 9, 5, ALL),
    tFeature("ROOK_MOBILITY", true, Eval::ROOK_MOBILITY, 15, 7, ALL),
    tFeature("QUEEN_MOBILITY", true, Eval::QUEEN_MOBILITY, 28, 7, ALL),
    tFeature("KING_PSQT_BLACK", true, Eval::KING_PSQT_BLACK, 64, 8, ALL),
    tFeature("PAWN_PSQT_BLACK", true, Eval::PAWN_PSQT_BLACK, 64, 8, ALL),
    tFeature("ROOK_PSQT_BLACK", true, Eval::ROOK_PSQT_BLACK, 64, 8, ALL),
    tFeature("BISHOP_PSQT_BLACK", true, Eval::BISHOP_PSQT_BLACK, 64, 8, ALL),
    tFeature("KNIGHT_PSQT_BLACK", true, Eval::KNIGHT_PSQT_BLACK, 64, 8, ALL),
    tFeature("QUEEN_PSQT_BLACK", true, Eval::QUEEN_PSQT_BLACK, 64, 8, ALL),
    tFeature("KNIGHT_PROT_OUTPOST_BLACK", true, KNIGHT_PROT_OUTPOST_BLACK, 64, 8, ALL),
    tFeature("BISHOP_PROT_OUTPOST_BLACK", true, BISHOP_PROT_OUTPOST_BLACK, 64, 8, ALL),
    tFeature("KNIGHT_OUTPOST_BLACK", true, KNIGHT_OUTPOST_BLACK, 64, 8, ALL),
    tFeature("BISHOP_OUTPOST_BLACK", true, BISHOP_OUTPOST_BLACK, 64, 8, ALL)
};


#endif