#include "defs.h"

int main()
{
    std::cout << NAME << "\n";

    // initializes the attack tables
    init_leapers_attacks();

    // mask piece attacks at given square
    uint64_t attack_mask = mask_bishop_attacks(d4);

    // init occupancy
    for (int i = 0; i < 4096; ++i)
    {
        uint64_t occupancy = set_occupancy(i, count_bits(attack_mask), attack_mask);
        print_bitboard(occupancy);
    }

    return 0;
}