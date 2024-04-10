#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    Board b;
    b.pieces[pawn] = 0ULL;
    set_bit(b.pieces[pawn], e2);

    print_bitboard(b.pieces[pawn]);

    std::cout << "piece: " << ascii_pieces[char_pieces['P']];
    // init_magic_numbers();

    return 0;
}