#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv]
 */
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(2,0), gS(1,0), gS(3,2),
           gS(15,2), gS(32,3), gS(34,12), gS(24,17),
           gS(19,7), gS(31,6), gS(25,19), gS(29,20),
           gS(12,-2), gS(16,0), gS(25,11), gS(34,5),
           gS(3,-3), gS(1,-2), gS(4,4), gS(13,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(19,7), gS(3,11),
           gS(-6,-2), gS(23,14), gS(31,10), gS(35,5),
           gS(0,0), gS(24,7), gS(25,6), gS(41,3),
           gS(1,0), gS(24,5), gS(31,9), gS(40,14),
           gS(0,0), gS(12,11), gS(12,15), gS(14,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-15), gS(8,0), gS(6,0), gS(13,0),
           gS(8,-3), gS(19,5), gS(23,11), gS(41,6),
           gS(4,0), gS(4,1), gS(25,10), gS(26,9),
           gS(8,-6), gS(0,1), gS(0,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-11,4), gS(-7,4),
           gS(-5,-2), gS(0,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,0), gS(17,0), gS(16,6),
           gS(5,2), gS(16,2), gS(23,1), gS(35,10),
           gS(17,3), gS(22,3), gS(25,5), gS(24,9),
           gS(6,13), gS(6,6), gS(3,6), gS(8,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(0,4),
           gS(0,6), gS(-6,5), gS(-2,6), gS(-2,0),
};
#endif