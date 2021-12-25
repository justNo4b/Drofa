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
           gS(15,2), gS(33,4), gS(34,12), gS(24,17),
           gS(20,7), gS(37,9), gS(29,21), gS(34,23),
           gS(13,-2), gS(16,2), gS(28,12), gS(37,7),
           gS(4,-3), gS(1,-2), gS(4,4), gS(13,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(20,7), gS(3,12),
           gS(-6,-2), gS(23,14), gS(31,10), gS(35,5),
           gS(0,0), gS(31,8), gS(25,7), gS(43,6),
           gS(1,0), gS(25,6), gS(32,10), gS(41,15),
           gS(0,0), gS(12,11), gS(12,15), gS(15,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-14), gS(7,0), gS(6,0), gS(12,0),
           gS(11,-3), gS(25,6), gS(25,13), gS(45,8),
           gS(6,0), gS(4,1), gS(27,11), gS(29,11),
           gS(8,-6), gS(0,1), gS(1,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-12,4), gS(-6,4),
           gS(-5,-2), gS(0,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,0), gS(16,0), gS(15,6),
           gS(7,2), gS(21,2), gS(23,3), gS(35,12),
           gS(18,3), gS(23,3), gS(26,6), gS(25,9),
           gS(6,13), gS(6,6), gS(3,6), gS(9,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-6,5), gS(-2,7), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,5), gS(0,1), gS(0,1),
           gS(9,27), gS(20,48), gS(17,36), gS(18,36),
           gS(17,7), gS(13,15), gS(27,21), gS(25,18),
           gS(1,2), gS(10,5), gS(7,11), gS(10,17),
           gS(2,4), gS(2,0), gS(6,7), gS(7,7),
           gS(8,-5), gS(4,3), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,14), gS(14,14), gS(12,13), gS(12,19),
           gS(20,15), gS(32,16), gS(48,23), gS(55,23),
           gS(8,2), gS(8,11), gS(25,5), gS(20,16),
           gS(3,4), gS(16,1), gS(11,4), gS(11,9),
           gS(6,2), gS(14,7), gS(14,3), gS(10,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif