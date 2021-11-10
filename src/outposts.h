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
           gS(3,1), gS(17,5), gS(18,10), gS(13,11),
           gS(9,4), gS(35,9), gS(21,16), gS(30,18),
           gS(15,0), gS(6,2), gS(23,10), gS(31,10),
           gS(3,-1), gS(-2,-2), gS(0,2), gS(5,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(4,6), gS(4,3), gS(1,2),
           gS(-2,-1), gS(11,9), gS(17,9), gS(14,5),
           gS(0,0), gS(26,7), gS(12,6), gS(27,8),
           gS(0,0), gS(10,5), gS(24,11), gS(25,16),
           gS(0,0), gS(5,5), gS(9,10), gS(15,15),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-4,-4), gS(0,0), gS(0,-1), gS(2,-1),
           gS(6,-1), gS(24,5), gS(14,9), gS(30,8),
           gS(7,0), gS(1,0), gS(21,10), gS(25,12),
           gS(7,-3), gS(0,0), gS(3,8), gS(1,5),
           gS(-1,-2), gS(-1,-1), gS(-8,0), gS(-5,1),
           gS(-1,-1), gS(-3,1), gS(-2,-1), gS(-3,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,0), gS(2,-1), gS(6,0), gS(3,2),
           gS(4,1), gS(16,2), gS(8,2), gS(14,7),
           gS(10,2), gS(6,2), gS(18,6), gS(13,7),
           gS(8,7), gS(2,2), gS(2,4), gS(9,4),
           gS(-2,0), gS(0,3), gS(-2,1), gS(4,4),
           gS(0,2), gS(-2,1), gS(-2,7), gS(-1,0),
};

#endif