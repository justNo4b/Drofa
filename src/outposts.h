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
           gS(11,2), gS(27,5), gS(34,13), gS(24,16),
           gS(19,6), gS(41,10), gS(33,23), gS(45,24),
           gS(16,-1), gS(16,3), gS(31,14), gS(44,10),
           gS(8,-2), gS(1,-1), gS(8,7), gS(12,2),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(13,16), gS(13,6), gS(2,8),
           gS(-5,-1), gS(22,14), gS(30,11), gS(30,7),
           gS(0,0), gS(33,8), gS(23,8), gS(44,8),
           gS(1,0), gS(22,7), gS(33,12), gS(41,17),
           gS(0,0), gS(11,11), gS(13,15), gS(17,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-11,-12), gS(3,0), gS(3,0), gS(8,0),
           gS(10,-4), gS(26,5), gS(25,13), gS(45,7),
           gS(6,0), gS(3,0), gS(27,11), gS(28,11),
           gS(8,-5), gS(0,0), gS(0,13), gS(0,8),
           gS(0,-5), gS(-1,-1), gS(-12,3), gS(-7,3),
           gS(-4,-1), gS(-1,3), gS(-5,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(5,-1), gS(15,0), gS(11,5),
           gS(7,1), gS(21,1), gS(19,3), gS(30,12),
           gS(17,2), gS(17,2), gS(24,5), gS(23,9),
           gS(6,12), gS(5,5), gS(2,5), gS(8,3),
           gS(-2,0), gS(1,4), gS(-2,1), gS(1,3),
           gS(0,5), gS(-5,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,2), gS(0,0), gS(0,0),
           gS(7,17), gS(12,30), gS(10,22), gS(10,22),
           gS(15,10), gS(16,18), gS(25,21), gS(24,18),
           gS(1,3), gS(11,5), gS(7,11), gS(10,16),
           gS(2,4), gS(2,1), gS(7,6), gS(8,6),
           gS(7,-4), gS(3,2), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,7), gS(7,7), gS(5,6), gS(5,11),
           gS(18,14), gS(29,16), gS(42,23), gS(46,26),
           gS(9,2), gS(7,11), gS(24,5), gS(19,17),
           gS(3,3), gS(15,1), gS(11,4), gS(11,9),
           gS(6,2), gS(14,7), gS(14,3), gS(11,9),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif