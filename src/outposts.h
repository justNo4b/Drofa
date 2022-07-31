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
           gS(15,2), gS(32,3), gS(35,12), gS(25,17),
           gS(19,7), gS(32,6), gS(26,19), gS(35,20),
           gS(12,-2), gS(17,0), gS(27,11), gS(38,6),
           gS(6,-3), gS(1,-2), gS(7,5), gS(14,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(20,7), gS(3,12),
           gS(-6,-2), gS(23,14), gS(32,10), gS(36,5),
           gS(0,0), gS(25,7), gS(25,6), gS(42,3),
           gS(1,0), gS(25,5), gS(32,9), gS(41,14),
           gS(0,0), gS(12,11), gS(13,15), gS(15,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-15), gS(8,0), gS(6,0), gS(13,0),
           gS(7,-4), gS(17,4), gS(22,10), gS(40,5),
           gS(4,0), gS(4,1), gS(24,10), gS(25,8),
           gS(8,-6), gS(0,1), gS(0,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-11,4), gS(-7,4),
           gS(-5,-2), gS(0,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,0), gS(17,0), gS(16,6),
           gS(5,2), gS(15,2), gS(23,0), gS(35,9),
           gS(17,3), gS(22,3), gS(24,5), gS(23,8),
           gS(6,13), gS(6,6), gS(3,6), gS(8,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-6,5), gS(-2,6), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,5), gS(0,1), gS(0,1),
           gS(9,28), gS(21,51), gS(16,37), gS(19,37),
           gS(19,7), gS(12,13), gS(28,20), gS(25,15),
           gS(1,2), gS(10,3), gS(7,10), gS(9,16),
           gS(1,3), gS(1,0), gS(6,5), gS(6,5),
           gS(8,-5), gS(3,1), gS(4,-5), gS(1,-1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,15), gS(15,15), gS(13,13), gS(13,20),
           gS(19,15), gS(34,16), gS(50,24), gS(55,24),
           gS(10,5), gS(8,13), gS(26,7), gS(21,18),
           gS(3,5), gS(14,1), gS(10,5), gS(11,9),
           gS(6,3), gS(14,6), gS(13,3), gS(9,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif