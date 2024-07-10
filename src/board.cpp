#include "board.h"

constexpr uint8_t castling_rights[64] = {
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

    size_t char_it = 0;
    uint8_t square = 0;
    // we break this whenever we get a space
    while (fen[char_it] != ' ')
    {
        // if it is a piece name
        if (std::isalpha(fen[char_it]))
        {
            int piece = char_pieces[fen[char_it]];
            uint8_t bitboard_piece = colored_to_uncolored(piece);
            // initialize the bitboard
            // divide by two since we aren't keeping track of color
            set_bit(pieces[bitboard_piece], square);
            // if piece & 1 = 1, then that means it is the black
            set_bit(colors[piece & 1], square);

            // initialize the mailbox
            mailbox[square] = piece;

            // initialize the zobrist hash
            hash ^= zobrist_pieces[piece][square];

            // if piece is a pawn we can update the pawn hash
            if (bitboard_piece == BITBOARD_PIECES::PAWN)
                pawn_hash ^= zobrist_pieces[piece][square];

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
    {
        side_to_move = BLACK;

        // masks the side_to_move in zobrist hash
        hash ^= zobrist_side_to_move;
        // pawn_hash ^= zobrist_side_to_move;
    }

    // increment the reader
    char_it += 2;

    // read castling rights
    while (fen[char_it] != ' ')
    {
        switch (fen[char_it])
        {
        case 'K':
            rights |= WHITE_KING_CASTLE;
            break;

        case 'Q':
            rights |= WHITE_QUEEN_CASTLE;
            break;

        case 'k':
            rights |= BLACK_KING_CASTLE;
            break;

        case 'q':
            rights |= BLACK_QUEEN_CASTLE;
            break;

        default:
            break;
        }
        ++char_it;
    }

    ++char_it;

    // now that we've read in the castling rights we can mask it into the zobrist hash
    hash ^= zobrist_castling_rights[rights];

    // parses enpassant square
    if (fen[char_it] != '-')
    {
        // reverse it because of board representation
        int file = fen[char_it] - 'a';
        int rank = 7 - (fen[char_it + 1] - '1');
        en_passant_square = rank * 8 + file;

        // if there's an en passant square, we put it into the hash
        hash ^= zobrist_en_passant[file];
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

std::string Board::fen() const
{
    std::string fen = "";

    int no_piece_counter = 0;

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            int square = rank * 8 + file;
            int piece = mailbox[square];

            if (piece == NO_PIECE)
                ++no_piece_counter;
            else
            {
                if (no_piece_counter != 0)
                {
                    fen += (char)(no_piece_counter + '0');
                }

                fen += ascii_pieces[piece];

                no_piece_counter = 0;
            }
        }

        if (no_piece_counter != 0)
            fen += (char)(no_piece_counter + '0');

        no_piece_counter = 0;

        fen += "/";
    }

    // clips away the last "/"
    fen = fen.substr(0, fen.size() - 1);

    fen += " ";

    fen += side_to_move ? "b" : "w";

    fen += " ";

    if (rights == 0)
        fen += "-";

    if (rights & WHITE_KING_CASTLE)
        fen += "K";

    if (rights & WHITE_QUEEN_CASTLE)
        fen += "Q";

    if (rights & BLACK_KING_CASTLE)
        fen += "k";

    if (rights & BLACK_QUEEN_CASTLE)
        fen += "q";

    fen += " ";

    if (en_passant_square == no_square)
        fen += "-";
    else
        fen += square_to_coordinate[en_passant_square];

    fen += " ";

    fen += std::to_string(fifty_move_counter / 2);

    fen += " ";

    fen += std::to_string(full_move_counter());

    return fen;
}

uint16_t Board::full_move_counter() const
{
    return ((half_move_counter) / 2) + 1;
}

uint64_t Board::bitboard(uint8_t piece) const
{
    return pieces[colored_to_uncolored(piece)] & colors[piece & 1];
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

uint64_t Board::attackers(uint8_t square) const
{
    // attacks by sliding pieces
    uint64_t blocking_pieces = blockers();
    uint64_t attacks = 0;

    attacks |= pawn_attacks[BLACK][square] & bitboard(WHITE_PAWN);
    attacks |= pawn_attacks[WHITE][square] & bitboard(BLACK_PAWN);

    // attacked by knights
    attacks |= knight_attacks[square] & pieces[KNIGHT];

    // attacked by king
    attacks |= king_attacks[square] & pieces[KING];

    // attacked by bishop
    attacks |= get_bishop_attacks(square, blocking_pieces) & pieces[BISHOP];

    // attacked by rooks
    attacks |= get_rook_attacks(square, blocking_pieces) & pieces[ROOK];

    // attacked by queens
    attacks |= get_queen_attacks(square, blocking_pieces) & pieces[QUEEN];

    return attacks;
}

bool Board::is_in_check()
{
    return is_square_attacked(lsb(bitboard(WHITE_KING + side_to_move)), (side_to_move ^ 1));
}

// bitshift to the left (<<) the file number
constexpr uint64_t can_white_double_pawn_push = 0x1010000000000;
constexpr uint64_t can_black_double_pawn_push = 0x10100;

bool Board::is_pseudolegal(Move move) const
{
    uint8_t source_square = move.from_square();
    uint8_t target_square = move.to_square();
    uint8_t move_flag = move.move_flag();
    uint8_t piece = mailbox[source_square];
    uint64_t blocking_pieces = this->blockers();

    // if the piece doesn't exist ofc it won't work
    if (piece == NO_PIECE)
        return false;

    // if the piece isn't the right color
    if ((piece & 1) != side_to_move)
        return false;

    if ((colored_to_uncolored(mailbox[source_square] == PAWN) && move_flag == MOVE_FLAG::QUIET_MOVE))
    {
        return mailbox[target_square] == NO_PIECE;
    }

    // it is a pawn, we have to generate attacks separately
    if (piece <= 1)
    {
        return pawn_attacks[piece][source_square] & blocking_pieces;
    }

    // special cases for us to deal with separately
    switch (move_flag)
    {
    case MOVE_FLAG::EN_PASSANT_CAPTURE:
        return target_square == en_passant_square;

    case MOVE_FLAG::DOUBLE_PAWN_PUSH:
        if (side_to_move == WHITE)
        {
            // since pawn value = 0, it is basically just side to move
            if (mailbox[source_square] != WHITE_PAWN)
                return false;

            uint8_t pawn_file = file(source_square);

            return blocking_pieces & (can_white_double_pawn_push << pawn_file);
        }
        else
        {
            // since pawn value = 0, it is basically just side to move
            if (mailbox[source_square] != WHITE_PAWN)
                return false;

            uint8_t pawn_file = file(source_square);

            return blocking_pieces & (can_black_double_pawn_push << pawn_file);
        }

    case MOVE_FLAG::KING_CASTLE:
        if ((side_to_move == WHITE) && (rights & WHITE_KING_CASTLE) && !(blocking_pieces & 0x6000000000000000ULL) && !is_square_attacked(e1, side_to_move ^ 1) && !is_square_attacked(f1, side_to_move ^ 1) && !is_square_attacked(g1, side_to_move ^ 1))
            return true;
        else if ((rights & BLACK_KING_CASTLE) && !(blocking_pieces & 0x60ULL) && !is_square_attacked(e8, side_to_move ^ 1) && !is_square_attacked(f8, side_to_move ^ 1) && !is_square_attacked(g8, side_to_move ^ 1))
            return true;

        // cannot king castle
        return false;

    case MOVE_FLAG::QUEEN_CASTLE:
        if ((side_to_move == WHITE) && (rights & WHITE_QUEEN_CASTLE) && !(blocking_pieces & 0xe00000000000000ULL) && !is_square_attacked(e1, side_to_move ^ 1) && !is_square_attacked(d1, side_to_move ^ 1) && !is_square_attacked(c1, side_to_move ^ 1))
            return true;
        else if ((rights & BLACK_QUEEN_CASTLE) && !(blocking_pieces & 0xeULL) && !is_square_attacked(e8, side_to_move ^ 1) && !is_square_attacked(d8, side_to_move ^ 1) && !is_square_attacked(c8, side_to_move ^ 1))
            return true;

        return false;
    // none of the special cases
    default:
        break;
    }

    // check that a capture is valid
    if ((move_flag & CAPTURES) && (mailbox[target_square] == NO_PIECE))
        return false;

    // we can just generate bitboard attacks
    // we actually don't care if it's a capture or not, we just need to guarentee that a piece is able to move there
    piece = colored_to_uncolored(piece);

    switch (piece)
    {
    case BITBOARD_PIECES::KNIGHT:
        return knight_attacks[source_square] & (1ULL << target_square);

    case BITBOARD_PIECES::KING:
        return king_attacks[source_square] & (1ULL << target_square);

    case BITBOARD_PIECES::BISHOP:
        return get_bishop_attacks(source_square, blocking_pieces) & (1ULL << target_square);

    case BITBOARD_PIECES::ROOK:
        return get_rook_attacks(source_square, blocking_pieces) & (1ULL << target_square);

    case BITBOARD_PIECES::QUEEN:
        return get_queen_attacks(source_square, blocking_pieces) & (1ULL << target_square);

    default:
        break;
    }

    // if you ended up here, something went wrong
    return false;
}

bool Board::was_legal() const
{
    return !(is_square_attacked(lsb(bitboard(WHITE_KING + (side_to_move ^ 1))), side_to_move));
}

bool Board::only_pawns(uint8_t side_to_move) const
{
    uint64_t bitboard = 0;

    for (int i = BITBOARD_PIECES::KNIGHT; i <= BITBOARD_PIECES::QUEEN; ++i)
    {
        bitboard |= pieces[i];
    }

    return !(bitboard & colors[side_to_move]);
}

void Board::make_move(Move move)
{
    // update the bitboard
    uint8_t source_square = move.from_square();
    uint8_t target_square = move.to_square();
    uint8_t move_flag = move.move_flag();

    // colored piece
    uint8_t move_piece_type = mailbox[source_square];
    // uncolored piece
    uint8_t bitboard_piece_type = move_piece_type / 2;

    // en passant capture
    // do this before we clear out we update the en passant square
    if (move_flag == EN_PASSANT_CAPTURE)
    {
        uint8_t remove_square = en_passant_square + ((side_to_move == WHITE) ? 8 : -8);
        mailbox[remove_square] = NO_PIECE;
        remove_bit(pieces[PAWN], remove_square);
        remove_bit(colors[side_to_move ^ 1], remove_square);

        // removes pawn from zobrist hash
        // side_to_move ^ 1 represent the opposite move's pawn
        hash ^= zobrist_pieces[side_to_move ^ 1][remove_square];
        pawn_hash ^= zobrist_pieces[side_to_move ^ 1][remove_square];
    }
    else if (move_flag & CAPTURES)
    {
        uint8_t captured_piece = mailbox[target_square];
        uint8_t uncolored_captured_piece = colored_to_uncolored(captured_piece);

        remove_bit(pieces[uncolored_captured_piece], target_square);
        remove_bit(colors[side_to_move ^ 1], target_square);

        // do not need to update mailbox because it would've automatically overwritten it

        // update zobrist hash
        // this->print();

        // std::cout << (int)captured_piece << " " << move.to_string() << " " << this->fen() << '\n';
        hash ^= zobrist_pieces[captured_piece][target_square];

        if (uncolored_captured_piece == BITBOARD_PIECES::PAWN)
            pawn_hash ^= zobrist_pieces[captured_piece][target_square];
    }

    // moves the piece
    remove_bit(pieces[bitboard_piece_type], source_square);
    set_bit(pieces[bitboard_piece_type], target_square);
    remove_bit(colors[side_to_move], source_square);
    set_bit(colors[side_to_move], target_square);
    mailbox[source_square] = NO_PIECE;
    mailbox[target_square] = move_piece_type;

    // updates the hash
    // std::cout << ascii_pieces[move_piece_type] << " " << square_to_coordinate[source_square] << square_to_coordinate[target_square];

    hash ^= zobrist_pieces[move_piece_type][source_square];
    hash ^= zobrist_pieces[move_piece_type][target_square];

    if (bitboard_piece_type == BITBOARD_PIECES::PAWN)
    {
        pawn_hash ^= zobrist_pieces[move_piece_type][source_square];
        pawn_hash ^= zobrist_pieces[move_piece_type][target_square];
    }

    if (move_flag & PROMOTION)
    {
        // change the piece to the respective piece
        uint8_t promotion_piece = (2 * ((move_flag & 0b11ULL) + 1)) + side_to_move;
        remove_bit(pieces[PAWN], target_square);
        set_bit(pieces[promotion_piece / 2], target_square);
        mailbox[target_square] = promotion_piece;

        // removes the pawn from the bb and adds in the promotion piece
        hash ^= zobrist_pieces[move_piece_type][target_square];
        hash ^= zobrist_pieces[promotion_piece][target_square];
    }

    // double pawn push, basically updating the en_passant square
    if (move_flag == DOUBLE_PAWN_PUSH)
    {
        // if there previous was an en_passant square, we can just get rid of it
        if (en_passant_square != no_square)
            hash ^= zobrist_en_passant[file(en_passant_square)];

        // updates en_passant
        en_passant_square = target_square + (side_to_move == WHITE ? 8 : -8);

        // std::cout << "here";

        hash ^= zobrist_en_passant[file(en_passant_square)];
    }
    else
    {
        // if there previous was an en_passant square, we can just get rid of it
        if (en_passant_square != no_square)
        {
            hash ^= zobrist_en_passant[file(en_passant_square)];
        }

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

        // now updated to rook
        move_piece_type = mailbox[rook_source_square];

        // moves the piece
        remove_bit(pieces[ROOK], rook_source_square);
        set_bit(pieces[ROOK], rook_target_square);
        remove_bit(colors[side_to_move], rook_source_square);
        set_bit(colors[side_to_move], rook_target_square);
        mailbox[rook_source_square] = NO_PIECE;
        mailbox[rook_target_square] = move_piece_type;

        // updates the rook hash in zobrist
        hash ^= zobrist_pieces[move_piece_type][rook_source_square];
        hash ^= zobrist_pieces[move_piece_type][rook_target_square];
    }
    else if (move_flag == QUEEN_CASTLE)
    {
        // shifts the rook
        rook_source_square = source_square - 4;
        rook_target_square = target_square + 1;

        // now updated to rook
        move_piece_type = mailbox[rook_source_square];

        // moves the piece
        remove_bit(pieces[ROOK], rook_source_square);
        set_bit(pieces[ROOK], rook_target_square);
        remove_bit(colors[side_to_move], rook_source_square);
        set_bit(colors[side_to_move], rook_target_square);
        mailbox[rook_source_square] = NO_PIECE;
        mailbox[rook_target_square] = move_piece_type;

        // updates the rook hash in zobrist
        hash ^= zobrist_pieces[move_piece_type][rook_source_square];
        hash ^= zobrist_pieces[move_piece_type][rook_target_square];
    }

    // removes the castling rights from the hash
    hash ^= zobrist_castling_rights[rights];

    // updates castling rights
    rights &= castling_rights[source_square];
    rights &= castling_rights[target_square];

    // adds in the updated castling rights
    hash ^= zobrist_castling_rights[rights];

    // updates the entire board
    side_to_move ^= 1;

    // update zobrist side_to_move
    hash ^= zobrist_side_to_move;
    // pawn_hash ^= zobrist_side_to_move;

    if (bitboard_piece_type != PAWN && !(move_flag & CAPTURES))
        ++fifty_move_counter;
    else
        fifty_move_counter = 0;

    ++half_move_counter;
}

void Board::make_null_move()
{
    side_to_move ^= 1;

    // update the hash
    hash ^= zobrist_side_to_move;

    if (en_passant_square != no_square)
    {
        // undo the hash
        hash ^= zobrist_en_passant[file(en_passant_square)];

        en_passant_square = no_square;
    }

    ++fifty_move_counter;
    ++half_move_counter;
}

void Board::print() const
{

    for (int i = WHITE_PAWN; i <= BLACK_KING; ++i)
    {
        print_bitboard(bitboard(i));
    }

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
    std::cout << "\n     50-move(ply):  " << (int)(fifty_move_counter);
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