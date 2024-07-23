#pragma once

#include "defs.h"
#include "nnue.h"
#include "search_stack.h"

class ThreadData
{
public:
    std::vector<SearchStack> search_stack;
    std::vector<Accumulator> accumulators;
    QuietHistory main_history;
    PawnHistory pawnhist;
    CaptureHistory capthist;
    ContinuationHistory conthist;
    CorrectionHistory corrhist;

    ThreadData() : accumulators(MAX_PLY + 4)
    {
        for (int i = 0; i < MAX_PLY + 10; i++)
            search_stack.emplace_back(i - 4);
    };
};