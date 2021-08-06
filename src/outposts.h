#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,3), gS(7,2), gS(0,2), gS(0,0), gS(14,0), gS(0,3), gS(0,0), gS(0,0),
           gS(21,13), gS(12,24), gS(19,12), gS(25,14), gS(10,24), gS(38,18), gS(28,28), gS(32,8),
           gS(11,11), gS(27,10), gS(23,20), gS(29,19), gS(29,27), gS(26,23), gS(40,15), gS(17,16),
           gS(17,7), gS(15,8), gS(29,10), gS(34,11), gS(37,8), gS(24,8), gS(21,6), gS(16,5),
           gS(1,2), gS(0,-1), gS(6,2), gS(3,-1), gS(9,0), gS(-1,3), gS(0,0), gS(7,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,-1), gS(0,0), gS(0,0), gS(0,-4), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(23,0), gS(12,0), gS(0,0), gS(4,-5), gS(3,0), gS(28,0), gS(0,-1),
           gS(-6,-1), gS(27,2), gS(19,3), gS(40,0), gS(33,0), gS(62,-2), gS(31,7), gS(-10,-1),
           gS(-1,-1), gS(25,8), gS(31,-1), gS(42,3), gS(41,1), gS(34,3), gS(30,12), gS(0,2),
           gS(-2,2), gS(31,7), gS(36,6), gS(43,8), gS(49,12), gS(32,7), gS(31,5), gS(-4,0),
           gS(0,5), gS(10,15), gS(14,14), gS(16,16), gS(21,15), gS(6,6), gS(11,7), gS(-1,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,-5), gS(7,0), gS(7,-1), gS(18,0), gS(8,0), gS(28,-5), gS(14,0), gS(-7,-3),
           gS(8,2), gS(19,4), gS(23,6), gS(46,6), gS(42,7), gS(36,12), gS(34,8), gS(12,5),
           gS(8,2), gS(9,1), gS(26,9), gS(24,15), gS(35,10), gS(26,4), gS(10,1), gS(11,1),
           gS(5,2), gS(3,4), gS(4,10), gS(1,10), gS(1,10), gS(1,13), gS(5,0), gS(8,0),
           gS(-11,-5), gS(-2,0), gS(-12,14), gS(-6,4), gS(-7,1), gS(-10,8), gS(0,0), gS(-4,-1),
           gS(-3,0), gS(-5,0), gS(-8,5), gS(-10,0), gS(-6,0), gS(-4,0), gS(1,1), gS(-7,0),
};

const int BISHOP_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(8,-6), gS(13,-12), gS(11,-3), gS(19,1), gS(7,0), gS(30,0), gS(8,0), gS(-1,1),
           gS(14,0), gS(15,0), gS(24,0), gS(32,3), gS(31,1), gS(26,0), gS(21,0), gS(6,0),
           gS(12,0), gS(15,-1), gS(20,4), gS(23,5), gS(30,7), gS(25,3), gS(19,0), gS(10,2),
           gS(11,8), gS(1,7), gS(4,5), gS(8,6), gS(7,6), gS(0,4), gS(1,5), gS(5,7),
           gS(-5,3), gS(0,3), gS(-4,5), gS(2,5), gS(4,7), gS(0,1), gS(4,3), gS(-3,3),
           gS(2,5), gS(-4,15), gS(1,6), gS(0,6), gS(0,2), gS(-6,7), gS(-2,5), gS(0,3),
};
#endif