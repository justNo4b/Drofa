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
           gS(15,2), gS(33,2), gS(34,12), gS(24,17),
           gS(18,7), gS(30,5), gS(23,19), gS(28,20),
           gS(12,-2), gS(16,0), gS(25,11), gS(34,5),
           gS(3,-3), gS(1,-2), gS(4,4), gS(13,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(20,7), gS(3,12),
           gS(-6,-2), gS(23,14), gS(31,10), gS(35,5),
           gS(0,0), gS(24,7), gS(24,5), gS(40,3),
           gS(1,0), gS(24,5), gS(31,9), gS(40,14),
           gS(0,0), gS(12,11), gS(12,15), gS(14,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-15), gS(9,0), gS(7,0), gS(14,0),
           gS(7,-3), gS(18,5), gS(21,11), gS(41,5),
           gS(4,0), gS(4,1), gS(25,10), gS(26,9),
           gS(8,-6), gS(0,1), gS(0,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-11,4), gS(-6,4),
           gS(-5,-2), gS(0,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,0), gS(17,0), gS(16,6),
           gS(5,2), gS(15,2), gS(23,0), gS(35,9),
           gS(17,3), gS(22,3), gS(25,5), gS(24,9),
           gS(6,13), gS(6,6), gS(3,6), gS(8,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-6,5), gS(-2,6), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,6), gS(0,1), gS(0,1),
           gS(9,29), gS(22,52), gS(16,38), gS(19,38),
           gS(19,7), gS(12,13), gS(29,20), gS(25,15),
           gS(1,2), gS(10,3), gS(7,10), gS(9,16),
           gS(1,4), gS(1,0), gS(6,5), gS(7,5),
           gS(8,-5), gS(3,1), gS(4,-5), gS(1,-1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,16), gS(16,16), gS(14,13), gS(13,21),
           gS(19,15), gS(34,17), gS(50,24), gS(56,24),
           gS(10,5), gS(8,13), gS(26,8), gS(21,19),
           gS(3,5), gS(14,2), gS(10,5), gS(11,10),
           gS(5,3), gS(14,6), gS(13,3), gS(9,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif