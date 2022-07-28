#ifndef TUNINGFEATURES_H
#define TUNINGFEATURES_H

#include "defs.h"
#include "eval.h"
#include "outposts.h"

#define BIG_FEATURE_NUMBER (71)

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
    tFeature("KING_HIGH_DANGER", false, &Eval::KING_HIGH_DANGER, 1, 1, ALL),
    tFeature("KING_MED_DANGER", false, &Eval::KING_MED_DANGER, 1, 1, ALL),
    tFeature("KING_LOW_DANGER", false, &Eval::KING_LOW_DANGER, 1, 1, ALL),
    tFeature("BISHOP_PAIR_BONUS", false, &Eval::BISHOP_PAIR_BONUS, 1, 1, ALL),
    tFeature("DOUBLED_PAWN_PENALTY", false, &Eval::DOUBLED_PAWN_PENALTY, 1, 1, ALL),
    tFeature("ISOLATED_PAWN_PENALTY", false, &Eval::ISOLATED_PAWN_PENALTY, 1, 1, ALL),
    tFeature("PAWN_BLOCKED", false, &Eval::PAWN_BLOCKED, 1, 1, ALL),
    tFeature("PASSER_BLOCKED", false, &Eval::PASSER_BLOCKED, 1, 1, ALL),
    tFeature("PAWN_PUSH_THREAT", false, &Eval::PAWN_PUSH_THREAT, 1, 1, ALL),
    tFeature("BISHOP_RAMMED_PENALTY", false, &Eval::BISHOP_RAMMED_PENALTY, 1, 1, ALL),
    tFeature("BISHOP_CENTER_CONTROL", false, &Eval::BISHOP_CENTER_CONTROL, 1, 1, ALL),
    tFeature("BISHOP_POS_PROUTPOST_JUMP", false, &Eval::BISHOP_POS_PROUTPOST_JUMP, 1, 1, ALL),
    tFeature("BISHOP_POS_GENOUTPOST_JUMP", false, &Eval::BISHOP_POS_GENOUTPOST_JUMP, 1, 1, ALL),
    tFeature("KNIGHT_POS_PROUTPOST_JUMP", false, &Eval::KNIGHT_POS_PROUTPOST_JUMP, 1, 1, ALL),
    tFeature("KNIGHT_POS_GENOUTPOST_JUMP", false, &Eval::KNIGHT_POS_GENOUTPOST_JUMP, 1, 1, ALL),
    tFeature("MINOR_BEHIND_PAWN", false, &Eval::MINOR_BEHIND_PAWN, 1, 1, ALL),
    tFeature("MINOR_BEHIND_PASSER", false, &Eval::MINOR_BEHIND_PASSER, 1, 1, ALL),
    tFeature("MINOR_BLOCK_OWN_PAWN", false, &Eval::MINOR_BLOCK_OWN_PAWN, 1, 1, ALL),
    tFeature("MINOR_BLOCK_OWN_PASSER", false, &Eval::MINOR_BLOCK_OWN_PASSER, 1, 1, ALL),
    tFeature("ROOK_LINE_TENSION", false, &Eval::ROOK_LINE_TENSION, 1, 1, ALL),
    tFeature("ROOK_RAMMED_LINE", false, &Eval::ROOK_RAMMED_LINE, 1, 1, ALL),
    tFeature("KING_AHEAD_PASSER", false, &Eval::KING_AHEAD_PASSER, 1, 1, ALL),
    tFeature("KING_EQUAL_PASSER", false, &Eval::KING_EQUAL_PASSER, 1, 1, ALL),
    tFeature("KING_BEHIND_PASSER", false, &Eval::KING_BEHIND_PASSER, 1, 1, ALL),
    tFeature("KING_OPEN_FILE", false, &Eval::KING_OPEN_FILE, 1, 1, ALL),
    tFeature("KING_OWN_SEMI_FILE", false, &Eval::KING_OWN_SEMI_FILE, 1, 1, ALL),
    tFeature("KING_ENEMY_SEMI_LINE", false, &Eval::KING_ENEMY_SEMI_LINE, 1, 1, ALL),
    tFeature("KING_ATTACK_PAWN", false, &Eval::KING_ATTACK_PAWN, 1, 1, ALL),
    tFeature("KING_PAWNLESS_FLANG", false, &Eval::KING_PAWNLESS_FLANG, 1, 1, ALL),
    tFeature("PASSED_PAWN_RANKS", true, Eval::PASSED_PAWN_RANKS, 7, 4, ALL),
    tFeature("PASSED_PAWN_FILES", true, Eval::PASSED_PAWN_FILES, 4, 10, ALL),
    tFeature("PASSED_PAWN_FREE", true, Eval::PASSED_PAWN_FREE, 7, 4, ALL),
    tFeature("PASSED_PAWN_POS_ADVANCE", true, Eval::PASSED_PAWN_POS_ADVANCE, 7, 4, ALL),
    tFeature("PASSED_PASSED_DISTANCE", true, Eval::PASSED_PASSED_DISTANCE, 8, 4, ALL),
    tFeature("KING_PASSER_DISTANCE_FRIENDLY", true, Eval::KING_PASSER_DISTANCE_FRIENDLY, 8, 4, ALL),
    tFeature("KING_PASSER_DISTANCE_ENEMY", true, Eval::KING_PASSER_DISTANCE_ENEMY, 8, 4, ALL),
    tFeature("KNIGHT_PASSER_DISTANCE_ENEMY", true, Eval::KNIGHT_PASSER_DISTANCE_ENEMY, 4, 10, ALL),
    tFeature("CANDIDATE_PASSED_PAWN", true, Eval::CANDIDATE_PASSED_PAWN, 7, 4, ALL),
    tFeature("CANDIDATE_PASSED_PAWN_FILES", true, Eval::CANDIDATE_PASSED_PAWN_FILES, 4, 10, ALL),
    tFeature("BACKWARD_PAWN", true, Eval::BACKWARD_PAWN, 7, 4, ALL),
    tFeature("BACKWARD_OPEN_PAWN", true, Eval::BACKWARD_OPEN_PAWN, 7, 4, ALL),
    tFeature("PAWN_STORM_UNBLOCKED", true, Eval::PAWN_STORM_UNBLOCKED, 7, 4, ALL),
    tFeature("PAWN_STORM_BLOCKED", true, Eval::PAWN_STORM_BLOCKED, 7, 4, ALL),
    tFeature("ROOK_OPEN_FILE_BONUS", true, Eval::ROOK_OPEN_FILE_BONUS, 2, 10, ALL),
    tFeature("ROOK_SEMI_FILE_BONUS", true, Eval::ROOK_SEMI_FILE_BONUS, 2, 10, ALL),
    tFeature("HANGING_PIECE", true, Eval::HANGING_PIECE, 5, 10, ALL),
    tFeature("MINOR_ATTACKED_BY", true, Eval::MINOR_ATTACKED_BY, 4, 10, ALL),
    tFeature("ROOK_ATTACKED_BY", true, Eval::ROOK_ATTACKED_BY, 4, 10, ALL),
    tFeature("QUEEN_ATTACKED_BY", true, Eval::QUEEN_ATTACKED_BY, 4, 10, ALL),
    tFeature("KING_PAWN_SHIELD_KS", true, Eval::KING_PAWN_SHIELD_KS, 8, 4, ALL),
    tFeature("KING_PAWN_SHIELD_QS", true, Eval::KING_PAWN_SHIELD_QS, 8, 4, ALL),
    tFeature("BISHOP_MOBILITY", true, Eval::BISHOP_MOBILITY, 14, 7, ALL),
    tFeature("KNIGHT_MOBILITY", true, Eval::KNIGHT_MOBILITY, 9, 5, ALL),
    tFeature("KING_MOBILITY", true, Eval::KING_MOBILITY, 9, 5, ALL),
    tFeature("ROOK_MOBILITY", true, Eval::ROOK_MOBILITY, 15, 7, ALL),
    tFeature("QUEEN_MOBILITY", true, Eval::QUEEN_MOBILITY, 28, 7, ALL),
    tFeature("KING_PSQT_BLACK", true, Eval::KING_PSQT_BLACK, 64, 8, ALL),
    tFeature("PAWN_PSQT_BLACK", true, Eval::PAWN_PSQT_BLACK, 64, 8, ALL),
    tFeature("PAWN_PSQT_BLACK_IS_ENEMY_QUEEN", true, Eval::PAWN_PSQT_BLACK_IS_ENEMY_QUEEN, 64, 8, ALL),
    tFeature("PAWN_PSQT_BLACK_IS_OWN_QUEEN", true, Eval::PAWN_PSQT_BLACK_IS_OWN_QUEEN, 64, 8, ALL),
    tFeature("ROOK_PSQT_BLACK", true, Eval::ROOK_PSQT_BLACK, 64, 8, ALL),
    tFeature("BISHOP_PSQT_BLACK", true, Eval::BISHOP_PSQT_BLACK, 64, 8, ALL),
    tFeature("KNIGHT_PSQT_BLACK", true, Eval::KNIGHT_PSQT_BLACK, 64, 8, ALL),
    tFeature("QUEEN_PSQT_BLACK", true, Eval::QUEEN_PSQT_BLACK, 64, 8, ALL),
    tFeature("KNIGHT_PROT_OUTPOST_BLACK", true, KNIGHT_PROT_OUTPOST_BLACK, 32, 4, ALL),
    tFeature("BISHOP_PROT_OUTPOST_BLACK", true, BISHOP_PROT_OUTPOST_BLACK, 32, 4, ALL),
    tFeature("KNIGHT_OUTPOST_BLACK", true, KNIGHT_OUTPOST_BLACK, 32, 4, ALL),
    tFeature("BISHOP_OUTPOST_BLACK", true, BISHOP_OUTPOST_BLACK, 32, 4, ALL),
    tFeature("PAWN_CONNECTED", true, PAWN_CONNECTED, 32, 4, ALL),
    tFeature("PAWN_SUPPORTED", true, PAWN_SUPPORTED, 32, 4, ALL),
    tFeature("MATERIAL_VALUES", true, Eval::MATERIAL_VALUES, 5, 10, ALL),
};


#endif