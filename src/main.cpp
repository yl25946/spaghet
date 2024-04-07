#include "defs.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    // initializes the attack tables
    init_leapers_attacks();

    // print_bitboard((uint64_t)(random_U32()));
    // slices the first 16 bits
    print_bitboard(random_uint64());
    print_bitboard(generate_magic_number());

    return 0;
}