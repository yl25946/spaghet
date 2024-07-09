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

CaptureHistory::CaptureHistory()
{
    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 7; ++k)
                table[i][j][k] = 0;
}

void CaptureHistory::clear()
{
    for (int i = 0; i < 12; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 7; ++k)
                table[i][j][k] = 0;
}

void CaptureHistory::update(const Board &board, MoveList &move_list, Move failed_high_move, int depth)
{
    for (int i = 0; i < move_list.size(); ++i)
    {
        if (move_list.moves[i].info == failed_high_move.info)
            update(board, move_list.moves[i], depth, true);
        else
            update(board, move_list.moves[i], depth, false);
    }
}

int64_t CaptureHistory::move_value(const Board &board, Move move)
{
    uint8_t to = move.to_square();
    uint8_t uncolored_captured_piece = colored_to_uncolored(board.mailbox[to]);

    return table[board.mailbox[move.from_square()]][to][uncolored_captured_piece];
}

void CaptureHistory::update(const Board &board, Move move, int depth, bool good)
{
    uint8_t from_square = move.from_square();
    uint8_t to_square = move.to_square();
    uint8_t capturing_piece = board.mailbox[from_square];
    uint8_t uncolored_captured_piece = colored_to_uncolored(board.mailbox[to_square]);

    // const int64_t updated_value = butterfly_table[side_to_move][from_square][to_square] + (good ? depth * depth : -depth * depth);

    // butterfly_table[side_to_move][from_square][to_square] = std::clamp(updated_value, -MAX_HISTORY, MAX_HISTORY);

    const int delta = good ? 170 * depth : -450 * depth;

    // formula taken from ethereal
    table[capturing_piece][to_square][uncolored_captured_piece] += delta - (static_cast<int64_t>(table[capturing_piece][to_square][uncolored_captured_piece]) * abs(delta) / MAX_HISTORY);
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

CorrectionHistory::CorrectionHistory()
{
    for (int i = 0; i < table.size(); ++i)
    {
        table[0][i][0] = 0;
        table[0][i][1] = 0;
        table[1][i][0] = 0;
        table[1][i][1] = 0;
    }
}

void CorrectionHistory::update(const Board &board, int score, int static_eval)
{
    if (is_mate_score(score))
        return;

    const int delta = score - static_eval;
    const int hash_location = board.pawn_hash % table.size();

    table[board.side_to_move][hash_location][0] += std::clamp(delta, -256, 256);

    // update how many times we've updated to the respective corrhist entry
    ++table[board.side_to_move][hash_location][1];
}

int CorrectionHistory::corrected_eval(const Board &board, int static_eval)
{
    const int hash_location = board.pawn_hash % table.size();

    return std::clamp<int>(static_eval + table[board.side_to_move][hash_location][0] / (table[board.side_to_move][hash_location][1] == 0 ? 1 : table[board.side_to_move][hash_location][1]), MIN_MATE_SCORE, MAX_MATE_SCORE);
}

void Killers::insert(Move move)
{
    // don't want to insert multiple of the same moves into killers
    if (move == killers[0])
        return;

    killers[1] = killers[0];
    killers[0] = move;
}