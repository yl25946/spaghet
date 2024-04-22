#pragma once

#include "defs.h"

#include "move.h"
#include "board.h"

Move parse_move(const std::string &move_string, Board &board);

Board &parse_position(const std::string &command);