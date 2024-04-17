#include "utils.h"

void print_bitboard(uint64_t bitboard)
{
    std::cout << "\n";

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            // convert file & rank into square index
            uint8_t square = rank * 8 + file;

            // prints ranks
            if (!file)
                std::cout << "  " << (8 - rank) << " ";

            // prints out a 1 if there is a 1 at the bit location, 0 otherwise
            std::cout
                << " " << (get_bit(bitboard, square) ? 1 : 0);
        }

        // add a new line every rank
        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n\n";

    std::cout << "Bitboard: " << bitboard << "\n";
}

// random number generator
std::mt19937_64 random_uint64_generator{seed};

uint64_t random_uint64()
{
    return random_uint64_generator();
}

uint64_t generate_magic_number()
{
    return random_uint64() & random_uint64() & random_uint64();
}

// bool isWhite(uint8_t piece)
// {
//     return !(piece & 1);
// }