#include "history.h"

QuietHistory::QuietHistory()
{
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 64; ++k)
                butterfly_table[i][j][k] = 0;
}

void QuietHistory::clear()
{
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 64; ++k)
                butterfly_table[i][j][k] = 0;
}

void QuietHistory::update()
{
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 64; ++k)
                butterfly_table[i][j][k] /= 4;
}

void QuietHistory::update(Move move, int depth, uint8_t side_to_move, bool good)
{
    uint8_t from_square = move.from_square();
    uint8_t to_square = move.to_square();

    // const int64_t updated_value = butterfly_table[side_to_move][from_square][to_square] + (good ? depth * depth : -depth * depth);

    // butterfly_table[side_to_move][from_square][to_square] = std::clamp(updated_value, -MAX_HISTORY, MAX_HISTORY);

    const int delta = good ? depth * depth : -depth * depth;

    // formula taken from ethereal
    butterfly_table[side_to_move][from_square][to_square] += delta - (butterfly_table[side_to_move][from_square][to_square] * abs(delta) / MAX_HISTORY);
}

void QuietHistory::update(const MoveList &move_list, Move best_move, int depth, uint8_t side_to_move)
{
    for (int i = 0; i < move_list.size(); ++i)
    {
        if (move_list.moves[i].info == best_move.info)
            update(move_list.moves[i], depth, side_to_move, true);
        else
            update(move_list.moves[i], depth, side_to_move, false);
    }
}

int64_t QuietHistory::move_value(Move move, uint8_t side_to_move)
{
    return butterfly_table[side_to_move][move.from_square()][move.to_square()];
}

Killers::Killers()
{
    for (int i = 0; i < MAX_PLY; ++i)
    {
        count[i] = 0;
    }
}

void Killers::insert(Move move, int ply)
{
    if (count[ply] >= 2)
    {
        std::swap(killers[ply][0], killers[ply][1]);
        killers[ply][1] = move;

        return;
    }

    killers[ply][count[ply]++] = move;
}

size_t Killers::size(int ply) const
{
    return count[ply];
}