#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(0,0), gS(2,1),
           gS(7,2), gS(23,5), gS(26,11), gS(17,14),
           gS(14,6), gS(36,9), gS(25,19), gS(32,20),
           gS(17,0), gS(10,2), gS(25,11), gS(35,9),
           gS(4,-2), gS(-2,-2), gS(1,2), gS(8,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(8,11), gS(8,5), gS(2,4),
           gS(-4,-1), gS(17,13), gS(24,10), gS(22,6),
           gS(0,0), gS(29,7), gS(17,7), gS(35,7),
           gS(0,0), gS(17,6), gS(29,11), gS(34,17),
           gS(0,0), gS(8,8), gS(11,13), gS(16,18),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-7,-8), gS(2,0), gS(1,-1), gS(5,-1),
           gS(9,-2), gS(27,5), gS(22,13), gS(42,10),
           gS(8,0), gS(2,0), gS(26,11), gS(29,13),
           gS(8,-5), gS(0,0), gS(2,11), gS(0,8),
           gS(-1,-4), gS(-1,-1), gS(-11,1), gS(-6,2),
           gS(-3,-1), gS(-3,2), gS(-4,-1), gS(-5,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(4,-1), gS(10,0), gS(6,4),
           gS(6,1), gS(19,1), gS(14,3), gS(23,11),
           gS(13,2), gS(11,2), gS(22,5), gS(20,10),
           gS(8,10), gS(4,4), gS(2,5), gS(10,4),
           gS(-2,0), gS(0,4), gS(-2,1), gS(3,4),
           gS(0,4), gS(-4,3), gS(-2,9), gS(-1,0),
};

#endif