#include "movelist.h"

void MoveList::insert(uint8_t from, uint8_t to, uint8_t move_flag)
{
    moves[count++] = OrderedMove(from, to, move_flag);
}

void MoveList::insert(uint8_t from, uint8_t to, uint8_t move_flag, uint16_t value)
{
    moves[count++] = OrderedMove(from, to, move_flag, value);
}

uint8_t MoveList::size() const
{
    return count;
}

void MoveList::print()
{
    for (int i = 0; i < count; ++i)
    {
        moves[i].print();
        std::cout << "\n";
    }
};