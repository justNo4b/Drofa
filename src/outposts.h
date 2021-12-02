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
           gS(8,2), gS(24,5), gS(28,11), gS(19,14),
           gS(16,6), gS(35,9), gS(27,19), gS(32,20),
           gS(16,0), gS(12,2), gS(25,11), gS(35,8),
           gS(4,-2), gS(-1,-2), gS(1,3), gS(8,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(10,13), gS(10,6), gS(2,5),
           gS(-5,-1), gS(18,13), gS(26,10), gS(25,6),
           gS(0,0), gS(28,7), gS(19,7), gS(37,6),
           gS(0,0), gS(18,6), gS(29,11), gS(36,16),
           gS(0,0), gS(9,10), gS(11,14), gS(16,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-8,-9), gS(3,0), gS(2,-1), gS(6,0),
           gS(10,-2), gS(26,5), gS(24,13), gS(44,9),
           gS(10,0), gS(3,0), gS(27,11), gS(30,12),
           gS(9,-5), gS(0,0), gS(1,12), gS(0,7),
           gS(-1,-5), gS(-1,-1), gS(-12,2), gS(-7,2),
           gS(-4,-1), gS(-3,3), gS(-5,-1), gS(-7,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(5,-1), gS(12,0), gS(8,5),
           gS(7,1), gS(19,1), gS(16,3), gS(26,12),
           gS(15,2), gS(14,2), gS(22,5), gS(21,9),
           gS(7,11), gS(5,5), gS(2,5), gS(9,3),
           gS(-2,0), gS(0,4), gS(-2,1), gS(2,3),
           gS(0,5), gS(-5,4), gS(-3,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,2), gS(0,0), gS(0,0),
           gS(4,10), gS(7,17), gS(6,13), gS(6,13),
           gS(12,11), gS(17,21), gS(23,23), gS(23,22),
           gS(1,2), gS(10,6), gS(7,12), gS(10,17),
           gS(2,4), gS(2,2), gS(7,7), gS(8,7),
           gS(8,-3), gS(5,2), gS(5,-1), gS(3,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,4), gS(4,4), gS(2,3), gS(2,6),
           gS(16,12), gS(22,14), gS(31,21), gS(33,23),
           gS(8,2), gS(6,10), gS(24,4), gS(19,15),
           gS(3,3), gS(15,2), gS(11,5), gS(12,10),
           gS(7,4), gS(16,8), gS(15,5), gS(13,11),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif