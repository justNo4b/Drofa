#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(0,0), gS(3,1),
           gS(11,2), gS(27,5), gS(33,13), gS(23,16),
           gS(19,6), gS(37,9), gS(29,21), gS(35,22),
           gS(14,-1), gS(15,2), gS(28,12), gS(37,8),
           gS(4,-3), gS(0,-2), gS(4,5), gS(11,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(13,16), gS(13,7), gS(2,8),
           gS(-6,-1), gS(21,14), gS(30,11), gS(30,6),
           gS(0,0), gS(30,8), gS(23,7), gS(42,6),
           gS(1,0), gS(22,6), gS(31,11), gS(39,16),
           gS(0,0), gS(11,11), gS(12,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-11,-12), gS(4,0), gS(4,0), gS(9,0),
           gS(10,-4), gS(26,5), gS(26,13), gS(45,8),
           gS(6,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-6), gS(0,1), gS(1,13), gS(0,9),
           gS(0,-5), gS(-1,-1), gS(-12,4), gS(-7,4),
           gS(-5,-1), gS(-1,3), gS(-6,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(6,-1), gS(15,0), gS(11,6),
           gS(7,1), gS(22,1), gS(20,3), gS(31,12),
           gS(17,2), gS(18,2), gS(25,6), gS(24,9),
           gS(6,13), gS(5,6), gS(2,6), gS(9,3),
           gS(-2,0), gS(1,5), gS(-3,1), gS(0,3),
           gS(0,5), gS(-6,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,3), gS(0,0), gS(0,0),
           gS(7,18), gS(13,31), gS(11,23), gS(11,23),
           gS(15,9), gS(15,17), gS(24,20), gS(22,17),
           gS(1,3), gS(11,5), gS(7,11), gS(10,16),
           gS(2,4), gS(2,1), gS(7,6), gS(8,6),
           gS(8,-4), gS(4,2), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,8), gS(8,8), gS(5,6), gS(5,12),
           gS(19,15), gS(30,16), gS(43,23), gS(47,26),
           gS(9,2), gS(7,11), gS(25,6), gS(19,17),
           gS(3,4), gS(15,1), gS(11,4), gS(11,9),
           gS(6,3), gS(14,7), gS(14,4), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif