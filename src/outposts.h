#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv]
 */
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(2,1), gS(3,2),
           gS(15,2), gS(28,2), gS(38,14), gS(31,20),
           gS(13,4), gS(29,8), gS(26,19), gS(31,21),
           gS(17,0), gS(17,1), gS(29,12), gS(32,6),
           gS(2,-1), gS(1,0), gS(10,7), gS(11,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(16,18), gS(20,7), gS(4,11),
           gS(-6,-2), gS(20,9), gS(32,9), gS(33,6),
           gS(-2,-2), gS(24,6), gS(25,3), gS(40,4),
           gS(0,0), gS(27,7), gS(32,9), gS(39,13),
           gS(0,0), gS(12,12), gS(12,13), gS(16,17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-15,-17), gS(5,0), gS(7,0), gS(14,0),
           gS(6,-2), gS(22,8), gS(18,10), gS(39,5),
           gS(7,2), gS(4,2), gS(26,10), gS(24,10),
           gS(4,-5), gS(0,0), gS(2,14), gS(1,10),
           gS(0,-3), gS(0,-1), gS(-8,7), gS(-4,4),
           gS(-5,-2), gS(0,1), gS(-4,1), gS(-9,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(4,1), gS(9,1), gS(20,2), gS(14,6),
           gS(2,5), gS(13,2), gS(24,1), gS(32,7),
           gS(13,3), gS(22,4), gS(26,6), gS(26,10),
           gS(4,13), gS(5,6), gS(5,6), gS(5,5),
           gS(-4,2), gS(0,4), gS(-4,0), gS(1,6),
           gS(-3,4), gS(-5,3), gS(-3,5), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,5), gS(0,2), gS(0,1),
           gS(9,27), gS(21,52), gS(18,39), gS(20,38),
           gS(17,6), gS(10,8), gS(29,17), gS(25,14),
           gS(0,0), gS(12,3), gS(6,7), gS(12,15),
           gS(0,-1), gS(7,0), gS(5,3), gS(8,2),
           gS(5,-4), gS(0,3), gS(5,-6), gS(4,2),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,12), gS(14,14), gS(13,16), gS(13,21),
           gS(14,17), gS(31,20), gS(54,29), gS(57,32),
           gS(3,11), gS(5,17), gS(24,14), gS(24,22),
           gS(1,3), gS(13,5), gS(14,6), gS(16,11),
           gS(7,4), gS(13,9), gS(17,8), gS(17,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif
