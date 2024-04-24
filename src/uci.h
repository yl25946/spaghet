#pragma once

#include "defs.h"

#include "move.h"
#include "board.h"
#include "search.h"
#include "time.h"

Move parse_move(const std::string &move_string, Board &board);

void UCI_loop();

// position fen
// position startpos
// ... moves e2e4 e7e5
Board parse_position(std::string &line);

void parse_moves(std::string &line, std::vector<Move> &move_list);

// Board &parse_position(const std::string &command);