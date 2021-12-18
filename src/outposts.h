#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 

const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(1,0), gS(3,2),
           gS(13,2), gS(30,4), gS(41,16), gS(32,17),
           gS(21,6), gS(38,7), gS(33,23), gS(35,25),
           gS(14,-1), gS(18,2), gS(28,14), gS(37,9),
           gS(3,-5), gS(1,-3), gS(4,4), gS(14,2),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,-1), gS(0,0), gS(0,0),
           gS(-1,-1), gS(14,17), gS(15,7), gS(3,11),
           gS(-7,-2), gS(22,15), gS(33,12), gS(36,8),
           gS(0,0), gS(30,10), gS(25,9), gS(46,9),
           gS(1,0), gS(24,7), gS(32,13), gS(43,17),
           gS(0,0), gS(13,11), gS(13,15), gS(15,21),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-15,-16), gS(7,0), gS(9,2), gS(16,0),
           gS(12,-5), gS(26,5), gS(29,16), gS(48,10),
           gS(6,0), gS(5,1), gS(28,14), gS(30,14),
           gS(8,-9), gS(0,0), gS(2,14), gS(1,12),
           gS(-2,-9), gS(-2,-1), gS(-12,3), gS(-6,6),
           gS(-7,-5), gS(-1,4), gS(-7,-1), gS(-12,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(4,-1), gS(6,-1), gS(17,0), gS(16,7),
           gS(3,0), gS(22,2), gS(22,4), gS(36,14),
           gS(17,1), gS(21,3), gS(26,7), gS(27,10),
           gS(6,10), gS(7,6), gS(4,6), gS(9,5),
           gS(-2,-2), gS(4,3), gS(-2,1), gS(1,5),
           gS(-3,2), gS(-7,5), gS(-3,9), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,4), gS(0,1), gS(0,1),
           gS(10,23), gS(16,40), gS(14,30), gS(14,30),
           gS(16,7), gS(13,15), gS(27,20), gS(26,17),
           gS(1,2), gS(10,4), gS(6,11), gS(10,17),
           gS(2,4), gS(2,0), gS(7,6), gS(7,6),
           gS(9,-5), gS(4,3), gS(5,-4), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(4,11), gS(11,11), gS(9,9), gS(8,16),
           gS(20,14), gS(32,15), gS(47,22), gS(52,24),
           gS(9,1), gS(7,10), gS(24,4), gS(19,16),
           gS(3,3), gS(15,0), gS(11,3), gS(11,8),
           gS(7,2), gS(15,6), gS(14,2), gS(11,9),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif