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
            TranspositionTable dummy(16);
            QuietHistory history;
            Searcher searcher(board, move_list, dummy, history, 0, UINT64_MAX);
            searcher.bench();
            return 0;
        }
    }

    // SEE_suite();

    UCI_loop();

    // perft_driver(tricky_position, 5);

    // Board board(start_position);

    // Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // std::cout << perft_debug(board, 4, 4);

    // Board board("rnbqkb1r/ppp1pppp/5n2/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
    // print_bitboard(board.attackers(d5, WHITE));

    // Board board("rnbqkb1r/ppp1pppp/5n2/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
    // std::vector<Move> move_list;
    // TranspositionTable dummy(16);
    // QuietHistory history;
    // Searcher searcher(board, move_list, dummy, history, 0, UINT64_MAX);
    // std::cout << (searcher.SEE(board, Move(e5, d6, MOVE_FLAG::EN_PASSANT_CAPTURE), 0) ? "true" : "false");

    return 0;
}