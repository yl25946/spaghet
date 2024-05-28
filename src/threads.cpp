#include "threads.h"
#include "uci.h"

void search(Searcher *searcher, UciOptions *options)
{
    searcher->search();
}

Threads::Threads(UciOptions &options) : options(options)
{
    this->options = options;
}

void Threads::insert(Searcher &searcher)
{
    thread_data.push_back(searcher);
}

void Threads::go()
{
    for (Searcher &searcher : thread_data)
        threads.push_back(std::thread(search, &searcher, &options));
}

void Threads::terminate()
{
    for (int i = 0; i < threads.size(); ++i)
        thread_data[i].stopped = true;

    for (int i = 0; i < threads.size(); ++i)
        threads[i].join();

    threads.clear();
    thread_data.clear();
}
