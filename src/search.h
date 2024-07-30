#pragma once

#include "defs.h"
#include "board.h"
#include "eval.h"
#include "movepicker.h"
#include "movegen.h"
#include "transposition_table.h"
#include "threads.h"
#include "see.h"
#include "movelist.h"
#include "search_stack.h"
#include "thread_data.h"
// #include "history.h"

class QuietHistory;
class Killers;
class ThreadManager;

constexpr uint64_t check_count = 4096;

constexpr bool PV = true;
constexpr bool nonPV = false;

class Searcher
{
public:
    // zorbrist hashes
    std::vector<uint64_t> game_history;

    // will play all the moves in the movelist
    Board board;

    TranspositionTable &transposition_table;

    ThreadData &thread_data;

    ThreadManager &thread_manager;

    // tracks how many times we've called "go" command to check age in TT
    uint32_t age;

    bool stopped = false;
    bool is_main_thread;

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
    bool nodes_set = false;
    int max_depth = 255;

    // current deth for iterative deepening
    int curr_depth = 0;

    // use for tracking seldepth
    int seldepth = 0;

    // used for tracking aspiration window size
    int average_score = -INF;
    // bool increase_depth = true;

    // represents the number of nodes for a depths search
    uint64_t nodes = 0;
    uint64_t max_nodes = UINT64_MAX;

    std::array<uint64_t, 64 * 64> nodes_spent_table;

    Searcher(Board board, const std::vector<Move> &move_list, TranspositionTable &transposition_table, ThreadData &thread_data, ThreadManager &thread_manager, uint32_t age, bool is_main_thread);

    // uses iterative deepening
    void search();

    // returns true if board is in checkmate
    // bool is_checkmate(Board &board);
    template <bool inPV>
    int quiescence_search(int alpha, int beta, SearchStack *ss);
    template <bool inPV>
    int negamax(int alpha, int beta, int depth, bool cutnode, SearchStack *ss);

    void update_conthist(SearchStack *ss, MoveList &quiet_moves, Move fail_high_move, int depth);

    // checks if there's a threefold draw
    // returns true if there is a draw
    bool twofold(Board &board);

    void scale_time(int best_move_stability_factor);
};