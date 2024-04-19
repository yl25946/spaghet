#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    perft_driver(tricky_position, 6);

    return 0;
}