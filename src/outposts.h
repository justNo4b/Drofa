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
           gS(12,2), gS(28,5), gS(33,13), gS(23,16),
           gS(19,6), gS(37,9), gS(29,21), gS(35,22),
           gS(14,-1), gS(15,2), gS(28,12), gS(37,8),
           gS(4,-3), gS(0,-2), gS(4,5), gS(12,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(14,17), gS(14,7), gS(2,9),
           gS(-6,-1), gS(21,14), gS(30,11), gS(31,6),
           gS(0,0), gS(30,8), gS(24,7), gS(42,6),
           gS(1,0), gS(23,6), gS(31,11), gS(40,16),
           gS(0,0), gS(11,11), gS(12,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-11,-13), gS(5,0), gS(5,0), gS(10,0),
           gS(10,-4), gS(26,5), gS(26,13), gS(45,8),
           gS(6,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-6), gS(0,1), gS(1,13), gS(0,9),
           gS(0,-5), gS(-1,-1), gS(-12,4), gS(-7,4),
           gS(-5,-1), gS(-1,3), gS(-6,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(6,-1), gS(16,0), gS(12,6),
           gS(7,1), gS(22,1), gS(21,3), gS(32,12),
           gS(18,2), gS(19,2), gS(25,6), gS(24,9),
           gS(6,13), gS(5,6), gS(2,6), gS(9,3),
           gS(-2,0), gS(1,5), gS(-3,1), gS(0,3),
           gS(0,5), gS(-6,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,3), gS(0,0), gS(0,0),
           gS(8,20), gS(14,34), gS(12,25), gS(12,25),
           gS(16,9), gS(15,17), gS(25,20), gS(24,17),
           gS(1,3), gS(10,5), gS(7,11), gS(9,16),
           gS(2,4), gS(2,1), gS(7,6), gS(7,6),
           gS(9,-5), gS(3,2), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,9), gS(9,9), gS(6,7), gS(6,13),
           gS(19,15), gS(31,16), gS(45,23), gS(49,26),
           gS(9,2), gS(7,11), gS(25,5), gS(19,17),
           gS(4,3), gS(15,1), gS(11,4), gS(11,9),
           gS(6,3), gS(16,6), gS(14,4), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif