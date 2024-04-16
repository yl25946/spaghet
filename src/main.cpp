#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    Board b("8/8/8/3p4/8/8/8/8 w - - 0 0");
    b.print();
    print_bitboard(b.pieces[PAWN] & b.colors[BLACK]);
    print_bitboard(pawn_attacks[WHITE][e4]);


    return 0;
}