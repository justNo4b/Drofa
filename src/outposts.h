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
           gS(9,2), gS(25,5), gS(30,12), gS(20,15),
           gS(17,6), gS(37,9), gS(28,20), gS(34,21),
           gS(16,0), gS(13,2), gS(27,12), gS(37,9),
           gS(4,-2), gS(0,-2), gS(2,3), gS(9,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(11,14), gS(11,6), gS(2,6),
           gS(-5,-1), gS(19,13), gS(27,10), gS(27,6),
           gS(0,0), gS(29,7), gS(20,7), gS(39,6),
           gS(0,0), gS(19,6), gS(30,11), gS(37,16),
           gS(0,0), gS(10,10), gS(11,14), gS(16,19),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-9,-10), gS(3,0), gS(2,-1), gS(7,0),
           gS(10,-3), gS(26,5), gS(24,13), gS(44,8),
           gS(9,0), gS(3,0), gS(27,11), gS(29,11),
           gS(8,-5), gS(0,0), gS(0,11), gS(0,7),
           gS(0,-5), gS(-1,-1), gS(-12,2), gS(-7,2),
           gS(-4,-1), gS(-2,3), gS(-5,-1), gS(-8,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(5,0), gS(5,-1), gS(13,0), gS(9,5),
           gS(7,1), gS(20,1), gS(17,3), gS(27,12),
           gS(16,2), gS(15,2), gS(23,5), gS(22,9),
           gS(6,11), gS(5,5), gS(2,5), gS(9,3),
           gS(-2,0), gS(0,4), gS(-2,1), gS(2,3),
           gS(0,5), gS(-5,4), gS(-2,8), gS(-1,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,1), gS(0,2), gS(0,0), gS(0,0),
           gS(5,13), gS(9,22), gS(8,16), gS(8,16),
           gS(13,11), gS(17,20), gS(23,22), gS(23,20),
           gS(1,2), gS(11,6), gS(6,12), gS(10,17),
           gS(2,4), gS(2,2), gS(7,7), gS(8,7),
           gS(8,-3), gS(4,2), gS(5,-1), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(2,5), gS(5,5), gS(3,4), gS(3,8),
           gS(17,13), gS(25,15), gS(36,23), gS(39,25),
           gS(8,2), gS(7,11), gS(24,5), gS(19,16),
           gS(3,3), gS(15,1), gS(11,5), gS(12,10),
           gS(7,3), gS(15,8), gS(14,5), gS(12,11),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif