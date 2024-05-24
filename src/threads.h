#pragma once

#include "defs.h"
#include "search.h"
#include "history.h"
// #include "uci.h"

class Searcher;
class QuietHistory;
class UciOptions;

// used to initiate a search
void search(Searcher *searcher, UciOptions *options);

class ThreadData
{
public:
    Searcher &searcher;
    // Since this 2 tables need to be cleaned after each search we just initialize (and subsequently clean them) elsewhere
    // PvTable pvTable;
    // QuietHistory &history;
};

class Threads
{
public:
    UciOptions options;
    std::vector<std::thread> threads;
    std::vector<Searcher> thread_data;

    Threads(){};

    void insert(Searcher &searcher);

    void go();

    // forcefully terminates all the seacrh threads
    void terminate();
};
