#pragma once

#include "defs.h";
#include "move.h"

class QuietHistory
{
public:
    uint16_t butterfly_table[64][64];

    QuietHistory();

    void clear();
    // inserts a move into the butterfly tables
    void insert(Move move);
    uint16_t move_value();
};