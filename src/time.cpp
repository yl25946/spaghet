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
uint64_t Time::get_move_time(uint8_t side_to_move)
{
    if (has_depth)
        return UINT64_MAX;

    if (side_to_move == WHITE)
    {
        if (move_time)
            return get_time() + move_time - move_overhead;

        return get_time() + std::max((int64_t)(white_time * 0.054 + white_increment * 0.85) - (int64_t)move_overhead, 20LL);
    }
    else
    {
        if (move_time)
            return get_time() + move_time - move_overhead;

        return get_time() + std::max((int64_t)(black_time * 0.054 + black_increment * 0.85) - (int64_t)move_overhead, 20LL);
    }
}