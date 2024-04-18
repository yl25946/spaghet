#include "movelist.h"

void MoveList::insert(Move move)
{
    moves[count++] = move;
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