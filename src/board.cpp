#include "board.h"

const uint8_t castling_rights[64] = {
    7, 15, 15, 15, 3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14};

Board::Board()
{
}

Board::Board(const std::string &fen)
{
    // clears the entire board
    mailbox.fill(NO_PIECE);
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
            mailbox[square] = piece;

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
        // std::cout << half_move_input << "\n";
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
    return ((half_move_counter) / 2) + 1;
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

bool Board::is_in_check()
{
    return is_square_attacked(lsb(bitboard(WHITE_KING + side_to_move)), (side_to_move ^ 1));
}

bool Board::was_legal() const
{
    return !(is_square_attacked(lsb(bitboard(WHITE_KING + (side_to_move ^ 1))), side_to_move));
}

void Board::make_move(Move move)
{
    // update the bitboard
    uint8_t source_square = move.from_square();
    uint8_t target_square = move.to_square();
    uint8_t move_flag = move.move_flag();

    // deals with castle;

    uint8_t move_piece_type = mailbox[source_square];
    uint8_t bitboard_piece_type = move_piece_type / 2;

    if (move_flag & CAPTURES)
    {
        uint8_t captured_piece = mailbox[target_square];
        remove_bit(pieces[captured_piece / 2], target_square);
        remove_bit(colors[side_to_move ^ 1], target_square);

        // do not need to update mailbox because it would've automatically overwritten it
    }

    // moves the piece
    remove_bit(pieces[bitboard_piece_type], source_square);
    set_bit(pieces[bitboard_piece_type], target_square);
    remove_bit(colors[side_to_move], source_square);
    set_bit(colors[side_to_move], target_square);
    mailbox[source_square] = NO_PIECE;
    mailbox[target_square] = move_piece_type;

    if (move_flag & PROMOTION)
    {
        // change the piece to the respective piece
        uint8_t promotion_piece = (2 * ((move_flag & 0b11ULL) + 1)) + side_to_move;
        remove_bit(pieces[PAWN], target_square);
        set_bit(pieces[promotion_piece / 2], target_square);
        mailbox[target_square] = promotion_piece;
    }

    // en passant capture
    // do this before we clear out we update the en passant square
    if (move_flag == EN_PASSANT_CAPTURE)
    {
        uint8_t remove_square = en_passant_square + ((side_to_move == WHITE) ? 8 : -8);
        mailbox[remove_square] = NO_PIECE;
        remove_bit(pieces[PAWN], remove_square);
        remove_bit(colors[side_to_move ^ 1], remove_square);
    }

    // double pawn push, basically updating the en_passant square
    if (move_flag == DOUBLE_PAWN_PUSH)
    {
        // updates en_passant
        en_passant_square = target_square + (side_to_move == WHITE ? 8 : -8);
    }
    else
    {
        en_passant_square = no_square;
    }

    // used for castling to determine where to put the rooks
    uint8_t rook_source_square;
    uint8_t rook_target_square;

    // castling
    if (move_flag == KING_CASTLE)
    {
        // shifts the rook
        rook_source_square = source_square + 3;
        rook_target_square = target_square - 1;

        move_piece_type = mailbox[rook_source_square];

        // moves the piece
        remove_bit(pieces[ROOK], rook_source_square);
        set_bit(pieces[ROOK], rook_target_square);
        remove_bit(colors[side_to_move], rook_source_square);
        set_bit(colors[side_to_move], rook_target_square);
        mailbox[rook_source_square] = NO_PIECE;
        mailbox[rook_target_square] = move_piece_type;
    }
    else if (move_flag == QUEEN_CASTLE)
    {
        // shifts the rook
        rook_source_square = source_square - 4;
        rook_target_square = target_square + 1;

        move_piece_type = mailbox[rook_source_square];

        // moves the piece
        remove_bit(pieces[ROOK], rook_source_square);
        set_bit(pieces[ROOK], rook_target_square);
        remove_bit(colors[side_to_move], rook_source_square);
        set_bit(colors[side_to_move], rook_target_square);
        mailbox[rook_source_square] = NO_PIECE;
        mailbox[rook_target_square] = move_piece_type;
    }

    // updates castling rights
    rights &= castling_rights[source_square];
    rights &= castling_rights[target_square];

    // updates the entire board
    side_to_move ^= 1;

    if (bitboard_piece_type != PAWN && !(move_flag & CAPTURES))
        ++fifty_move_counter;
    else
        fifty_move_counter = 0;

    ++half_move_counter;
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
                << " " << (mailbox[square] != NO_PIECE ? (ascii_pieces[mailbox[square]]) : ('.'));
        }

        // add a new line every rank
        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n\n";
    std::cout << "     Side:       " << (side_to_move == WHITE ? "white" : "black");
    std::cout << "\n     Enpassant:     " << ((en_passant_square == no_square) ? "no" : square_to_coordinate[en_passant_square]);
    std::cout << "\n     Castling:    " << ((rights & WHITE_KING_CASTLE) ? 'K' : '-') << ((rights & WHITE_QUEEN_CASTLE) ? 'Q' : '-') << ((rights & BLACK_KING_CASTLE) ? 'k' : '-') << ((rights & BLACK_QUEEN_CASTLE) ? 'q' : '-');
    std::cout << "\n     50-move(ply):  " << (fifty_move_counter);
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