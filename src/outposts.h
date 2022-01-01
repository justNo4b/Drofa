#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(2,0), gS(1,0), gS(3,2),
           gS(16,2), gS(32,3), gS(35,12), gS(25,17),
           gS(20,7), gS(40,8), gS(31,21), gS(43,22),
           gS(15,-2), gS(17,2), gS(30,12), gS(42,8),
           gS(6,-3), gS(1,-1), gS(7,5), gS(14,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(16,18), gS(19,7), gS(3,12),
           gS(-6,-2), gS(24,14), gS(32,10), gS(35,5),
           gS(0,0), gS(32,8), gS(26,7), gS(46,6),
           gS(1,0), gS(25,6), gS(33,10), gS(42,15),
           gS(0,0), gS(13,11), gS(13,15), gS(16,21),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-14), gS(7,0), gS(6,0), gS(12,0),
           gS(11,-3), gS(25,6), gS(25,13), gS(45,8),
           gS(5,0), gS(4,1), gS(27,11), gS(29,10),
           gS(8,-6), gS(0,1), gS(1,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-11,4), gS(-6,4),
           gS(-5,-2), gS(0,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,0), gS(17,0), gS(15,6),
           gS(7,2), gS(21,2), gS(23,3), gS(35,12),
           gS(18,3), gS(22,3), gS(25,6), gS(25,9),
           gS(6,13), gS(6,6), gS(3,6), gS(9,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-6,5), gS(-1,7), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,5), gS(0,1), gS(0,1),
           gS(9,26), gS(19,46), gS(16,35), gS(17,35),
           gS(18,7), gS(13,14), gS(27,21), gS(25,17),
           gS(1,2), gS(10,3), gS(7,10), gS(10,16),
           gS(2,3), gS(2,0), gS(7,5), gS(7,6),
           gS(8,-5), gS(3,1), gS(4,-4), gS(1,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,13), gS(13,13), gS(11,12), gS(11,18),
           gS(19,15), gS(33,16), gS(50,23), gS(55,24),
           gS(9,4), gS(8,13), gS(26,7), gS(21,18),
           gS(3,5), gS(16,1), gS(11,5), gS(11,9),
           gS(7,3), gS(15,5), gS(14,3), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

#endif