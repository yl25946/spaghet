#include "time.h"

Time::Time(const std::string &go_command)
{
    size_t end_line;

    // parse depth command
    size_t go_pt = go_command.find("depth");
    if (go_pt != std::string::npos)
    {
        go_pt += 6;
        end_line = go_command.find(" ", go_pt);
        max_depth = stoi(go_command.substr(go_pt, end_line - go_pt));
        has_depth = true;
    }
    else
    {
        max_depth = 255;
    }

    go_pt = go_command.find("wtime");
    if (go_pt != std::string::npos)
    {
        go_pt += 6;
        end_line = go_command.find(" ", go_pt);
        white_time = stoi(go_command.substr(go_pt, end_line - go_pt));
        if (white_time < 0)
            white_time = 1000;
    }

    go_pt = go_command.find("btime");
    if (go_pt != std::string::npos)
    {
        go_pt += 6;
        end_line = go_command.find(" ", go_pt);
        black_time = stoi(go_command.substr(go_pt, end_line - go_pt));
        if (black_time < 0)
            black_time = 1000;
    }

    go_pt = go_command.find("winc");
    if (go_pt != std::string::npos)
    {
        go_pt += 5;
        end_line = go_command.find(" ", go_pt);
        white_increment = stoi(go_command.substr(go_pt, end_line - go_pt));
    }
    go_pt = go_command.find("binc");
    if (go_pt != std::string::npos)
    {
        go_pt += 5;
        end_line = go_command.find(" ", go_pt);
        black_increment = stoi(go_command.substr(go_pt, end_line - go_pt));
    }

    go_pt = go_command.find("movetime");
    if (go_pt != std::string::npos)
    {
        go_pt += 9;
        end_line = go_command.find(" ", go_pt);
        move_time = stoi(go_command.substr(go_pt, end_line - go_pt));
    }
}

// time controls yoinked from Alexandria
void Time::set_time(Searcher &searcher)
{
    searcher.start_time = get_time();

    if (has_depth)
    {
        searcher.optimum_stop_time_duration = UINT64_MAX;
        searcher.max_stop_time_duration = UINT64_MAX;
        searcher.optimum_stop_time = UINT64_MAX;
        searcher.max_stop_time = UINT64_MAX;

        return;
    }

    if (move_time)
    {
        searcher.optimum_stop_time_duration = move_time - MOVE_OVERHEAD;
        searcher.max_stop_time_duration = move_time - MOVE_OVERHEAD;
        searcher.optimum_stop_time = searcher.start_time + move_time - MOVE_OVERHEAD;
        searcher.max_stop_time = searcher.start_time + move_time - MOVE_OVERHEAD;

        return;
    }

    searcher.time_set = true;

    uint64_t base_time;
    uint64_t max_time_bound;
    // if we only recieved binc and winc commands
    if (searcher.board.side_to_move == WHITE)
    {
        base_time = white_time * 0.054 + white_increment * 0.85;
        max_time_bound = 0.76 * white_time;
    }
    else
    {

        base_time = black_time * 0.054 + black_increment * 0.85;
        max_time_bound = 0.76 * black_time;
    }

    const uint64_t optimum_time = std::min<uint64_t>(0.86 * base_time, max_time_bound);
    const uint64_t max_time = std::min<uint64_t>(3.04 * base_time, max_time_bound);

    searcher.optimum_stop_time = get_time() + optimum_time - MOVE_OVERHEAD;
    searcher.optimum_stop_time_duration = optimum_time - MOVE_OVERHEAD;
    searcher.max_stop_time = get_time() + max_time - MOVE_OVERHEAD;
    searcher.max_stop_time_duration = max_time - MOVE_OVERHEAD;
}