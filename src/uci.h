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
#ifndef UCI_H
#define UCI_H

#include "searchdata.h"
#include "move.h"
#include "search.h"
#include "option.h"
#include "book.h"
#include <sstream>
#include <fstream>
#include <memory>

/**
 * @brief Class for handling UCI input/output.
 */
namespace Uci {
/**
 * @brief Initializes UCI options with their default values
 */
void init();

/**
 * @brief Starts listening for UCI input and responds to it when received
 */
void start();
};

#endif
