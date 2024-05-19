#pragma once

#include "defs.h";
#include "move.h"

class QuietHistory
{
public:
    int64_t butterfly_table[2][64][64];

    QuietHistory();

    void clear();

    // side_to_move is based on the side that is playing the move

    // inserts a move into the butterfly tables
    void insert(Move move, int depth, uint8_t side_to_move);
    uint16_t move_value(Move move, uint8_t side_to_move);
};

class Killer
{
public:
    Move killers[MAX_PLY][2];

    uint8_t count[MAX_PLY];

    Killer();

    void insert(Move move, int ply);

    // Move get_killer(int ply) const;;

    size_t size(int ply) const;
};