#pragma once

#include "defs.h"
#include "board.h"
#include "eval.h"
#include "movelist.h"
#include "movegen.h"

const uint64_t check_count = 4096;

class Searcher
{
public:
    // zorbrist hashes
    std::vector<uint64_t> threefold_repetition;

    // don't really change this lol
    Board &board;

    Move best_move;

    bool stopped = false;

    // start time of the current id so we can calculate nps
    uint64_t start_time;
    uint64_t end_time;

    uint8_t max_depth = 255;
    uint8_t curr_depth;

    uint64_t node_count = 0;

    Searcher(Board &board, std::vector<Move> move_list, uint64_t end_time);
    Searcher(Board &board, std::vector<Move> move_list, uint64_t end_time, uint8_t max_depth);

    // uses iterative deepening
    void search();

    int negamax(Board &board, uint8_t depth);

    bool threefold(uint64_t hash);
};