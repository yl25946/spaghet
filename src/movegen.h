#pragma once

#include "defs.h"
#include "attacks.h"
#include "board.h"
#include "movelist.h"

// generates all pseudolegal moves and puts them into an array
void generate_moves(Board &board, MoveList &move_list);

void generate_capture_moves(Board &board, MoveList &move_list);