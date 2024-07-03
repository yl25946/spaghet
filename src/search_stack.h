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
    int ply;
    Killers killers;
    MoveList pv;

    Board board;
    Move move_played = NO_MOVE;
    Move tt_move;
    int static_eval;

    SearchStack() {};
    SearchStack(int ply) { this->ply = ply; };
};