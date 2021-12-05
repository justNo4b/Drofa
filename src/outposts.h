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
           gS(12,2), gS(28,5), gS(35,13), gS(25,16),
           gS(20,6), gS(40,10), gS(33,22), gS(44,23),
           gS(15,-1), gS(17,3), gS(31,13), gS(44,9),
           gS(7,-2), gS(1,-1), gS(8,7), gS(13,2),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(14,17), gS(14,6), gS(2,9),
           gS(-5,-1), gS(23,14), gS(31,11), gS(31,7),
           gS(0,0), gS(32,8), gS(24,8), gS(45,7),
           gS(1,0), gS(23,7), gS(33,11), gS(42,17),
           gS(0,0), gS(12,11), gS(13,15), gS(17,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-11,-13), gS(4,0), gS(4,0), gS(9,0),
           gS(10,-4), gS(26,5), gS(25,13), gS(46,7),
           gS(6,0), gS(3,0), gS(27,11), gS(28,11),
           gS(8,-5), gS(0,0), gS(0,13), gS(0,9),
           gS(0,-5), gS(-1,-1), gS(-12,3), gS(-7,3),
           gS(-4,-1), gS(-1,3), gS(-5,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(6,-1), gS(15,0), gS(12,5),
           gS(7,1), gS(21,1), gS(20,3), gS(31,12),
           gS(17,2), gS(18,2), gS(24,5), gS(23,9),
           gS(6,12), gS(5,5), gS(2,5), gS(8,3),
           gS(-2,0), gS(1,4), gS(-2,1), gS(0,3),
           gS(0,5), gS(-5,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,2), gS(0,0), gS(0,0),
           gS(8,19), gS(13,33), gS(11,24), gS(11,24),
           gS(16,10), gS(15,17), gS(25,20), gS(24,17),
           gS(1,3), gS(11,5), gS(7,11), gS(10,16),
           gS(1,4), gS(2,1), gS(6,6), gS(7,6),
           gS(8,-4), gS(4,2), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,8), gS(8,8), gS(6,7), gS(6,12),
           gS(19,14), gS(30,16), gS(44,23), gS(48,26),
           gS(9,2), gS(7,11), gS(25,5), gS(19,17),
           gS(3,4), gS(15,1), gS(11,4), gS(11,9),
           gS(6,3), gS(14,7), gS(14,3), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif