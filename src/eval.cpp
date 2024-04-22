#include "eval.h"

// just counts the pieces and adds them up
uint16_t piece_value[6] = {
    100, 300, 350, 500, 900, 0};

int piece_count_evaluation(Board &board)
{
    int eval = 0;
    for (uint8_t piece = PAWN; piece <= QUEEN; ++piece)
    {
        eval += piece_value[piece] * count_bits(board.bitboard(2 * piece + board.side_to_move));
        eval -= piece_value[piece] * count_bits(board.bitboard(2 * piece + (board.side_to_move ^ 1)));
    }

    return eval;
}

int evaluate(Board &board)
{
    return piece_count_evaluation(board);
}