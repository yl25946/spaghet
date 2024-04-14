#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    Board b(cmk_position);
    b.print();
    for (uint64_t num : b.colors)
    {
        print_bitboard(num);
    }
    // std::cout << square_to_coordinate[a8];

    return 0;
}