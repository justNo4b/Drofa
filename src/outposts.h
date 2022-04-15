#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(3,1), gS(5,2), gS(5,3),
           gS(15,2), gS(29,4), gS(41,15), gS(29,17),
           gS(15,7), gS(24,2), gS(22,19), gS(18,17),
           gS(15,-3), gS(15,0), gS(20,8), gS(24,3),
           gS(5,-4), gS(2,-2), gS(2,4), gS(7,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(19,23), gS(20,9), gS(5,12),
           gS(-5,-2), gS(23,16), gS(43,18), gS(36,10),
           gS(-1,0), gS(14,6), gS(27,8), gS(38,7),
           gS(-1,-1), gS(25,7), gS(17,7), gS(34,10),
           gS(0,1), gS(10,11), gS(6,11), gS(7,17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-15), gS(14,2), gS(7,3), gS(17,2),
           gS(6,-2), gS(16,5), gS(24,14), gS(39,8),
           gS(5,0), gS(5,2), gS(20,10), gS(22,13),
           gS(10,-7), gS(1,3), gS(3,15), gS(1,8),
           gS(-1,-8), gS(-2,-2), gS(-10,5), gS(-7,2),
           gS(-7,-4), gS(-3,3), gS(-7,0), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(14,4), gS(13,0), gS(16,6), gS(19,7),
           gS(7,3), gS(13,1), gS(26,4), gS(35,10),
           gS(17,4), gS(23,5), gS(19,8), gS(28,10),
           gS(5,10), gS(7,7), gS(3,8), gS(2,2),
           gS(0,2), gS(-3,6), gS(-5,0), gS(-1,2),
           gS(-1,7), gS(-7,4), gS(0,6), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,3), gS(0,7), gS(0,2), gS(0,2),
           gS(9,26), gS(21,50), gS(18,37), gS(21,39),
           gS(16,8), gS(10,14), gS(28,21), gS(23,15),
           gS(-1,0), gS(10,3), gS(5,9), gS(8,13),
           gS(6,1), gS(0,-2), gS(8,3), gS(7,3),
           gS(5,-2), gS(4,2), gS(4,-4), gS(2,3),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(4,15), gS(17,17), gS(14,15), gS(14,23),
           gS(23,13), gS(38,22), gS(62,30), gS(63,28),
           gS(8,5), gS(11,15), gS(27,9), gS(26,16),
           gS(5,4), gS(16,-2), gS(10,4), gS(11,9),
           gS(4,3), gS(16,3), gS(13,2), gS(11,9),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif
