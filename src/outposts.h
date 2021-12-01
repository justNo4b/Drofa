#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(0,0), gS(1,1),
           gS(7,2), gS(23,5), gS(26,11), gS(18,14),
           gS(14,6), gS(36,9), gS(26,19), gS(33,20),
           gS(16,0), gS(10,2), gS(25,11), gS(34,9),
           gS(4,-2), gS(-1,-2), gS(1,3), gS(8,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(8,11), gS(8,5), gS(2,4),
           gS(-4,-1), gS(16,12), gS(24,10), gS(22,6),
           gS(0,0), gS(28,7), gS(17,7), gS(35,7),
           gS(0,0), gS(16,6), gS(28,11), gS(34,17),
           gS(0,0), gS(8,9), gS(11,13), gS(16,18),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-7,-8), gS(2,0), gS(1,-1), gS(5,0),
           gS(9,-2), gS(27,5), gS(22,13), gS(42,10),
           gS(10,0), gS(2,0), gS(27,11), gS(30,13),
           gS(9,-4), gS(0,0), gS(1,11), gS(0,7),
           gS(-1,-4), gS(-1,-1), gS(-11,1), gS(-7,2),
           gS(-3,-1), gS(-3,2), gS(-4,-1), gS(-6,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(4,-1), gS(10,0), gS(7,4),
           gS(6,1), gS(20,2), gS(14,3), gS(23,11),
           gS(14,2), gS(12,2), gS(22,6), gS(20,9),
           gS(8,10), gS(4,4), gS(2,5), gS(9,3),
           gS(-2,0), gS(0,4), gS(-2,1), gS(2,4),
           gS(0,4), gS(-4,3), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,1), gS(0,0), gS(0,0),
           gS(2,5), gS(4,9), gS(3,7), gS(3,7),
           gS(12,11), gS(20,22), gS(23,23), gS(25,23),
           gS(2,2), gS(10,7), gS(7,12), gS(9,18),
           gS(1,3), gS(1,3), gS(6,7), gS(8,7),
           gS(7,-2), gS(6,1), gS(4,-1), gS(3,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(1,2), gS(2,2), gS(1,1), gS(1,3),
           gS(11,8), gS(14,10), gS(20,15), gS(21,16),
           gS(7,1), gS(6,7), gS(22,3), gS(18,13),
           gS(3,3), gS(14,1), gS(10,4), gS(11,9),
           gS(6,4), gS(14,8), gS(14,5), gS(12,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif