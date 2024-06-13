#pragma once

#include "defs.h"
#include "board.h"
#include "eval.h"
#include "movepicker.h"
#include "movegen.h"
#include "transposition_table.h"
#include "threads.h"
#include "see.h"
// #include "history.h"

class QuietHistory;
class Killers;

constexpr uint64_t check_count = 4096;

// tracking the max depth across the engine
extern int max_depth;

class SearchStack
{
public:
    bool in_pv_node = true;
    bool null_moved = false;
    int ply;
    Killers killers;
    MoveList pv;

    SearchStack() {};
    SearchStack(int ply) { this->ply = ply; };
};

class Searcher
{
public:
    // zorbrist hashes
    std::vector<uint64_t> game_history;

    // will play all the moves in the movelist
    Board &board;

    TranspositionTable &transposition_table;

    QuietHistory &history;

    // tracks how many times we've called "go" command to check age in TT
    uint32_t age;

    Move current_depth_best_move;
    // Move best_move;

    bool stopped = false;

    // start time of the current id so we can calculate nps
    uint64_t start_time;
    // if it exceeds this time and it's at the end of the search, we cut it
    uint64_t optimum_stop_time;
    uint64_t optimum_stop_time_duration;
    // we force the search to stop at this time
    uint64_t max_stop_time;
    uint64_t max_stop_time_duration;

    // if we're playing on btime, wtime, binc, winc commands
    bool time_set = false;

    // current deth for iterative deepening
    int curr_depth = 0;

    // use for tracking seldepth
    int seldepth = 0;

    std::array<SearchStack, MAX_PLY + 10> search_stack;

    // used for tracking aspiration window size
    int average_score = -INF;
    // bool increase_depth = true;

    // std::vector<MoveList> pv;

    // represents the number of nodes for a depths search
    uint64_t nodes;
    // uint64_t total_nodes = 0;

    std::array<uint64_t, 64 * 64> nodes_spent_table;

    // Searcher();
    Searcher(Board &board, std::vector<Move> &move_list, TranspositionTable &transposition_table, QuietHistory &history, uint32_t age);

    // creates a hard time limit
    Searcher(Board &board, std::vector<Move> &move_list, TranspositionTable &transposition_table, QuietHistory &history, uint32_t age, uint64_t end_time);
    // Searcher(Board &board, std::vector<Move> &move_list, uint64_t end_time, uint8_t max_depth);

    // bool SEE(const Board &board, Move move, int threshold);

    // uses iterative deepening
    void search();

    // returns true if board is in checkmate
    // bool is_checkmate(Board &board);
    int quiescence_search(Board &board, int alpha, int beta, SearchStack *ss, bool in_pv_node);
    int negamax(Board &board, int alpha, int beta, int depth, SearchStack *ss, bool in_pv_node, bool null_moved);

    // checks if there's a threefold draw
    // returns true if there is a draw
    bool twofold(Board &board);

    void scale_time(int best_move_stability_factor);

    // void bench();
};