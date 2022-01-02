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
           gS(15,2), gS(31,3), gS(34,12), gS(24,17),
           gS(20,7), gS(33,7), gS(28,20), gS(31,21),
           gS(13,-2), gS(16,1), gS(25,11), gS(35,6),
           gS(3,-3), gS(1,-2), gS(4,4), gS(13,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(19,7), gS(3,11),
           gS(-6,-2), gS(23,14), gS(31,10), gS(34,5),
           gS(0,0), gS(27,7), gS(25,6), gS(42,4),
           gS(1,0), gS(24,5), gS(31,9), gS(40,14),
           gS(0,0), gS(12,11), gS(12,15), gS(14,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-14), gS(7,0), gS(6,0), gS(13,0),
           gS(10,-3), gS(22,6), gS(25,13), gS(44,7),
           gS(4,0), gS(4,1), gS(26,11), gS(28,10),
           gS(8,-6), gS(0,1), gS(0,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-11,4), gS(-7,4),
           gS(-5,-2), gS(0,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,0), gS(17,0), gS(15,6),
           gS(7,2), gS(19,2), gS(23,2), gS(35,12),
           gS(17,3), gS(22,3), gS(25,5), gS(25,9),
           gS(6,13), gS(6,6), gS(3,6), gS(8,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-6,5), gS(-1,7), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,5), gS(0,1), gS(0,1),
           gS(9,26), gS(19,46), gS(16,35), gS(17,35),
           gS(18,7), gS(13,14), gS(27,21), gS(25,17),
           gS(0,2), gS(10,4), gS(7,10), gS(9,16),
           gS(3,3), gS(2,0), gS(7,5), gS(8,6),
           gS(8,-5), gS(3,1), gS(4,-5), gS(1,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,13), gS(13,13), gS(11,12), gS(11,18),
           gS(19,15), gS(34,16), gS(50,23), gS(55,24),
           gS(8,4), gS(7,13), gS(25,7), gS(20,18),
           gS(3,5), gS(15,2), gS(11,5), gS(11,9),
           gS(7,3), gS(15,5), gS(14,3), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif