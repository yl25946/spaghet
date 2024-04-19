#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    Board b("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    // b.make_move(Move(e1, g1, KING_CASTLE));

    // b.make_move(Move(c7, c6, QUIET_MOVE));

    // b.make_move(Move(a2, a3, QUIET_MOVE));

    // b.make_move(Move(g2, h1, KNIGHT_PROMOTION_CAPTURE));

    b.print();

    std::cout
        << perft_debug(b, 5, 5);

    // Board b("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

    // b.make_move(Move(c4, c5, CAPTURES));

    // b.make_move(Move(a8, b8, CAPTURES));

    // b.make_move(Move(a7, b8, ROOK_PROMOTION_CAPTURE));

    // b.make_move(Move(g2, h1, KNIGHT_PROMOTION_CAPTURE));

    // b.print();

    // std::cout
    //     << perft_debug(b, 6, 6);

    return 0;
}