#include "movelist.h"

void MoveList::insert(Move move)
{
    moves[count++] = OrderedMove(move);
}

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

std::string MoveList::to_string()
{
    std::string movelist_string = moves[0].to_string();

    for (int i = 1; i < size(); ++i)
    {
        movelist_string += " " + moves[i].to_string();
    }

    return movelist_string;
}

std::string MoveList::reverse_to_string()
{
    std::string movelist_string = moves[count - 1].to_string();

    for (int i = count - 2; i >= 0; --i)
    {
        movelist_string += " " + moves[i].to_string();
    }

    return movelist_string;
}

void MoveList::print()
{
    for (int i = 0; i < count; ++i)
    {
        moves[i].print();
        std::cout << "\n";
    }
};

void MoveList::copy_over(MoveList &move_list)
{
    for (int i = 0; i < move_list.size(); ++i)
    {
        insert(move_list[i]);
    }
}