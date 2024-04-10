#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    uint64_t occupancy = 0ULL;
    set_bit(occupancy, c5);
    set_bit(occupancy, f2);
    set_bit(occupancy, g7);
    set_bit(occupancy, b2);
    set_bit(occupancy, g5);
    set_bit(occupancy, e2);
    set_bit(occupancy, e7);

    print_bitboard(occupancy);

    print_bitboard(get_bishop_attacks(d4, occupancy));

    print_bitboard(get_rook_attacks(e5, occupancy));

    // init_magic_numbers();

    return 0;
}