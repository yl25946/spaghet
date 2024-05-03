#pragma once

#include "defs.h"
#include "board.h"
#include "eval.h"
#include "movelist.h"
#include "movegen.h"

const uint64_t check_count = 4096;

// tracking the max depth across the engine
extern int max_depth;

class Searcher
{
public:
    // zorbrist hashes
    std::vector<uint64_t> threefold_repetition;

    // don't really change this lol
    Board &board;

    Move current_depth_best_move;
    // Move best_move;

    bool stopped = false;

    // start time of the current id so we can calculate nps
    uint64_t start_time = 0;
    uint64_t end_time = 0;

    int curr_depth = 0;

    // represents the number of nodes for a depths search
    uint64_t current_depth_node_count = 0;
    uint64_t total_nodes = 0;

    // Searcher();
    Searcher(Board &board, std::vector<Move> &move_list);
    Searcher(Board &board, std::vector<Move> &move_list, uint64_t end_time);
    // Searcher(Board &board, std::vector<Move> &move_list, uint64_t end_time, uint8_t max_depth);

    // uses iterative deepening
    void search();

    // returns true if board is in checkmate
    // bool is_checkmate(Board &board);
    int quiescence_search(Board &board, int alpha, int beta, int ply);
    int negamax(Board &board, int alpha, int beta, int depth, int ply);

    // checks if there's a threefold draw
    // returns true if there is a draw
    bool threefold(Board &board);

    void bench();
};