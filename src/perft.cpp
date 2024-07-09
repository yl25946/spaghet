#include "perft.h"

uint64_t perft(Board &board, uint8_t depth)
{

    if (depth == 0)
        return 1ULL;

    MoveList move_list;
    uint64_t nodes = 0;

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
        Board test(board.fen());
        if (test.hash != board.hash)
            std::cout << board.fen() << " " << board.hash << " " << test.hash << "\n";
        if (test.pawn_hash != board.pawn_hash)
            std::cout << board.fen() << " " << board.hash << " " << test.hash << "\n";
        return 1ULL;
    }

    uint64_t total_nodes = 0;
    MoveList move_list;

    generate_moves(board, move_list);

    // TranspositionTable tt(8);

    // move_list.score(board, tt);

    MovePicker move_picker(move_list);

    while (move_picker.has_next())
    {
        Move move = move_picker.next_move();
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

void perft_driver(const std::string &fen, uint8_t depth)
{
    Board board(fen);

    // board.print();.

    std::cout << "\nPERFT: ";

    auto start = std::chrono::system_clock::now();

    uint64_t nodes = perft(board, depth);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << "Total Nodes Searched: " << nodes << "\nTotal Time: " << elapsed_seconds.count() << "\nNodes Per Second: " << nodes / (elapsed_seconds.count()) / 1000000 << " million nodes per second\n";
}

void perft_debug_driver(const std::string &fen, uint8_t depth)
{
    Board board(fen);

    // board.print();.

    std::cout << "PERFT: \n";

    auto start = std::chrono::system_clock::now();

    uint64_t nodes = perft_debug(board, depth, depth);

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << "Total Nodes Searched: " << nodes << "\nTotal Time: " << elapsed_seconds.count() << "\nNodes Per Second: " << nodes / (elapsed_seconds.count()) / 1000000 << " million nodes per second\n";
}

void captures_perft_driver(const std::string &fen, uint8_t depth)
{
}