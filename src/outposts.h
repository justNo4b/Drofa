#ifndef OUTPOSTS_H
#define OUTPOSTS_H

#include "eval.h"

//various OUTPOSTS PSQT are defined here

/**
 * @brief [color][sqv] 
 */ 
const int KNIGHT_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(1,0), gS(3,3), gS(4,3),
           gS(15,3), gS(29,2), gS(41,16), gS(35,22),
           gS(11,5), gS(29,9), gS(25,19), gS(30,21),
           gS(20,0), gS(18,3), gS(30,13), gS(31,9),
           gS(2,-1), gS(1,1), gS(10,7), gS(11,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int BISHOP_PROT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(17,19), gS(22,9), gS(6,12),
           gS(-7,-3), gS(20,7), gS(33,11), gS(34,8),
           gS(-3,-4), gS(24,6), gS(24,2), gS(40,4),
           gS(0,0), gS(29,9), gS(32,9), gS(40,13),
           gS(0,0), gS(12,12), gS(12,12), gS(16,17),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int KNIGHT_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(-17,-19), gS(6,0), gS(9,-1), gS(15,1),
           gS(6,-2), gS(23,10), gS(16,10), gS(38,6),
           gS(8,3), gS(4,3), gS(26,11), gS(24,11),
           gS(4,-6), gS(0,0), gS(3,14), gS(2,10),
           gS(0,-4), gS(0,-1), gS(-7,9), gS(-5,6),
           gS(-6,-3), gS(1,1), gS(-5,3), gS(-10,0),
};

const int BISHOP_OUTPOST_BLACK[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,2), gS(11,1), gS(22,5), gS(16,6),
           gS(3,7), gS(12,3), gS(24,2), gS(31,7),
           gS(14,3), gS(22,5), gS(28,6), gS(28,11),
           gS(5,14), gS(6,6), gS(6,6), gS(4,7),
           gS(-4,4), gS(-1,4), gS(-5,0), gS(1,7),
           gS(-5,5), gS(-6,5), gS(-3,7), gS(-2,1),
};

const int PAWN_CONNECTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,2), gS(0,6), gS(0,3), gS(0,2),
           gS(9,27), gS(22,54), gS(19,42), gS(22,41),
           gS(17,6), gS(10,7), gS(29,17), gS(24,14),
           gS(0,0), gS(12,4), gS(6,6), gS(12,15),
           gS(0,-1), gS(9,-1), gS(5,3), gS(9,2),
           gS(5,-5), gS(0,4), gS(6,-7), gS(5,1),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

const int PAWN_SUPPORTED[32] = {
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(3,12), gS(15,14), gS(15,18), gS(15,24),
           gS(12,19), gS(29,22), gS(54,32), gS(59,37),
           gS(1,13), gS(5,17), gS(23,15), gS(25,22),
           gS(2,2), gS(13,5), gS(15,6), gS(17,11),
           gS(7,5), gS(16,8), gS(18,7), gS(18,9),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
           gS(0,0), gS(0,0), gS(0,0), gS(0,0),
};

#endif
