/*
    Drofa - UCI compatable chess engine
        Copyright (C) 2017 - 2019  Rhys Rustad-Elliott
                      2020 - 2023  Litov Alexander
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef TRANSPTABLEENTRY_H
#define TRANSPTABLEENTRY_H

#include "move.h"
#include <cstdint>

/**
 * @brief Represent CuttOffState of the node saved in the transposition table.
 */
enum CutOffState{
    NONE,
    EXACT,
    ALPHA,
    BETA
};

/**
 * @brief Represents an entry in a transposition table.
 *
 * Stores key, score, depth, upper/lower bound information and the best move found
 */
struct HASH_Entry
{
  U64 posKey;           // 8
  int move;             // 4
  int16_t score;        // 2
  uint8_t depth;        // 1
  uint8_t Flag;         // 1

  HASH_Entry() : posKey(0), move(0),  score(0), depth(0), Flag(NONE) {}
  HASH_Entry( U64 key, int cMove, int16_t s, uint8_t d, CutOffState state) :
      posKey(key), move(cMove), score(s), depth(d), Flag(state) {}
};

/**
 * @brief Represents an entry in pawn hash table
 *
 * Stores key and scores for an ENDGAME and OPENING elavuation.
 * Also store bitboards for white and black passed pawns
 */

struct pawn_HASH_Entry
{
    U64 posKey;
    U64 wPassers;
    U64 bPassers;
    int score;

    pawn_HASH_Entry() : posKey(0), wPassers(0), bPassers(0), score(0) {}
    pawn_HASH_Entry( U64 key, U64 wP, U64 bP, int sc ) :
        posKey(key), wPassers(wP), bPassers(bP), score(sc) {}
};


#endif
