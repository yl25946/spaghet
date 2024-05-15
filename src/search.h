#pragma once

#include "defs.h"
#include "board.h"
#include "eval.h"
#include "movelist.h"
#include "movegen.h"
#include "transposition_table.h"
#include "history.h"

constexpr uint64_t check_count = 4096;

// tracking the max depth across the engine
extern int max_depth;

class Searcher
{
public:
    // zorbrist hashes
    std::vector<uint64_t> threefold_repetition;

    // don't really change this lol, just for reference
    Board &board;

    TranspositionTable &transposition_table;

    QuietHistory history;
    Killer killers;

    // tracks how many times we've called "go" command to check age in TT
    uint32_t age;

    Move current_depth_best_move;
    // Move best_move;

    bool stopped = false;

    // start time of the current id so we can calculate nps
    uint64_t start_time;
    uint64_t end_time;

    int curr_depth = 0;

    // represents the number of nodes for a depths search
    uint64_t node_count;
    uint64_t total_nodes = 0;

    // Searcher();
    Searcher(Board &board, std::vector<Move> &move_list, TranspositionTable &transposition_table, uint32_t age);
    Searcher(Board &board, std::vector<Move> &move_list, TranspositionTable &transposition_table, uint32_t age, uint64_t end_time);
    // Searcher(Board &board, std::vector<Move> &move_list, uint64_t end_time, uint8_t max_depth);

    // uses iterative deepening
    void search();

    // returns true if board is in checkmate
    // bool is_checkmate(Board &board);
    int quiescence_search(Board &board, int alpha, int beta, int ply);
    int negamax(Board &board, int alpha, int beta, int depth, int ply, bool in_pv_node, bool null_moved);

    // checks if there's a threefold draw
    // returns true if there is a draw
    bool threefold(Board &board);

    void bench();
};