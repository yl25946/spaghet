#include "history.h"
// #include "movepicker.h"

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

    const int delta = good ? 170 * depth : -450 * depth;

    // formula taken from ethereal
    butterfly_table[side_to_move][from_square][to_square] += delta - (static_cast<int64_t>(butterfly_table[side_to_move][from_square][to_square]) * abs(delta) / MAX_HISTORY);
}

void QuietHistory::update(MoveList &move_list, Move best_move, int depth, uint8_t side_to_move)
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

ContinuationHistory::ContinuationHistory()
{
    for (int i = 0; i < 13; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 13; ++k)
                for (int l = 0; l < 64; ++l)
                    table[i][j][k][l] = 0;
}

void ContinuationHistory::clear()
{
    for (int i = 0; i < 13; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 13; ++k)
                for (int l = 0; l < 64; ++l)
                    table[i][j][k][l] = 0;
}

void ContinuationHistory::update()
{
    for (int i = 0; i < 13; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 13; ++k)
                for (int l = 0; l < 64; ++l)
                    table[i][j][k][l] /= 4;
}

void ContinuationHistory::update(const Board &board, Move move, const Board &previous_board, Move previous_move, int depth, bool good)
{
    uint8_t piece = board.mailbox[move.from_square()];
    uint8_t to_square = move.to_square();
    uint8_t previous_piece = previous_board.mailbox[previous_move.from_square()];
    uint8_t previous_to_square = previous_move.to_square();

    // const int64_t updated_value = table[side_to_move][from_square][to_square] + (good ? depth * depth : -depth * depth);

    // table[side_to_move][from_square][to_square] = std::clamp(updated_value, -MAX_HISTORY, MAX_HISTORY);

    const int delta = good ? 170 * depth : -450 * depth;

    // formula taken from ethereal
    table[piece][to_square][previous_piece][previous_to_square] += delta - (static_cast<int64_t>(table[piece][to_square][previous_piece][previous_to_square]) * abs(delta) / MAX_HISTORY);
}

void ContinuationHistory::update(const Board &board, MoveList &move_list, Move best_move, const Board &previous_board, Move previous_move, int depth)
{
    for (int i = 0; i < move_list.size(); ++i)
    {
        if (move_list.moves[i].info == best_move.info)
            update(board, move_list.moves[i], previous_board, previous_move, depth, true);
        else
            update(board, move_list.moves[i], previous_board, previous_move, depth, false);
    }
}

int64_t ContinuationHistory::move_value(const Board &board, Move move, const Board &previous_board, Move previous_move)
{
    return table[board.mailbox[move.from_square()]][move.to_square()][previous_board.mailbox[previous_move.from_square()]][previous_move.to_square()];
}

void Killers::insert(Move move)
{
    if (count >= 2)
    {
        if (killers[0] == move)
            return;

        std::swap(killers[0], killers[1]);
        killers[1] = move;

        return;
    }

    killers[count++] = move;
}

size_t Killers::size() const
{
    return count;
}