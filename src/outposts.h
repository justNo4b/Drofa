#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(0,0), gS(1,1),
           gS(5,2), gS(21,5), gS(23,11), gS(16,13),
           gS(12,5), gS(36,9), gS(24,18), gS(32,19),
           gS(17,0), gS(8,2), gS(25,11), gS(34,10),
           gS(4,-2), gS(-2,-2), gS(0,2), gS(7,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(6,9), gS(6,4), gS(2,3),
           gS(-3,-1), gS(14,11), gS(21,10), gS(19,6),
           gS(0,0), gS(28,7), gS(15,7), gS(32,8),
           gS(0,0), gS(14,6), gS(27,11), gS(31,17),
           gS(0,0), gS(7,7), gS(10,12), gS(16,17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-6,-6), gS(1,0), gS(0,-1), gS(3,-1),
           gS(7,-2), gS(26,5), gS(19,12), gS(38,10),
           gS(8,0), gS(1,0), gS(25,11), gS(28,13),
           gS(8,-4), gS(0,0), gS(2,10), gS(1,7),
           gS(-1,-3), gS(-1,-1), gS(-10,1), gS(-6,2),
           gS(-2,-1), gS(-3,1), gS(-3,-1), gS(-4,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(4,0), gS(3,-1), gS(8,0), gS(5,3),
           gS(5,1), gS(18,2), gS(11,3), gS(19,9),
           gS(12,2), gS(9,2), gS(21,6), gS(17,9),
           gS(8,9), gS(3,3), gS(2,5), gS(10,4),
           gS(-2,0), gS(0,4), gS(-2,1), gS(3,4),
           gS(0,3), gS(-3,2), gS(-2,8), gS(-1,0),
};

#endif