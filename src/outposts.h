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
           gS(15,2), gS(32,5), gS(34,12), gS(24,17),
           gS(20,7), gS(37,9), gS(29,21), gS(34,23),
           gS(14,-2), gS(16,2), gS(28,12), gS(38,7),
           gS(3,-3), gS(1,-2), gS(4,4), gS(13,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(15,18), gS(18,7), gS(3,11),
           gS(-6,-2), gS(23,14), gS(31,10), gS(34,6),
           gS(0,0), gS(31,8), gS(25,7), gS(43,6),
           gS(1,0), gS(24,6), gS(32,10), gS(41,15),
           gS(0,0), gS(12,11), gS(12,15), gS(15,20),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-12,-14), gS(6,0), gS(6,0), gS(12,0),
           gS(11,-3), gS(25,6), gS(25,13), gS(45,8),
           gS(6,0), gS(4,1), gS(27,11), gS(29,11),
           gS(8,-6), gS(0,1), gS(1,14), gS(0,10),
           gS(0,-6), gS(-1,-1), gS(-12,4), gS(-6,4),
           gS(-5,-2), gS(0,3), gS(-6,-1), gS(-11,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(6,0), gS(7,-1), gS(16,0), gS(14,6),
           gS(7,2), gS(22,2), gS(23,3), gS(35,12),
           gS(18,3), gS(22,3), gS(26,6), gS(25,9),
           gS(6,13), gS(6,6), gS(3,6), gS(9,3),
           gS(-2,0), gS(2,5), gS(-3,1), gS(1,4),
           gS(0,6), gS(-6,5), gS(-1,7), gS(-2,0),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,4), gS(0,1), gS(0,1),
           gS(9,25), gS(18,43), gS(15,33), gS(16,33),
           gS(17,7), gS(13,15), gS(27,21), gS(25,18),
           gS(1,2), gS(10,5), gS(7,11), gS(10,17),
           gS(3,4), gS(2,0), gS(7,6), gS(7,6),
           gS(9,-6), gS(3,3), gS(5,-3), gS(2,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,12), gS(12,12), gS(10,11), gS(10,17),
           gS(20,15), gS(32,16), gS(48,23), gS(54,24),
           gS(8,3), gS(8,11), gS(25,5), gS(20,16),
           gS(4,3), gS(16,1), gS(11,4), gS(11,9),
           gS(7,1), gS(16,6), gS(14,3), gS(10,10),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};
#endif