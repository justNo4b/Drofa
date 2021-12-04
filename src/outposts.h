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
           gS(10,2), gS(26,5), gS(31,12), gS(21,15),
           gS(18,6), gS(37,9), gS(29,21), gS(35,22),
           gS(15,-1), gS(14,2), gS(27,12), gS(36,8),
           gS(4,-2), gS(0,-2), gS(3,4), gS(10,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(12,15), gS(12,6), gS(2,7),
           gS(-5,-1), gS(20,13), gS(28,10), gS(28,6),
           gS(0,0), gS(30,7), gS(21,7), gS(40,6),
           gS(0,0), gS(20,6), gS(31,11), gS(38,16),
           gS(0,0), gS(10,10), gS(11,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-10,-11), gS(3,0), gS(3,0), gS(8,0),
           gS(10,-3), gS(26,5), gS(25,13), gS(45,8),
           gS(7,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-5), gS(0,0), gS(0,12), gS(0,8),
           gS(0,-5), gS(-1,-1), gS(-12,3), gS(-7,3),
           gS(-4,-1), gS(-1,3), gS(-5,-1), gS(-9,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(5,-1), gS(14,0), gS(10,5),
           gS(7,1), gS(21,1), gS(18,3), gS(29,12),
           gS(17,2), gS(16,2), gS(24,5), gS(23,9),
           gS(6,12), gS(5,5), gS(2,5), gS(9,3),
           gS(-2,0), gS(1,4), gS(-2,1), gS(1,3),
           gS(0,5), gS(-5,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,2), gS(0,0), gS(0,0),
           gS(6,15), gS(11,26), gS(9,19), gS(9,19),
           gS(14,10), gS(16,19), gS(24,21), gS(24,19),
           gS(1,2), gS(11,5), gS(7,11), gS(10,16),
           gS(2,4), gS(2,1), gS(6,6), gS(7,6),
           gS(8,-4), gS(4,1), gS(5,-2), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,6), gS(6,6), gS(4,5), gS(4,10),
           gS(18,14), gS(27,16), gS(39,23), gS(43,26),
           gS(9,2), gS(7,11), gS(25,5), gS(19,17),
           gS(3,4), gS(15,1), gS(11,4), gS(11,9),
           gS(7,2), gS(14,7), gS(14,4), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif