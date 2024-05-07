#pragma once

#include "defs.h"
#include "move.h"

class MoveList
{
public:
    std::array<OrderedMove, 256> moves;
    uint8_t count = 0;

    uint8_t size() const;

    // MoveList();

    // move order value defaults to 0
    void insert(uint8_t from, uint8_t to, uint8_t move_flag);
    void insert(uint8_t from, uint8_t to, uint8_t move_flag, uint16_t value);
    // gets the next move with the highest value
    Move nextMove();

    void print();
};