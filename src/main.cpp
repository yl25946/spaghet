#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    Board b("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Move m(e2, e4, DOUBLE_PAWN_PUSH);

    b.make_move(m);

    std::cout << b.was_legal();

    return 0;
}