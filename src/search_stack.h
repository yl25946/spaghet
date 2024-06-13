#pragma once

#include "defs.h"
#include "history.h"
#include "movelist.h"

class SearchStack
{
public:
    bool in_pv_node = true;
    bool null_moved = false;
    int ply;
    Killers killers;
    ContinuationHistory conthist;
    MoveList pv;

    SearchStack() {};
    SearchStack(int ply) { this->ply = ply; };
};