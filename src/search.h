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
    uint64_t start_time = 0;
    uint64_t end_time = 0;

    int max_depth = 255;
    int curr_depth = 0;

    // represents the number of nodes for a depths search
    uint64_t current_depth_node_count = 0;
    uint64_t total_nodes = 0;

    // Searcher();
    Searcher(Board &board, std::vector<Move> &move_list);
    Searcher(Board &board, std::vector<Move> &move_list, uint64_t end_time);
    Searcher(Board &board, std::vector<Move> &move_list, uint64_t end_time, uint8_t max_depth);

    // uses iterative deepening
    void search();

    int negamax(Board &board, uint8_t depth);

    bool threefold(uint64_t hash);

    void bench();
};