#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(2,1), gS(3,2),
           gS(15,2), gS(29,2), gS(38,14), gS(31,20),
           gS(14,5), gS(30,8), gS(27,19), gS(32,21),
           gS(17,0), gS(17,1), gS(29,12), gS(32,7),
           gS(2,-2), gS(1,0), gS(10,7), gS(12,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(16,18), gS(20,7), gS(4,11),
           gS(-6,-2), gS(21,10), gS(32,10), gS(33,6),
           gS(-2,-2), gS(24,6), gS(25,3), gS(41,4),
           gS(0,0), gS(27,7), gS(32,9), gS(40,13),
           gS(0,0), gS(12,12), gS(12,14), gS(16,18),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-15,-17), gS(6,0), gS(7,0), gS(14,0),
           gS(7,-3), gS(22,8), gS(19,10), gS(40,6),
           gS(7,2), gS(4,2), gS(26,10), gS(25,10),
           gS(5,-6), gS(0,0), gS(2,14), gS(1,10),
           gS(0,-4), gS(-1,-1), gS(-9,7), gS(-5,5),
           gS(-5,-2), gS(0,2), gS(-5,1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,1), gS(9,1), gS(20,2), gS(15,6),
           gS(3,5), gS(14,2), gS(24,1), gS(33,8),
           gS(14,3), gS(22,4), gS(27,6), gS(26,10),
           gS(5,13), gS(5,6), gS(5,7), gS(5,5),
           gS(-4,2), gS(-1,4), gS(-4,0), gS(1,6),
           gS(-3,5), gS(-6,4), gS(-3,6), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,5), gS(0,2), gS(0,1),
           gS(9,27), gS(21,52), gS(18,39), gS(20,38),
           gS(17,6), gS(10,9), gS(29,18), gS(25,14),
           gS(0,0), gS(12,3), gS(6,7), gS(12,15),
           gS(1,-1), gS(7,-1), gS(5,3), gS(9,2),
           gS(5,-5), gS(1,3), gS(6,-6), gS(4,2),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,13), gS(14,14), gS(13,16), gS(13,21),
           gS(15,17), gS(32,20), gS(54,29), gS(58,32),
           gS(3,11), gS(6,17), gS(24,14), gS(25,22),
           gS(1,3), gS(14,5), gS(14,6), gS(16,12),
           gS(7,5), gS(14,9), gS(18,8), gS(17,11),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif
