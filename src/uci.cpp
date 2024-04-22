#include "uci.h"

Move parse_move(const std::string &move_string, Board &board)
{
    uint8_t from_square = 8 * (8 - (move_string[1] - '0')) + (move_string[0] - 'a');
    uint8_t to_square = 8 * (8 - (move_string[3] - '0')) + (move_string[2] - 'a');
    uint8_t promotion_piece = move_string.size() > 4 ? colored_to_uncolored(char_pieces[move_string[4]]) : PAWN;
    uint8_t piece_moving = board.mailbox[from_square];
    uint8_t move_flag = promotion_piece != PAWN ? PROMOTION + promotion_piece - 1 : 0;

    // checks for double push
    if (board.mailbox[from_square] / 2 == PAWN && abs((int)from_square - (int)to_square) == 16)
        return Move(from_square, to_square, DOUBLE_PAWN_PUSH);

    // checks for an en-passant capture
    if (colored_to_uncolored(piece_moving) == PAWN && to_square == board.en_passant_square)
        return Move(from_square, to_square, EN_PASSANT_CAPTURE);

    // checks for castling
    if (colored_to_uncolored(piece_moving) == KING && abs((int)from_square - (int)to_square) != 1)
    {
        // checks if the king moves to the g file. If it is, that means we are king side castling
        return Move(from_square, to_square, to_square % 8 == 6 ? KING_CASTLE : QUEEN_CASTLE);
    }

    // checks for captures
    if (board.mailbox[to_square] != NO_PIECE)
        move_flag |= CAPTURES;

    return Move(from_square, to_square, move_flag);
}

Board &parse_position(const std::string &command)
{
}