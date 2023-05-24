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
#ifndef RAYS_H
#define RAYS_H

#include "defs.h"

/**
 * @brief Namespace containg fast ray generation functions
 *
 * This namespace contains an init() function, which initializes a table of
 * 8*64 ray bitboards for each square and cardinal/intercardinal direction.
 *
 * In time intensive scenarios Rays::getRay() can then be used to get ray
 * bitboards when needed (eg. as masks for evaluation purposes).
 */
namespace Rays {
namespace detail {

/**
 * @brief Internal table of precalculated ray bitboards indexed by [Dir][square]
 */
extern U64 _rays[8][64];
};

/**
 * @enum Dir
 * @brief Enum representing the eight cardinal and intercardinal directions
 */
enum Dir {
  NORTH,
  SOUTH,
  EAST,
  WEST,
  NORTH_EAST,
  NORTH_WEST,
  SOUTH_EAST,
  SOUTH_WEST
};

/**
 * @brief Initializes the internal table of ray bitboards
 */
void init();

/**
 * @brief Gets a bitboard containing the given ray in the given direction.
 *
 * @param dir Direction of ray to return
 * @param square Square to get ray starting from (in little endian rank file mapping form)
 * @return A bitboard containing the given ray in the given direction
 */
U64 getRay(Dir, int);
};

#endif
