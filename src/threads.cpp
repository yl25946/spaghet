#include "threads.h"
#include "uci.h"

void search(Searcher *searcher, UciOptions *options)
{
    searcher->search();
}

uint64_t ThreadManager::get_nodes()
{
    uint64_t nodes = 0;

    for (Searcher &searcher : searchers)
        nodes += searcher.nodes;

    return nodes;
}

void ThreadManager::go(Board &board, std::vector<Move> &move_list, Time &time)
{
    // clears away the results from a previous search
    join();
    threads.clear();
    searchers.clear();

    searching = true;

    Searcher main_thread(board, move_list, options.transposition_table, thread_data[0], *this, options.age, true);
    time.set_time(main_thread);
    searchers.push_back(main_thread);

    // time manager used for non main threads, on non main threads we keep searching until we stop
    Time non_main_time("go depth 255");

    for (int i = 1; i < options.threads; ++i)
    {
        Searcher non_main_thread(board, move_list, options.transposition_table, thread_data[i], *this, options.age, false);
        non_main_time.set_time(non_main_thread);
        searchers.push_back(non_main_thread);
    }

    for (Searcher &searcher : searchers)
        threads.emplace_back(search, &searcher, &options);
}

Move ThreadManager::get_best_move()
{
    int best_thread = 0;

    // Step 1: Check for any mates
    bool has_mate_score = false;
    int fastest_mate = 0;
    for (size_t curr_thread = 0; curr_thread < searchers.size(); ++curr_thread)
    {
        Searcher &searcher = searchers[curr_thread];
        if (searcher.root_score > MAX_MATE_SCORE)
        {
            has_mate_score = true;
            if (searcher.root_score > fastest_mate)
            {
                fastest_mate = searcher.root_score;
                best_thread = curr_thread;
            }
        }
    }

    // return the move that gives the fastest mate
    if (has_mate_score)
        return searchers[best_thread].best_move;

    // if there is no mate, then we select the thread with the highest depth reached
    thread_tracker1.clear();

    int max_depth_reached = -1;
    for (size_t curr_thread = 0; curr_thread < searchers.size(); ++curr_thread)
    {
        Searcher &searcher = searchers[curr_thread];

        if (searcher.depth_reached > max_depth_reached)
        {
            thread_tracker1.clear();
            max_depth_reached = searcher.depth_reached;
            thread_tracker1.push_back(curr_thread);
        }
        else if (searcher.depth_reached == max_depth_reached)
            thread_tracker1.push_back(curr_thread);
    }

    // if there is only one thread, that means we have a unanimous decision
    if (thread_tracker1.size() == 1)
        return searchers[thread_tracker1[0]].best_move;

    // tiebreaker: if there are multiple threads with the same depth, we prioritize the thread with the longest pv
    thread_tracker2.clear();
    int longest_pv = -1;
    for (int i : thread_tracker1)
    {
        Searcher &searcher = searchers[i];
        if (searcher.root_pv.size() > longest_pv)
        {
            thread_tracker2.clear();
            max_depth_reached = searcher.depth_reached;
            thread_tracker2.push_back(i);
        }
        else if (searcher.root_pv.size() == longest_pv)
            thread_tracker2.push_back(i);
    }

    // final tiebreaker: choose the thread with the highest score (let's pray that none of the threads have the same score)
    int best_score = -INF;
    for (int i : thread_tracker2)
    {
        Searcher &searcher = searchers[i];
        if (searcher.root_score > best_score)
        {
            best_score = searcher.root_score;
            best_thread = i;
        }
    }

    return searchers[best_thread].best_move;
}

void ThreadManager::stop()
{
    if (!searching)
        return;

    nodes = get_nodes();

    for (Searcher &searcher : searchers)
        searcher.stopped = true;

    searching = false;
}

void ThreadManager::join()
{
    if (!searching)
        return;

    nodes = get_nodes();

    for (std::jthread &thread : threads)
        thread.join();

    searching = false;
}

void ThreadManager::terminate()
{
    if (!searching)
        return;

    nodes = get_nodes();

    for (Searcher &searcher : searchers)
        searcher.stopped = true;

    for (std::jthread &thread : threads)
        thread.join();

    searching = false;
}

void ThreadManager::resize(int new_thread_count)
{
    terminate();
    thread_data.clear();
    thread_data.resize(new_thread_count);
    options.threads = new_thread_count;
}
