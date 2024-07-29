#pragma once

#include "defs.h"
#include "search.h"

constexpr uint64_t MOVE_OVERHEAD = 16;
constexpr int64_t min_move_time = 20;

class Searcher;

class Time
{
public:
    int64_t white_time = 0;
    int64_t black_time = 0;
    int64_t white_increment = 0;
    int64_t black_increment = 0;

    int max_depth = 255;
    uint64_t nodes = 0;

    bool has_depth = false;
    bool has_nodes = false;

    uint64_t move_time = 0;

    // go depth 6 wtime 180000 btime 100000 binc 1000 winc 100 movetime 1000 movestogo 40
    Time(const std::string &go_command);

    // archaeic, no one uses this
    int moves_to_go;

    // will give you the time in milliseconds of when the search should end, and subtracts the buffer from the acutal time to account for computation time
    void set_time(Searcher &searcher);
};