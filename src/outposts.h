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
           gS(14,2), gS(30,5), gS(34,12), gS(24,17),
           gS(20,7), gS(38,9), gS(29,22), gS(34,23),
           gS(14,-2), gS(16,2), gS(28,12), gS(38,7),
           gS(4,-3), gS(1,-2), gS(4,4), gS(13,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(16,7), gS(3,11),
           gS(-6,-2), gS(23,14), gS(31,10), gS(32,6),
           gS(0,0), gS(31,8), gS(24,7), gS(43,6),
           gS(1,0), gS(24,6), gS(32,11), gS(41,15),
           gS(0,0), gS(12,11), gS(12,15), gS(15,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-14), gS(6,0), gS(5,0), gS(11,0),
           gS(10,-3), gS(26,6), gS(25,13), gS(45,8),
           gS(6,0), gS(4,1), gS(27,11), gS(29,11),
           gS(8,-6), gS(0,1), gS(1,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-12,4), gS(-6,4),
           gS(-5,-2), gS(-1,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,-1), gS(16,0), gS(14,6),
           gS(7,2), gS(22,2), gS(22,3), gS(33,12),
           gS(18,3), gS(20,3), gS(26,6), gS(25,9),
           gS(6,13), gS(6,6), gS(3,6), gS(9,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-6,5), gS(-2,8), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,3), gS(0,1), gS(0,1),
           gS(9,23), gS(16,39), gS(13,29), gS(14,29),
           gS(16,7), gS(13,15), gS(27,21), gS(25,18),
           gS(1,2), gS(10,5), gS(7,12), gS(10,17),
           gS(2,4), gS(2,0), gS(7,6), gS(7,7),
           gS(8,-5), gS(4,3), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,10), gS(10,10), gS(8,9), gS(8,15),
           gS(20,15), gS(32,16), gS(47,23), gS(52,26),
           gS(8,2), gS(7,11), gS(25,5), gS(20,17),
           gS(3,4), gS(16,1), gS(11,5), gS(11,9),
           gS(7,2), gS(15,7), gS(14,4), gS(11,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif