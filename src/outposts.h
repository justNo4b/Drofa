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
           gS(14,2), gS(29,1), gS(34,8), gS(23,13),
           gS(20,5), gS(38,4), gS(29,18), gS(35,18),
           gS(14,-3), gS(16,0), gS(28,9), gS(37,5),
           gS(4,-4), gS(0,-2), gS(4,4), gS(12,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(14,17), gS(17,6), gS(2,11),
           gS(-5,-1), gS(24,13), gS(32,10), gS(33,6),
           gS(0,0), gS(30,8), gS(25,7), gS(43,5),
           gS(0,0), gS(24,6), gS(31,11), gS(41,16),
           gS(0,0), gS(11,10), gS(11,14), gS(15,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-16), gS(6,0), gS(4,0), gS(11,0),
           gS(11,-4), gS(25,4), gS(25,10), gS(45,5),
           gS(6,0), gS(3,0), gS(27,10), gS(29,9),
           gS(8,-6), gS(0,0), gS(1,13), gS(0,9),
           gS(0,-5), gS(-1,-1), gS(-12,3), gS(-7,3),
           gS(-4,-1), gS(-1,3), gS(-5,-1), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,0), gS(17,0), gS(15,6),
           gS(7,1), gS(22,1), gS(23,3), gS(34,12),
           gS(18,2), gS(21,2), gS(25,6), gS(25,9),
           gS(6,13), gS(5,5), gS(2,5), gS(9,3),
           gS(-2,0), gS(2,4), gS(-2,1), gS(0,3),
           gS(0,5), gS(-5,4), gS(-2,7), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,4), gS(0,0), gS(0,0),
           gS(8,23), gS(16,41), gS(14,30), gS(14,30),
           gS(16,7), gS(14,15), gS(27,21), gS(26,17),
           gS(1,1), gS(10,5), gS(7,11), gS(10,17),
           gS(1,4), gS(2,0), gS(7,6), gS(7,7),
           gS(8,-4), gS(4,1), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,11), gS(11,11), gS(9,10), gS(9,15),
           gS(19,14), gS(32,16), gS(48,23), gS(53,25),
           gS(9,2), gS(7,11), gS(25,6), gS(19,17),
           gS(3,3), gS(15,1), gS(11,5), gS(11,9),
           gS(6,2), gS(15,7), gS(14,3), gS(10,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif