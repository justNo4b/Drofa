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
           gS(16,2), gS(32,3), gS(34,13), gS(24,17),
           gS(18,7), gS(30,6), gS(23,21), gS(27,21),
           gS(12,-3), gS(16,0), gS(24,11), gS(34,3),
           gS(4,-4), gS(1,-2), gS(4,4), gS(14,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(20,7), gS(3,11),
           gS(-7,-2), gS(23,13), gS(31,10), gS(35,5),
           gS(0,0), gS(24,6), gS(24,4), gS(40,5),
           gS(2,0), gS(26,5), gS(32,7), gS(40,12),
           gS(0,0), gS(13,12), gS(12,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-13,-15), gS(8,0), gS(7,0), gS(14,0),
           gS(7,-3), gS(18,4), gS(21,11), gS(40,7),
           gS(4,0), gS(4,1), gS(24,10), gS(25,10),
           gS(8,-6), gS(0,1), gS(1,13), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-11,3), gS(-6,2),
           gS(-6,-2), gS(0,3), gS(-6,-1), gS(-12,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(8,0), gS(18,0), gS(17,7),
           gS(5,2), gS(14,2), gS(23,1), gS(35,10),
           gS(17,3), gS(23,3), gS(24,6), gS(24,9),
           gS(6,13), gS(6,6), gS(3,5), gS(8,3),
           gS(-2,0), gS(3,5), gS(-3,1), gS(1,4),
           gS(0,5), gS(-7,5), gS(-1,5), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,5), gS(0,1), gS(0,1),
           gS(9,27), gS(21,51), gS(17,37), gS(19,37),
           gS(18,8), gS(11,15), gS(28,21), gS(25,16),
           gS(1,3), gS(10,3), gS(7,10), gS(9,16),
           gS(1,5), gS(1,0), gS(6,6), gS(7,6),
           gS(8,-4), gS(3,2), gS(4,-5), gS(1,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,14), gS(15,14), gS(13,14), gS(13,20),
           gS(18,16), gS(33,19), gS(49,28), gS(55,25),
           gS(8,5), gS(7,15), gS(26,6), gS(21,17),
           gS(2,7), gS(14,2), gS(10,6), gS(11,11),
           gS(6,4), gS(14,8), gS(13,6), gS(10,11),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif