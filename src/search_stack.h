#pragma once

#include "defs.h"
#include "history.h"
#include "movelist.h"

class SearchStack
{
public:
    bool null_moved = false;
    bool exclude_tt_move = false;
    int ply;
    Killers killers;
    // std::unique_ptr<ContinuationHistory> conthist = std::make_unique<ContinuationHistory>();
    MoveList pv;

    Board board;
    Move move_played = NO_MOVE;
    Move tt_move;

    SearchStack() {};
    SearchStack(int ply) { this->ply = ply; };
};