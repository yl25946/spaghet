#pragma once

#include "defs.h"
#include "move.h"
#include "movelist.h"

class MoveList;

class QuietHistory
{
public:
    int64_t butterfly_table[2][64][64];

    QuietHistory();

    void clear();

    // call this after completing a search or before the another search
    // halves the values in the table
    void update();

    // side_to_move is based on the side that is playing the move

    // inserts a move into the butterfly tables
    void update(Move move, int depth, uint8_t side_to_move, bool good);
    void update(const MoveList &move_list, Move best_move, int depth, uint8_t side_to_move);
    int64_t move_value(Move move, uint8_t side_to_move);
};

class Killers
{
public:
    Move killers[MAX_PLY][2];

    uint8_t count[MAX_PLY];

    Killers();

    void insert(Move move, int ply);

    // Move get_killer(int ply) const;;

    size_t size(int ply) const;
};