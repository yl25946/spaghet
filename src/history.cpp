#include "history.h"
// #include "movepicker.h"

PawnCorrectionHistory::PawnCorrectionHistory()
{

    for (int k = 0; k < PAWN_CORRHIST_SIZE; ++k)
        table[0][k] = table[1][k] = 0;
}

void PawnCorrectionHistory::update(const Board &board, int depth, int score, int static_eval)
{
    if (is_mate_score(score))
        return;

    const int delta = score - static_eval;
    const int bonus = std::clamp(delta * depth / 8, -CORRHIST_LIMIT / 4, CORRHIST_LIMIT / 4);
    const int hash_location = board.pawn_hash % PAWN_CORRHIST_SIZE;

    table[board.side_to_move][hash_location] += bonus - (table[board.side_to_move][hash_location] * abs(bonus) / CORRHIST_LIMIT);
}

int PawnCorrectionHistory::correction(const Board &board)
{
    const int hash_location = board.pawn_hash % PAWN_CORRHIST_SIZE;

    const int white_king_square = lsb(board.pieces[BITBOARD_PIECES::KING] & board.colors[WHITE]);
    const int black_king_square = lsb(board.pieces[BITBOARD_PIECES::KING] & board.colors[BLACK]);

    const int raw_correction = table[board.side_to_move][hash_location];
    const int correction = raw_correction * std::abs(raw_correction) / 5'000;

    return correction;
}

MaterialCorrectionHistory::MaterialCorrectionHistory()
{
    for (size_t i = 0; i < MATERIAL_CORRHIST_SIZE; ++i)
    {
        table[0][i] = table[1][i] = 0;
    }
}

void MaterialCorrectionHistory::update(const Board &board, int depth, int score, int static_eval)
{
    if (is_mate_score(score))
        return;

    const int delta = score - static_eval;
    const int bonus = std::clamp(delta * depth / 8, -CORRHIST_LIMIT / 4, CORRHIST_LIMIT / 4);
    const int hash_location = board.material_hash % MATERIAL_CORRHIST_SIZE;

    table[board.side_to_move][hash_location] += bonus - (table[board.side_to_move][hash_location] * abs(bonus) / CORRHIST_LIMIT);
}

int MaterialCorrectionHistory::correction(const Board &board)
{
    const int hash_location = board.material_hash % MATERIAL_CORRHIST_SIZE;

    const int raw_correction = table[board.side_to_move][hash_location];
    const int correction = raw_correction * std::abs(raw_correction) / 5'000;

    return correction;
}