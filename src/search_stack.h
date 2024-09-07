#pragma once

#include "defs.h"
#include "history.h"
#include "movelist.h"

class SearchStack
{
public:
    bool null_moved = false;
    bool exclude_tt_move = false;
    // used for lazy updates,
    bool updated_accumulator = false;
    bool in_check = false;
    int ply;
    Move killer;
    MoveList pv;

    Board board;
    Move move_played = NO_MOVE;
    Move tt_move;
    int static_eval = SCORE_NONE;

    SearchStack() {};
    SearchStack(int ply) { this->ply = ply; };
};