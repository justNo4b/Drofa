#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,3), gS(7,0), gS(0,1), gS(2,0), gS(10,0), gS(0,3), gS(0,0), gS(0,0),
           gS(14,7), gS(16,12), gS(22,9), gS(26,12), gS(10,20), gS(43,14), gS(31,12), gS(35,4),
           gS(13,11), gS(30,6), gS(25,18), gS(33,17), gS(31,25), gS(26,22), gS(42,12), gS(19,9),
           gS(19,0), gS(13,6), gS(30,9), gS(34,9), gS(36,6), gS(23,11), gS(21,0), gS(15,1),
           gS(0,0), gS(-1,-1), gS(4,0), gS(5,-1), gS(13,0), gS(-3,3), gS(0,0), gS(7,-4),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,-1), gS(0,0), gS(0,0), gS(0,-2), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(25,2), gS(14,1), gS(0,4), gS(4,0), gS(8,1), gS(31,9), gS(0,-1),
           gS(-8,-1), gS(22,8), gS(22,7), gS(42,0), gS(33,0), gS(62,0), gS(32,9), gS(-10,-1),
           gS(-1,0), gS(25,8), gS(31,0), gS(43,2), gS(41,5), gS(24,6), gS(32,8), gS(0,0),
           gS(0,2), gS(31,1), gS(32,10), gS(41,11), gS(47,13), gS(32,7), gS(27,3), gS(0,0),
           gS(0,5), gS(13,17), gS(16,17), gS(14,18), gS(21,19), gS(8,11), gS(11,3), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-3,-7), gS(4,0), gS(5,-1), gS(18,0), gS(6,0), gS(20,-3), gS(10,0), gS(-6,-7),
           gS(8,0), gS(22,1), gS(25,5), gS(52,7), gS(42,7), gS(34,17), gS(34,11), gS(13,0),
           gS(8,0), gS(6,0), gS(29,10), gS(24,15), gS(35,10), gS(27,9), gS(10,2), gS(10,0),
           gS(6,0), gS(5,2), gS(1,9), gS(0,12), gS(0,8), gS(0,14), gS(-1,0), gS(9,-8),
           gS(-11,-5), gS(-2,0), gS(-15,8), gS(-7,4), gS(-7,2), gS(-11,3), gS(0,0), gS(0,-1),
           gS(-3,0), gS(-6,0), gS(-8,0), gS(-11,0), gS(-6,0), gS(-4,0), gS(0,3), gS(-7,0),
};

const int BISHOP_OUTPOST_BLACK[64] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(10,-4), gS(13,-12), gS(12,-4), gS(19,3), gS(13,0), gS(30,0), gS(8,0), gS(1,1),
           gS(13,0), gS(15,1), gS(24,0), gS(32,5), gS(37,1), gS(24,0), gS(21,0), gS(4,0),
           gS(14,0), gS(17,0), gS(20,3), gS(22,5), gS(30,5), gS(24,3), gS(22,0), gS(14,2),
           gS(9,11), gS(1,7), gS(4,4), gS(10,3), gS(8,6), gS(0,6), gS(2,5), gS(6,7),
           gS(-5,2), gS(-1,3), gS(-6,2), gS(2,4), gS(5,7), gS(0,2), gS(3,3), gS(-3,3),
           gS(0,5), gS(-6,15), gS(0,7), gS(-1,3), gS(0,0), gS(-8,10), gS(-2,5), gS(0,4),
};

#endif