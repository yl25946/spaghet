#pragma once

#include "defs.h"
#include "move.h"

class MoveList
{
public:
    std::array<Move, 256> moves;
    uint8_t count = 0;

    void insert(Move move);

    void print();
};