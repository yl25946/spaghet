#pragma once

#include "defs.h"
#include "search_stack.h"

class ThreadData
{
public:
    std::vector<SearchStack> search_stack;
    QuietHistory main_history;
    ContinuationHistory conthist;
    Countermove countermove;

    ThreadData()
    {
        for (int i = 0; i < MAX_PLY + 10; i++)
            search_stack.emplace_back(i - 4);
    };
};