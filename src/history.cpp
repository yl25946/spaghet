#include "history.h"
// #include "movepicker.h"

QuietHistory::QuietHistory()
{
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 64; ++k)
                butterfly_table[i][j][0][k][0] = butterfly_table[i][j][0][k][1] = butterfly_table[i][j][1][k][0] = butterfly_table[i][j][1][k][1] = 0;
}

void QuietHistory::clear()
{
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 64; ++j)
            for (int k = 0; k < 64; ++k)
                butterfly_table[i][j][0][k][0] = butterfly_table[i][j][0][k][1] = butterfly_table[i][j][1][k][0] = butterfly_table[i][j][1][k][1] = 0;
}

void QuietHistory::update()
{
    // for (int i = 0; i < 2; ++i)
    //     for (int j = 0; j < 64; ++j)
    //         for (int k = 0; k < 64; ++k)
    //             butterfly_table[i][j][k] /= 4;
}

void QuietHistory::update(const Board &board, Move move, int depth, bool good)
{
    uint8_t from_square = move.from_square();
    uint8_t to_square = move.to_square();
    bool from_square_attacked = board.is_square_attacked(from_square, board.side_to_move ^ 1);
    bool to_square_attacked = board.is_square_attacked(to_square, board.side_to_move ^ 1);

    const int delta = std::clamp(good ? 170 * depth : -450 * depth, -1500, 1500);

    // gravity formula
    butterfly_table[board.side_to_move][from_square][from_square_attacked][to_square][to_square_attacked] += delta - (static_cast<int64_t>(butterfly_table[board.side_to_move][from_square][from_square_attacked][to_square][to_square_attacked]) * abs(delta) / MAX_HISTORY);
}

void QuietHistory::update(const Board &board, MoveList &move_list, Move best_move, int depth)
{
    for (int i = 0; i < move_list.size(); ++i)
    {
        if (move_list[i] == best_move)
            update(board, move_list[i], depth, true);
        else
            update(board, move_list[i], depth, false);
    }
}

int64_t QuietHistory::move_value(const Board &board, Move move)
{
    uint8_t from_square = move.from_square();
    uint8_t to_square = move.to_square();
    bool from_square_attacked = board.is_square_attacked(from_square, board.side_to_move ^ 1);
    bool to_square_attacked = board.is_square_attacked(to_square, board.side_to_move ^ 1);

    return butterfly_table[board.side_to_move][from_square][from_square][to_square][to_square_attacked];
}

PawnHistory::PawnHistory()
{
    for (int i = 0; i < PAWNHIST_SIZE; ++i)
        for (int j = 0; j < 12; ++j)
            for (int k = 0; k < 64; ++k)
                table[i][j][k] = 0;
}

void PawnHistory::update(const Board &board, const MoveList &move_list, Move fail_high_move, int depth)
{
    for (int i = 0; i < move_list.size(); ++i)
    {
        if (move_list[i] == fail_high_move)
            update(board, move_list[i], depth, true);
        else
            update(board, move_list[i], depth, false);
    }
}

void PawnHistory::update(const Board &board, Move move, int depth, bool good)
{
    const uint8_t piece = board.mailbox[move.from_square()];
    const uint8_t to = move.to_square();
    const uint64_t pawn_hash_index = pawn_index(board);

    const int delta = std::clamp(good ? 170 * depth : -450 * depth, -1500, 1500);

    // gravity formula
    table[pawn_hash_index][piece][to] += delta - (static_cast<int64_t>(table[pawn_hash_index][piece][to]) * abs(delta) / MAX_HISTORY);
}

int64_t PawnHistory::move_value(const Board &board, Move move)
{
    return table[pawn_index(board)][board.mailbox[move.from_square()]][move.to_square()];
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
        if (move_list[i] == failed_high_move)
            update(board, move_list[i], depth, true);
        else
            update(board, move_list[i], depth, false);
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

    const int delta = std::clamp(good ? 170 * depth : -450 * depth, -1500, 1500);

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

    const int delta = std::clamp(good ? 170 * depth : -450 * depth, -1500, 1500);

    // gravity formula
    table[piece][to_square][previous_piece][previous_to_square] += delta - (static_cast<int64_t>(table[piece][to_square][previous_piece][previous_to_square]) * abs(delta) / MAX_HISTORY);
}

void ContinuationHistory::update(const Board &board, MoveList &move_list, Move best_move, const Board &previous_board, Move previous_move, int depth)
{
    for (int i = 0; i < move_list.size(); ++i)
    {
        if (move_list[i] == best_move)
            update(board, move_list[i], previous_board, previous_move, depth, true);
        else
            update(board, move_list[i], previous_board, previous_move, depth, false);
    }
}

int64_t ContinuationHistory::move_value(const Board &board, Move move, const Board &previous_board, Move previous_move)
{
    return table[board.mailbox[move.from_square()]][move.to_square()][previous_board.mailbox[previous_move.from_square()]][previous_move.to_square()];
}

CorrectionHistory::CorrectionHistory()
{
    for (int i = 0; i < CORRHIST_SIZE; ++i)
    {
        table[0][i] = table[1][i] = 0;
    }
}
void CorrectionHistory::update(const Board &board, int depth, int score, int static_eval)
{
    if (is_mate_score(score))
        return;

    const int delta = score - static_eval;
    const int bonus = std::clamp(delta * depth / 8, -CORRHIST_LIMIT / 4, CORRHIST_LIMIT / 4);
    const int hash_location = board.pawn_hash % CORRHIST_SIZE;

    table[board.side_to_move][hash_location] += bonus - (table[board.side_to_move][hash_location] * abs(bonus) / CORRHIST_LIMIT);
}

int CorrectionHistory::correct_eval(const Board &board, int uncorrected_static_eval)
{
    const int hash_location = board.pawn_hash % CORRHIST_SIZE;

    const int raw_correction = table[board.side_to_move][hash_location];
    const int correction = raw_correction * std::abs(raw_correction) / 5'000;

    return std::clamp(uncorrected_static_eval + correction, MIN_MATE_SCORE + 1, MAX_MATE_SCORE - 1);
}

void Killers::insert(Move move)
{
    // don't want to insert multiple of the same moves into killers
    if (move == killers[0])
        return;

    killers[1] = killers[0];
    killers[0] = move;
}