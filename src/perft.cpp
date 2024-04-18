#include "perft.h"

uint64_t perft(Board &board, uint8_t depth)
{
    MoveList move_list;
    uint64_t nodes = 0;

    if (depth == 0)
        return 1ULL;

    generate_moves(board, move_list);

    for (int i = 0; i < move_list.count; ++i)
    {
        Board copy = board;
        copy.make_move(move_list.moves[i]);
        if (!copy.was_legal())
            continue;
        nodes += perft(copy, depth - 1);
    }

    return nodes;
}

uint64_t perft_debug(Board &board, uint8_t depth, uint8_t start_depth)
{
    if (depth == 0)
    {
        return 1ULL;
    }

    uint64_t total_nodes = 0;
    MoveList move_list;

    generate_moves(board, move_list);

    for (int i = 0; i < move_list.count; ++i)
    {
        Move move = move_list.moves[i];
        Board copy = board;
        copy.make_move(move);
        if (!copy.was_legal())
            continue;

        uint64_t child_nodes = perft_debug(copy, depth - 1, start_depth);
        total_nodes += child_nodes;

        if (depth == start_depth)
        {
            move.print();
            std::cout << ": " << child_nodes << "\n";
        }
    }

    return total_nodes;
}