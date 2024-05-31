#pragma once

#include "defs.h"
#include "move.h"
// #include "movelist.h"

class MoveList;

class QuietHistory
{
public:
    std::vector<std::array<std::array<int64_t, 64>, 64>> butterfly_table;

    QuietHistory();

    void clear();

    // call this after completing a search or before the another search
    // halves the values in the table
    void update();

    // side_to_move is based on the side that is playing the move

    // inserts a move into the butterfly tables
    void update(Move move, int depth, uint8_t side_to_move, bool good);
    void update(MoveList &move_list, Move best_move, int depth, uint8_t side_to_move);
    int64_t move_value(Move move, uint8_t side_to_move);
};

class Killers
{
public:
    std::vector<std::array<Move, 2>> killers;

    std::vector<uint8_t> count;

    Killers() : killers(MAX_PLY + 1), count(MAX_PLY + 1, 0) {};

    void insert(Move move, int ply);

    // Move get_killer(int ply) const;;

    size_t size(int ply) const;
};