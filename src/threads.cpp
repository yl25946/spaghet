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
