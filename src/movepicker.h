#pragma once

#include "defs.h"
#include "move.h"
#include "transposition_table.h"
#include "history.h"
#include "see.h"
#include "thread_data.h"
#include "movelist.h"
#include "search_stack.h"

class Killers;
class QuietHistory;

class MovePicker
{
public:
    MoveList &move_list;

    int quiet_moves = 0;
    int moves_remaining;

    bool skip_quiet_moves = false;

    int moves_picked = 0;
    int legals = 0;

    // this is used to track the left pointer to swap with in selection sort
    int left_swap_index = 0;

    MovePicker(MoveList &move_list);

    void skip_quiets()
    {
        moves_remaining -= quiet_moves;
        quiet_moves = 0;
        skip_quiet_moves = true;
    };

    // segfaults if there is no next move, it is up to the search to catch it
    OrderedMove next_move();

    void update_moves_seen() { ++moves_picked; };
    void update_legal_moves() { ++legals; };
    int moves_seen() { return moves_picked; };
    int legal_moves() { return legals; };

    bool has_next();

    void score(SearchStack *ss, ThreadData &thread_data, Move tt_move, bool has_tt_move, int threshold);
};