#include "defs.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();

    for (int square = 0; square < 64; ++square)
    {
        printf("0x%llxULL,\n", bishop_magic_numbers[square]);
    }

    return 0;
}