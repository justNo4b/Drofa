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
           gS(11,2), gS(27,5), gS(32,12), gS(22,16),
           gS(19,6), gS(37,9), gS(29,21), gS(34,22),
           gS(15,-1), gS(15,2), gS(27,12), gS(37,8),
           gS(4,-2), gS(0,-2), gS(3,4), gS(11,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(13,16), gS(13,6), gS(2,8),
           gS(-5,-1), gS(21,13), gS(29,10), gS(29,6),
           gS(0,0), gS(30,7), gS(22,7), gS(41,6),
           gS(0,0), gS(21,6), gS(31,11), gS(39,16),
           gS(0,0), gS(10,10), gS(11,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-10,-12), gS(4,0), gS(4,0), gS(9,0),
           gS(10,-3), gS(26,5), gS(25,13), gS(45,8),
           gS(7,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-5), gS(0,0), gS(0,12), gS(0,8),
           gS(0,-5), gS(-1,-1), gS(-12,3), gS(-7,3),
           gS(-4,-1), gS(-1,3), gS(-5,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(6,-1), gS(15,0), gS(11,5),
           gS(7,1), gS(21,1), gS(19,3), gS(30,12),
           gS(17,2), gS(17,2), gS(25,5), gS(24,9),
           gS(6,12), gS(5,5), gS(2,5), gS(9,3),
           gS(-2,0), gS(1,4), gS(-2,1), gS(0,3),
           gS(0,5), gS(-5,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,3), gS(0,0), gS(0,0),
           gS(7,17), gS(12,30), gS(10,22), gS(10,22),
           gS(15,10), gS(16,18), gS(25,21), gS(24,18),
           gS(1,3), gS(11,5), gS(7,11), gS(10,16),
           gS(2,4), gS(2,1), gS(7,7), gS(8,6),
           gS(8,-3), gS(3,2), gS(5,-2), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,7), gS(7,7), gS(5,6), gS(5,11),
           gS(18,14), gS(29,16), gS(42,23), gS(46,26),
           gS(9,2), gS(7,11), gS(25,5), gS(19,17),
           gS(3,4), gS(15,1), gS(11,4), gS(11,9),
           gS(6,3), gS(14,8), gS(14,4), gS(12,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif