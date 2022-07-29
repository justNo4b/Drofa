#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(2,0), gS(1,0), gS(4,2),
           gS(16,2), gS(33,2), gS(34,12), gS(24,17),
           gS(18,7), gS(30,5), gS(23,19), gS(28,19),
           gS(12,-2), gS(16,0), gS(25,11), gS(33,5),
           gS(3,-4), gS(1,-2), gS(4,4), gS(13,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(16,19), gS(21,7), gS(3,12),
           gS(-7,-2), gS(23,14), gS(32,10), gS(36,4),
           gS(0,0), gS(24,7), gS(24,5), gS(40,3),
           gS(1,0), gS(25,5), gS(31,9), gS(40,14),
           gS(0,0), gS(13,11), gS(12,15), gS(14,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-13,-16), gS(9,0), gS(7,0), gS(14,0),
           gS(7,-4), gS(18,4), gS(21,11), gS(40,6),
           gS(4,-1), gS(4,1), gS(25,10), gS(25,9),
           gS(8,-7), gS(0,1), gS(0,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-11,4), gS(-6,4),
           gS(-6,-3), gS(0,3), gS(-6,-1), gS(-12,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(7,0), gS(8,0), gS(18,0), gS(17,7),
           gS(5,2), gS(14,2), gS(23,0), gS(35,9),
           gS(17,3), gS(23,3), gS(25,5), gS(24,9),
           gS(6,13), gS(6,6), gS(3,6), gS(9,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-7,5), gS(-1,6), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,3), gS(0,6), gS(0,1), gS(0,1),
           gS(9,29), gS(21,53), gS(17,38), gS(19,39),
           gS(19,7), gS(12,13), gS(29,20), gS(25,15),
           gS(1,2), gS(10,3), gS(6,10), gS(10,15),
           gS(1,4), gS(1,0), gS(6,5), gS(7,5),
           gS(7,-5), gS(3,1), gS(4,-6), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(4,16), gS(16,16), gS(14,14), gS(13,22),
           gS(18,15), gS(33,16), gS(50,23), gS(56,23),
           gS(10,5), gS(8,13), gS(27,7), gS(21,18),
           gS(2,5), gS(14,2), gS(10,5), gS(11,9),
           gS(5,3), gS(14,6), gS(13,3), gS(9,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif