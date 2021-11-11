#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(1,0), gS(2,2),
           gS(5,1), gS(23,5), gS(28,14), gS(23,14),
           gS(14,5), gS(36,9), gS(24,19), gS(33,21),
           gS(17,0), gS(10,2), gS(24,12), gS(33,11),
           gS(3,-3), gS(-2,-3), gS(0,2), gS(7,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(6,10), gS(6,5), gS(2,4),
           gS(-4,-2), gS(16,13), gS(23,11), gS(23,7),
           gS(0,0), gS(28,8), gS(16,8), gS(35,8),
           gS(0,0), gS(15,7), gS(28,12), gS(32,18),
           gS(0,0), gS(8,7), gS(11,12), gS(16,18),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-7,-7), gS(4,0), gS(3,0), gS(8,0),
           gS(11,-3), gS(26,5), gS(22,13), gS(42,11),
           gS(8,0), gS(2,0), gS(25,12), gS(29,14),
           gS(8,-6), gS(0,0), gS(2,10), gS(1,8),
           gS(-2,-5), gS(-2,-1), gS(-10,1), gS(-6,3),
           gS(-4,-4), gS(-3,2), gS(-4,-1), gS(-5,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(4,-1), gS(9,1), gS(7,4),
           gS(5,1), gS(19,2), gS(13,4), gS(22,11),
           gS(14,1), gS(10,3), gS(21,6), gS(19,9),
           gS(8,8), gS(4,3), gS(3,5), gS(10,4),
           gS(-2,-1), gS(0,3), gS(-2,1), gS(3,5),
           gS(-1,1), gS(-3,2), gS(-2,9), gS(-1,0),
};

#endif