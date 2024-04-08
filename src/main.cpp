#include "defs.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    uint64_t occupancy = 0ULL;
    set_bit(occupancy, c3);
    // set_bit(occupancy, e3);
    // set_bit(occupancy, g7);

    print_bitboard(occupancy);

    print_bitboard(get_bishop_attacks(d4, occupancy));

    // init_magic_numbers();

    return 0;
}