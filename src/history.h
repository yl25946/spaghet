#pragma once

#include "defs.h";
#include "move.h"

class QuietHistory
{
public:
    uint16_t butterfly_table[2][64][64];

    QuietHistory();

    void clear();

    // side_to_move is based on the side that is playing the move

    // inserts a move into the butterfly tables
    void insert(Move move, int depth, uint8_t side_to_move);
    uint16_t move_value(Move move, uint8_t side_to_move);
};