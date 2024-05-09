#include "main.h"

int main(int argc, char *argv[])
{
    std::cout << NAME << "\n\n\n\n";

    // initialize precalculated lookup tables
    init_leapers_attacks();
    init_sliders_attacks();
    init_zobrist_tables();

    // initialize psqt
    init_pesto_tables();

    // runs the bench test if we have a bench command
    if (argc >= 2)
    {
        // std::cout << "|" << argv[1] << "|";
        if (!strcmp(argv[1], "bench"))
        {
            // creates a dummy searcher
            Board board(start_position);
            std::vector<Move> move_list;
            Searcher searcher(board, move_list, UINT64_MAX);

            searcher.bench();
            return 0;
        }
    }

    UCI_loop();

    // perft_driver(tricky_position, 5);

    // Board board(start_position);

    // std::vector<Move> move_list;

    // move_list.push_back(Move(g1, f3, 0));
    // move_list.push_back(Move(g8, f6, 0));
    // move_list.push_back(Move(f3, g1, 0));
    // move_list.push_back(Move(f6, g8, 0));
    // move_list.push_back(Move(g1, f3, 0));
    // move_list.push_back(Move(g8, f6, 0));
    // move_list.push_back(Move(f3, g1, 0));
    // move_list.push_back(Move(f6, g8, 0));
    // move_list.push_back(Move(g1, f3, 0));
    // move_list.push_back(Move(g8, f6, 0));
    // move_list.push_back(Move(f3, g1, 0));
    // move_list.push_back(Move(f6, g8, 0));

    // Searcher searcher(board, move_list);

    std::cout << searcher.threefold(board);

    // Board board("rnb1kbnr/2q5/1p3p1p/p3p1P1/P6p/BPP4P/3P1P2/RN1QKB1R b KQkq - 0 17");
    // Move move(h6, g5, CAPTURES);
    // board.make_move(move);
    // print_bitboard(board.bitboard(WHITE_PAWN));

    return 0;
}