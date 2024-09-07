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
    void insert(Move move);
    void insert(uint8_t from, uint8_t to, uint8_t move_flag);
    void insert(uint8_t from, uint8_t to, uint8_t move_flag, uint16_t value);

    // gets the next move with the highest value
    void pop_back() { --count; }
    void clear() { count = 0; }
    void copy_over(MoveList &move_list);

    Move operator[](int index) const { return moves[index]; }

    std::string to_string();
    // iterates through the moves backwards (used for PV Tables)
    std::string reverse_to_string();
    void print();
};