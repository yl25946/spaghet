#include "board.h"

Board::Board(const std::string &fen)
{
    // clears the entire board
    board.fill(NO_PIECE);
    pieces.fill(0);
    colors.fill(0);

    int char_it = 0;
    uint8_t square = 0;
    // we break this whenever we get a space
    while (fen[char_it] != ' ')
    {
        // if it is a piece name
        if (std::isalpha(fen[char_it]))
        {
            int piece = char_pieces[fen[char_it]];
            // initialize the bitboard
            // divide by two since we aren't keeping track of color
            set_bit(pieces[piece / 2], square);
            // if piece & 1 = 1, then that means it is the black
            set_bit(colors[piece & 1], square);

            // initialize the mailbox
            board[square] = piece;

            ++square;
            ++char_it;
        }
        // if it's a space
        else if (std::isdigit(fen[char_it]))
        {
            square += fen[char_it] - '0';
            ++char_it;
        }
        // just a "/"
        else
        {
            ++char_it;
        }
    }

    ++char_it;

    // checks who moves
    if (fen[char_it] == 'w')
        side_to_move = WHITE;
    else
        side_to_move = BLACK;

    // increment the reader
    char_it += 2;

    // read castling rights
    while (fen[char_it] != ' ')
    {
        switch (fen[char_it])
        {
        case 'K':
            rights |= WHITE_KING_CASTLE;
            ++char_it;
            break;

        case 'Q':
            rights |= WHITE_QUEEN_CASTLE;
            ++char_it;
            break;

        case 'k':
            rights |= BLACK_KING_CASTLE;
            ++char_it;
            break;

        case 'q':
            rights |= BLACK_QUEEN_CASTLE;
            ++char_it;
            break;

        default:
            ++char_it;
            break;
        }
    }

    ++char_it;

    // parses enpassant square
    if (fen[char_it] != '-')
    {
        // reverse it because of board representation
        int rank = fen[char_it] - 'a';
        int file = 7 - (fen[char_it + 1] - '1');
        en_passant_square = file * 8 + rank;
    }

    char_it += 2;

    uint8_t half_move_input = 0;
    while (fen[char_it] != ' ')
    {
        half_move_input = half_move_input * 10 + (fen[char_it] - '0');
        std::cout << half_move_input << "\n";
        ++char_it;
    }
    fifty_move_counter = half_move_input;

    ++char_it;

    // parses the full move counter
    uint16_t full_move_input = 0;
    while (fen[char_it] != ' ' && char_it < fen.size())
    {
        full_move_input = full_move_input * 10 + (fen[char_it] - '0');
        ++char_it;
    }
    half_move_counter = (2 * full_move_input) - 2 + side_to_move;
}

uint16_t Board::full_move_counter() const
{
    return (half_move_counter) / 2 + 1;
}

uint64_t Board::bitboard(uint8_t piece) const
{
    return pieces[piece / 2] & colors[piece & 1];
}

uint64_t Board::blockers() const
{
    return colors[WHITE] | colors[BLACK];
}

bool Board::is_square_attacked(uint8_t square, uint8_t side_attacking) const
{
    // attacked by pawns
    if ((side_attacking == WHITE) && (pawn_attacks[BLACK][square] & bitboard(WHITE_PAWN)))
        return true;
    if ((side_attacking == BLACK) && (pawn_attacks[WHITE][square] & bitboard(BLACK_PAWN)))
        return true;

    // attacked by knights
    if (knight_attacks[square] & (colors[side_attacking] & pieces[KNIGHT]))
        return true;

    // attacked by king
    if (king_attacks[square] & (colors[side_attacking] & pieces[KING]))
        return true;

    // attacks by sliding pieces
    uint64_t blocking_pieces = blockers();
    // attacked by bishop
    if (get_bishop_attacks(square, blocking_pieces) & (colors[side_attacking] & pieces[BISHOP]))
        return true;

    // attacked by rooks
    if (get_rook_attacks(square, blocking_pieces) & (colors[side_attacking] & pieces[ROOK]))
        return true;

    // attacked by queens
    if (get_queen_attacks(square, blocking_pieces) & (colors[side_attacking] & pieces[QUEEN]))
        return true;

    return false;
}

void Board::print() const
{
    std::cout << "\n";

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            // convert file & rank into square index
            uint8_t square = rank * 8 + file;

            // prints ranks
            if (!file)
                std::cout << "  " << (8 - rank) << " ";

            // prints out a 1 if there is a 1 at the bit location, 0 otherwise
            std::cout
                << " " << (board[square] != NO_PIECE ? (ascii_pieces[board[square]]) : ('.'));
        }

        // add a new line every rank
        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n\n";
    std::cout << "     Side:       " << (side_to_move == WHITE ? "white" : "black");
    std::cout << "\n     Enpassant:     " << ((en_passant_square == no_square) ? "no" : square_to_coordinate[en_passant_square]);
    std::cout << "\n     Castling:    " << ((rights & WHITE_KING_CASTLE) ? 'K' : '-') << ((rights & WHITE_QUEEN_CASTLE) ? 'Q' : '-') << ((rights & BLACK_KING_CASTLE) ? 'k' : '-') << ((rights & BLACK_QUEEN_CASTLE) ? 'q' : '-');
    std::cout << "\n     50-move:       " << (fifty_move_counter / 2);
    std::cout << "\n     Total Moves:   " << (full_move_counter());
    std::cout << "\n";
}

void Board::print_attacked_squares(uint8_t side_attacking) const
{
    uint64_t attack_mask = 0;
    // loops through all the squares to see if they are being attacked
    for (int square = 0; square < 64; ++square)
        if (is_square_attacked(square, side_attacking))
            set_bit(attack_mask, square);

    print_bitboard(attack_mask);
}