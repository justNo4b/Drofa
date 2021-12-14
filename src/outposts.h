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
           gS(13,2), gS(29,5), gS(34,12), gS(24,17),
           gS(20,6), gS(37,9), gS(29,21), gS(34,23),
           gS(14,-2), gS(16,2), gS(28,12), gS(37,7),
           gS(4,-3), gS(0,-2), gS(4,4), gS(13,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(15,7), gS(2,10),
           gS(-6,-1), gS(23,14), gS(31,10), gS(32,6),
           gS(0,0), gS(31,8), gS(24,7), gS(43,6),
           gS(1,0), gS(23,6), gS(32,11), gS(41,16),
           gS(0,0), gS(12,11), gS(12,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-14), gS(6,0), gS(5,0), gS(11,0),
           gS(10,-3), gS(26,6), gS(25,13), gS(45,8),
           gS(6,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-6), gS(0,0), gS(1,13), gS(0,10),
           gS(0,-5), gS(-1,-1), gS(-12,4), gS(-7,3),
           gS(-5,-1), gS(-1,3), gS(-5,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,-1), gS(16,0), gS(13,6),
           gS(7,2), gS(22,1), gS(21,3), gS(33,12),
           gS(18,2), gS(20,2), gS(25,6), gS(25,9),
           gS(6,13), gS(6,6), gS(2,6), gS(9,3),
           gS(-2,0), gS(2,5), gS(-2,1), gS(1,4),
           gS(0,5), gS(-6,5), gS(-2,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,3), gS(0,0), gS(0,0),
           gS(9,22), gS(15,38), gS(13,28), gS(13,28),
           gS(16,7), gS(14,16), gS(27,21), gS(26,18),
           gS(1,2), gS(11,5), gS(7,12), gS(10,17),
           gS(2,4), gS(1,1), gS(7,6), gS(7,7),
           gS(8,-5), gS(3,3), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,10), gS(10,10), gS(7,8), gS(7,14),
           gS(19,15), gS(31,16), gS(46,23), gS(51,26),
           gS(9,2), gS(7,11), gS(25,6), gS(20,17),
           gS(3,4), gS(15,1), gS(11,5), gS(11,9),
           gS(6,3), gS(14,7), gS(14,4), gS(10,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

#endif