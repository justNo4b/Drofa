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
           gS(13,2), gS(30,5), gS(33,13), gS(24,16),
           gS(20,6), gS(38,9), gS(29,21), gS(35,22),
           gS(14,-2), gS(16,2), gS(28,12), gS(37,8),
           gS(4,-3), gS(0,-2), gS(4,5), gS(12,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(14,17), gS(15,7), gS(2,10),
           gS(-6,-1), gS(22,14), gS(30,10), gS(32,6),
           gS(0,0), gS(30,8), gS(24,7), gS(43,6),
           gS(1,0), gS(23,6), gS(31,11), gS(40,16),
           gS(0,0), gS(11,11), gS(12,15), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-11,-14), gS(5,0), gS(5,0), gS(11,0),
           gS(10,-4), gS(26,5), gS(26,13), gS(45,8),
           gS(6,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-6), gS(0,1), gS(1,13), gS(0,9),
           gS(0,-5), gS(-1,-1), gS(-12,4), gS(-7,4),
           gS(-5,-1), gS(0,3), gS(-6,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(6,-1), gS(16,0), gS(13,6),
           gS(7,1), gS(22,1), gS(22,3), gS(33,12),
           gS(18,2), gS(20,2), gS(25,6), gS(25,9),
           gS(6,12), gS(5,6), gS(2,6), gS(9,3),
           gS(-2,0), gS(1,5), gS(-3,1), gS(0,3),
           gS(0,5), gS(-6,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,3), gS(0,0), gS(0,0),
           gS(7,22), gS(15,38), gS(13,27), gS(13,27),
           gS(16,10), gS(14,16), gS(26,20), gS(24,16),
           gS(1,3), gS(11,5), gS(7,11), gS(10,16),
           gS(1,4), gS(2,1), gS(6,6), gS(7,6),
           gS(8,-5), gS(3,2), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,10), gS(10,10), gS(7,9), gS(7,14),
           gS(19,15), gS(32,16), gS(46,23), gS(51,25),
           gS(9,2), gS(7,11), gS(25,5), gS(20,17),
           gS(3,4), gS(15,1), gS(11,4), gS(11,9),
           gS(6,2), gS(14,7), gS(14,3), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif