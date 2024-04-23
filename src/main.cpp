#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    // UCI_loop();

    std::vector<Move> move;
    Board b(start_position);
    Searcher searcher(b, move, get_time() + 10000);
    searcher.bench();

    // perft_driver(tricky_position, 6);

    return 0;
}