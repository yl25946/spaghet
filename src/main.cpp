#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    // define test bitboard
    uint64_t occupancy = 0ULL;

    set_bit(occupancy, b6);
    set_bit(occupancy, d6);
    set_bit(occupancy, f6);
    set_bit(occupancy, b4);
    set_bit(occupancy, g4);
    set_bit(occupancy, c3);
    set_bit(occupancy, d3);
    set_bit(occupancy, e3);

    print_bitboard(occupancy);

    print_bitboard(get_queen_attacks(d4, occupancy));

    return 0;
}