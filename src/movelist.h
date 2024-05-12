#pragma once

#include "defs.h"
#include "move.h"
#include "transposition_table.h"

class MoveList
{
public:
    std::array<OrderedMove, 256> moves;
    uint8_t count = 0;
    // this is used to track the left pointer to swap with in selection sort
    uint8_t left_swap_index = 0;

    uint8_t size() const;

    // MoveList();

    // move order value defaults to 0
    void insert(uint8_t from, uint8_t to, uint8_t move_flag);
    void insert(uint8_t from, uint8_t to, uint8_t move_flag, uint16_t value);
    // scores all the moves
    void score(const Board &board, const TT_Entry &tt_entry, bool can_use_tt_entry);
    // gets the next move with the highest value
    Move nextMove();

    void print();
};