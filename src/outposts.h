#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(3,1), gS(6,2), gS(6,3),
           gS(15,2), gS(31,5), gS(45,17), gS(33,19),
           gS(17,8), gS(27,2), gS(26,22), gS(20,19),
           gS(17,-3), gS(16,1), gS(21,9), gS(26,4),
           gS(6,-4), gS(2,-2), gS(2,4), gS(7,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(20,24), gS(20,9), gS(5,12),
           gS(-5,-1), gS(24,17), gS(46,20), gS(38,12),
           gS(-1,0), gS(16,7), gS(29,9), gS(41,8),
           gS(-1,-2), gS(25,8), gS(18,7), gS(37,11),
           gS(0,1), gS(10,11), gS(6,11), gS(8,18),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-15), gS(16,3), gS(8,3), gS(18,2),
           gS(8,-2), gS(18,5), gS(26,15), gS(42,10),
           gS(6,0), gS(6,2), gS(22,11), gS(25,14),
           gS(11,-7), gS(1,3), gS(4,16), gS(2,8),
           gS(-2,-9), gS(-2,-2), gS(-10,5), gS(-8,2),
           gS(-8,-4), gS(-3,2), gS(-7,0), gS(-12,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(15,5), gS(14,0), gS(16,7), gS(19,8),
           gS(9,4), gS(15,1), gS(27,5), gS(37,11),
           gS(20,4), gS(24,6), gS(20,9), gS(30,11),
           gS(6,11), gS(8,8), gS(4,9), gS(2,2),
           gS(0,2), gS(-3,6), gS(-5,0), gS(-1,2),
           gS(-1,7), gS(-7,4), gS(0,6), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,3), gS(0,7), gS(0,2), gS(0,3),
           gS(9,27), gS(21,50), gS(19,38), gS(21,40),
           gS(18,9), gS(12,16), gS(31,23), gS(25,16),
           gS(-1,0), gS(11,3), gS(5,10), gS(9,14),
           gS(7,1), gS(1,-2), gS(9,3), gS(8,3),
           gS(5,-3), gS(5,2), gS(5,-4), gS(2,4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(4,15), gS(17,18), gS(14,15), gS(14,23),
           gS(25,14), gS(41,25), gS(65,32), gS(66,30),
           gS(9,6), gS(13,16), gS(30,10), gS(29,17),
           gS(6,4), gS(18,-2), gS(11,4), gS(12,9),
           gS(4,2), gS(19,2), gS(15,2), gS(13,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif
