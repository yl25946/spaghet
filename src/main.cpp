#include "main.h"

int main()
{
    std::cout << NAME << "\n\n\n\n";

    init_leapers_attacks();
    init_sliders_attacks();

    Board b(tricky_position);
    b.print();

    b.print_attacked_squares(WHITE);

    return 0;
}