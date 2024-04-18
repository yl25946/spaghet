#include "move.h"

Move::Move(uint8_t from, uint8_t to, uint8_t move_flag)
{
    info = (move_flag << 12) | (from << 6) | to;
}

uint8_t Move::from_square()
{
    return (info >> 6) & 0b111111;
}

uint8_t Move::to_square()
{
    return info & 0b111111;
}

uint8_t Move::move_flag()
{
    return info >> 12;
}

void Move::print()
{
    uint8_t flag = move_flag();
    std::cout << square_to_coordinate[from_square()] << square_to_coordinate[to_square()] << ((flag & 0b1000) ? ascii_pieces[2 * (flag & 0b11) + 3] : '\0');
}