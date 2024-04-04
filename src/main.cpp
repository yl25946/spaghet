#include "defs.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    // initializes the attack tables
    init_leapers_attacks();

    // mask piece attacks at given square
    uint64_t attack_mask = mask_bishop_attacks(d4);

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            int square = rank * 8 + file;

            std::cout << count_bits(mask_bishop_attacks(square)) << ", ";
        }

        std::cout << "\n";
    }

    std::cout << "\n";
    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            int square = rank * 8 + file;

            std::cout << count_bits(mask_rook_attacks(square)) << ", ";
        }

        std::cout << "\n";
    }

    return 0;
}