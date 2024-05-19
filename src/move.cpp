#include "move.h"

// Move::Move()
// {
//     // info = 0;
// }
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

std::string Move::to_string()
{
    uint8_t flag = move_flag();
    std::string move_string = square_to_coordinate[from_square()];
    move_string += square_to_coordinate[to_square()];
    move_string += ((flag & 0b1000) ? ascii_pieces[2 * (flag & 0b11) + 3] : '\0');
    return move_string;
}

void Move::print()
{
    std::cout << to_string();
}

bool Move::is_quiet()
{
    return move_flag() & 12 == 0;
}

// OrderedMove::OrderedMove() : Move() {
//                              };

OrderedMove::OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag) : Move(from, to, move_flag)
{
    this->value = 0;
}

OrderedMove::OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag, int64_t value) : Move(from, to, move_flag)
{
    this->value = value;
}