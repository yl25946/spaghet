#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    Board b("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    b.make_move(Move(d5, d6, QUIET_MOVE));

    b.make_move(Move(h3, g2, CAPTURES));

    b.make_move(Move(a2, a3, QUIET_MOVE));

    std::cout
        << perft_debug(b, 1, 1);

    return 0;
}