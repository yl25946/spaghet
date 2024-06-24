#include "move.h"

// Move::Move()
// {
//     // info = 0;
// }
Move::Move(uint8_t from, uint8_t to, uint8_t move_flag)
{
    info = (move_flag << 12) | (from << 6) | to;
}

uint8_t Move::from_square() const
{
    return (info >> 6) & 0b111111;
}

uint8_t Move::to_square() const
{
    return info & 0b111111;
}

uint16_t Move::from_to() const
{
    return info & 0b111111111111;
}

uint8_t Move::move_flag() const
{
    return info >> 12;
}

std::string Move::to_string() const
{
    uint8_t flag = move_flag();
    std::string move_string = square_to_coordinate[from_square()];
    move_string += square_to_coordinate[to_square()];
    if (flag & PROMOTION)
        move_string += ascii_pieces[2 * (flag & 0b11) + 3];
    return move_string;
}

void Move::print() const
{
    std::cout << to_string();
}

bool Move::is_quiet() const
{
    return (move_flag() & 12) == 0;
}

bool Move::is_castle() const
{
    uint8_t move_flag = this->move_flag();

    return move_flag == MOVE_FLAG::KING_CASTLE || move_flag == MOVE_FLAG::QUEEN_CASTLE;
}

bool Move::is_promotion() const
{
    return move_flag() & PROMOTION;
}

bool Move::is_capture() const
{
    return move_flag() & MOVE_FLAG::CAPTURES;
}

uint8_t Move::promotion_piece() const
{
    return move_flag() % 4 + 1;
}

OrderedMove::OrderedMove(Move move) : Move(move) {};

OrderedMove::OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag) : Move(from, to, move_flag)
{
}

OrderedMove::OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag, int64_t score) : Move(from, to, move_flag)
{
    this->score = score;
}