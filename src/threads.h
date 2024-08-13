#pragma once

#include "defs.h"
#include "search.h"
#include "history.h"
#include "search_stack.h"
#include "thread_data.h"
#include "uci_options.h"
#include "time.h"

class Searcher;
class QuietHistory;
class UciOptions;
class Time;

// used to initiate a search
void search(Searcher *searcher, UciOptions *options);

class ThreadManager
{
    // used for tracking which threads is the best
    std::vector<int> thread_tracker1;
    std::vector<int> thread_tracker2;

public:
    UciOptions options;
    std::vector<std::jthread> threads;
    std::vector<Searcher> searchers;
    std::vector<ThreadData> thread_data;
    bool searching = false;
    // pretty much only used for bench
    uint64_t nodes = 0;

    ThreadManager() : thread_data(options.threads)
    {
        thread_tracker1.reserve(MAX_THREADS);
        thread_tracker2.reserve(MAX_THREADS);
    }

    void go(Board &board, std::vector<Move> &move_list, Time &time);
    // selects the best move using thread voting
    Move get_best_move();
    // clears thread data and resizes to new number of threads
    void resize(int new_thread_count);
    void ucinewgame() { resize(thread_data.size()); }
    void resize_tt(int new_hash_size) { options.resize(new_hash_size); }

    uint64_t get_nodes();

    // only stops the search doesn't join the search threads, dirty stop
    void stop();
    // stops the input therad until we are done searching
    void join();
    // makes the input thread wait for all the searcher to stop
    void terminate();
};
