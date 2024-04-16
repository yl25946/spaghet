#include "board.h"

Board::Board(const std::string_view &fen)
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
        half_move_input = half_move_counter * 10 + (fen[char_it] - '0');
        ++char_it;
    }
    half_move_counter = half_move_input;
}

void Board::print()
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
    std::cout << "     Side:      " << (side_to_move == WHITE ? "white" : "black");
    std::cout << "\n     Enpassant:    " << ((en_passant_square == no_square) ? "no" : square_to_coordinate[en_passant_square]);
    std::cout << "\n     Castling:   " << ((rights & WHITE_KING_CASTLE) ? 'K' : '-') << ((rights & WHITE_QUEEN_CASTLE) ? 'Q' : '-') << ((rights & BLACK_KING_CASTLE) ? 'k' : '-') << ((rights & BLACK_QUEEN_CASTLE) ? 'q' : '-');
    std::cout << "\n     50-move:      " << (half_move_counter / 2);
}
