#pragma once

#include "defs.h"
#include "search.h"
#include "history.h"
#include "search_stack.h"
// #include "uci.h"

class Searcher;
class QuietHistory;
class UciOptions;

// used to initiate a search
void search(Searcher *searcher, UciOptions *options);

class Threads
{
public:
    UciOptions &options;
    std::vector<std::thread> threads;
    std::vector<Searcher> thread_data;

    Threads(UciOptions &options);

    void insert(Searcher &searcher);

    void go();

    // forcefully terminates all the seacrh threads
    void terminate();
};
