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
    void insert(Move move, int depth);
    uint16_t move_value(Move move) const;
};

class Killer
{
public:
    Move killers[MAX_PLY][3];

    uint8_t count[MAX_PLY];

    Killer();

    void insert(Move move, int ply);

    // Move get_killer(int ply) const;;

    size_t size(int ply) const;
};