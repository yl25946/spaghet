#include "search.h"

int Searcher::negamax(Board &board, uint8_t depth)
{
    ++node_count;

    if (node_count & 4095)

        if (get_time() >= end_time)
        {
            stopped = true;
            return 0;
        }

    if (depth == 0)
        return evaluate(board);

    MoveList moves;

    generate_moves(board, moves);

    uint8_t legal_moves = 0;

    int best_eval = INT32_MIN;
    Move best_move;

    for (int i = 0; i < moves.size(); ++i)
    {
        Board copy = board;
        Move curr_move = moves.moves[i];
        copy.make_move(curr_move);

        if (!copy.was_legal())
            continue;

        ++legal_moves;

        int current_eval = negamax(copy, depth - 1);

        if (stopped)
            return 0;

        if (current_eval > best_eval)
        {
            best_eval = current_eval;
            best_move = curr_move;
        }
    }

    if (legal_moves == 0)
    {
        if (board.is_in_check())
            return -50000;
        else
            return 0;
    }
    // write the best move down at the current depth
    else if (depth == curr_depth)
    {
        this->best_move = best_move;
    }

    return best_eval;
}

void Searcher::search()
{
    int best_score;
    for (uint8_t current_depth = 1; current_depth <= max_depth; ++current_depth)
    {
        this->curr_depth = current_depth;
        node_count = 0;

        Board copy = board;

        best_score = negamax(copy, current_depth);

        if (stopped)
            break;

        printf("info score cp %d depth %d nodes %ld time %d\n", best_score, current_depth, node_count, get_time() - start_time);
    }

    printf("bestmove %s\n", best_move.to_string());
}