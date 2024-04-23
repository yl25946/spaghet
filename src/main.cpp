#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    // UCI_loop();

    std::vector<Move> move;
    Board b(start_position);
    Searcher searcher(b, move, get_time() + 1000);
    searcher.search();

    return 0;
}