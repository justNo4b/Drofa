#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 


const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(0,0), gS(2,1),
           gS(12,2), gS(28,5), gS(33,12), gS(23,16),
           gS(19,6), gS(38,9), gS(29,21), gS(35,22),
           gS(14,-1), gS(15,2), gS(28,12), gS(37,8),
           gS(4,-2), gS(0,-2), gS(4,4), gS(12,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(14,17), gS(14,6), gS(2,9),
           gS(-5,-1), gS(22,13), gS(30,10), gS(30,6),
           gS(0,0), gS(30,7), gS(23,7), gS(42,6),
           gS(0,0), gS(22,6), gS(31,11), gS(40,16),
           gS(0,0), gS(11,10), gS(11,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-11,-13), gS(5,0), gS(4,0), gS(10,0),
           gS(10,-3), gS(27,5), gS(25,13), gS(45,8),
           gS(6,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-5), gS(0,0), gS(0,13), gS(0,9),
           gS(0,-5), gS(-1,-1), gS(-12,3), gS(-7,3),
           gS(-4,-1), gS(-1,3), gS(-5,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(6,-1), gS(15,0), gS(12,5),
           gS(7,1), gS(22,1), gS(20,3), gS(31,12),
           gS(18,2), gS(18,2), gS(25,5), gS(24,9),
           gS(6,12), gS(5,5), gS(2,5), gS(9,3),
           gS(-2,0), gS(1,4), gS(-2,1), gS(0,3),
           gS(0,5), gS(-5,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,2), gS(0,0), gS(0,0),
           gS(8,19), gS(13,33), gS(11,24), gS(11,24),
           gS(15,8), gS(14,17), gS(26,21), gS(25,18),
           gS(1,2), gS(11,5), gS(7,11), gS(10,17),
           gS(1,4), gS(1,1), gS(7,6), gS(7,7),
           gS(8,-4), gS(3,3), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,8), gS(8,8), gS(6,7), gS(6,12),
           gS(19,14), gS(30,16), gS(44,23), gS(48,26),
           gS(9,2), gS(7,11), gS(25,5), gS(19,17),
           gS(3,4), gS(15,1), gS(11,4), gS(11,9),
           gS(6,3), gS(14,8), gS(14,4), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif