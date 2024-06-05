#pragma once

#include "defs.h"
#include "move.h"
#include "transposition_table.h"
#include "history.h"
#include "see.h"

class Killers;
class QuietHistory;

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
    // scores all the moves
    // void score(const Board &board, TranspositionTable &transposition_table, QuietHistory &history, Killers &killers, int threshold, int ply);
    // gets the next move with the highest value
    void pop_back() { --count; }
    void clear() { count = 0; }
    void copy_over(MoveList &move_list);

    std::string to_string();
    // iterates through the moves backwards (used for PV Tables)
    std::string reverse_to_string();
    void print();
};

class MovePicker
{
public:
    MoveList &move_list;

    bool skip_quiet_moves = false;
    bool has_next = true;

    int moves_picked = 0;

    // this is used to track the left pointer to swap with in selection sort
    int left_swap_index = 0;

    MovePicker(MoveList &move_list) : move_list(move_list) { this->move_list = move_list; };

    void skip_quiets() { skip_quiet_moves = true; };

    // returns gibberish if there isn't a next move, it is up to the search to catch it
    OrderedMove next_move();

    void update_moves_seen() { ++moves_picked; };

    int moves_seen() { return moves_picked; };

    bool has_next_move() { return this->has_next; };

    void score(const Board &board, TranspositionTable &transposition_table, QuietHistory &history, Killers &killers, int threshold, int ply);
};