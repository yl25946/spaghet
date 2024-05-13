#pragma once

#include "defs.h";
#include "move.h"

class History
{
public:
    uint16_t butterfly_tables[64][64];

    void clear();
    // inserts a move into the butterfly tables
    void insert(Move move);
    uint16_t move_value();
};