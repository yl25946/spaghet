#pragma once

#include "defs.h"
#include "attacks.h"
#include "board.h"
#include "movepicker.h"

class MoveList;

// generates all pseudolegal moves and puts them into an array
void generate_moves(Board &board, MoveList &move_list);

// used for qSearch
void generate_capture_moves(Board &board, MoveList &move_list);