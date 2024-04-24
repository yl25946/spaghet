#include "main.h"

int main(int argc, char *argv[])
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

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

    // perft_driver(start_position, 6);

    return 0;
}