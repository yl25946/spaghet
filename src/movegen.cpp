#include "movegen.h"

uint64_t white_promotion = 0xFF;
uint64_t black_promotion = 0xFF00000000000000;
uint64_t rank_3 = 0x0000FF0000000000;
uint64_t rank_6 = 0xFF0000;

void generate_promotions(uint8_t from_square, uint8_t promotion_square, bool capture, MoveList &move_list)
{
    const uint8_t move_flag_constant = capture ? 12 : 8;
    for (int flag_it = 0; flag_it < 4; ++flag_it)
        move_list.insert(from_square, promotion_square, flag_it + move_flag_constant);
}

void generate_pawn_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard, attacks, blocking_pieces = board.blockers();

    // generate pawn moves and capture moves
    uint64_t promotions;
    uint64_t quiet_moves;
    uint64_t captures;

    if (board.side_to_move == WHITE)
    {
        bitboard = board.bitboard(WHITE_PAWN);

        // generate all single push quiet/promotion pawn moves
        attacks = (bitboard >> 8) & ~blocking_pieces;
        // separates them into promotions and quiet moves
        promotions = attacks & white_promotion;
        quiet_moves = attacks & ~white_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square + 8, target_square, false, move_list);
            pop_bit(promotions);
        }

        while (quiet_moves)
        {
            target_square = lsb(quiet_moves);
            move_list.insert(target_square + 8, target_square, QUIET_MOVE);
            pop_bit(quiet_moves);
        }

        // generates double push moves
        // attacks here will be single pawn pushes
        quiet_moves = ((attacks & rank_3) >> 8) & ~blocking_pieces;

        while (quiet_moves)
        {
            target_square = lsb(quiet_moves);
            move_list.insert(target_square + 16, target_square, DOUBLE_PAWN_PUSH);
            pop_bit(quiet_moves);
        }

        // generates all capture moves
        // right captures
        attacks = (bitboard & NOT_H_FILE) >> 7;
        // not promotions
        captures = attacks & board.colors[BLACK] & ~white_promotion;
        promotions = attacks & board.colors[BLACK] & white_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square + 7, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square + 7, target_square, CAPTURES);
            pop_bit(captures);
        }

        // left captures
        attacks = (bitboard & NOT_A_FILE) >> 9;
        // not promotions
        captures = attacks & board.colors[BLACK] & ~white_promotion;
        promotions = attacks & board.colors[BLACK] & white_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square + 9, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square + 9, target_square, CAPTURES);
            pop_bit(captures);
        }

        // generates en passant
        if (board.en_passant_square != no_square)
        {
            attacks = pawn_attacks[BLACK][board.en_passant_square];
            captures = attacks & bitboard;
            while (captures)
            {
                source_square = lsb(captures);
                move_list.insert(source_square, board.en_passant_square, EN_PASSANT_CAPTURE);
                pop_bit(captures);
            }
        }
    }
    else
    {
        bitboard = board.bitboard(BLACK_PAWN);

        // generate all single push quiet/promotion pawn moves
        attacks = (bitboard << 8) & ~blocking_pieces;
        // separates them into promotions and quiet moves
        promotions = attacks & black_promotion;
        quiet_moves = attacks & ~black_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square - 8, target_square, false, move_list);
            pop_bit(promotions);
        }
        while (quiet_moves)
        {
            target_square = lsb(quiet_moves);
            move_list.insert(target_square - 8, target_square, QUIET_MOVE);
            pop_bit(quiet_moves);
        }

        // generates double push moves
        // attacks here will be single pawn pushes
        quiet_moves = ((attacks & rank_6) << 8) & ~blocking_pieces;

        while (quiet_moves)
        {
            target_square = lsb(quiet_moves);
            move_list.insert(target_square - 16, target_square, DOUBLE_PAWN_PUSH);
            pop_bit(quiet_moves);
        }

        // generates pawn capture moves
        // right captures
        attacks = (bitboard & NOT_H_FILE) << 9;
        // not promotions
        captures = attacks & board.colors[WHITE] & ~black_promotion;
        promotions = attacks & board.colors[WHITE] & black_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square - 9, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square - 9, target_square, CAPTURES);
            pop_bit(captures);
        }

        // left captures
        attacks = (bitboard & NOT_A_FILE) << 7;
        captures = attacks & board.colors[WHITE] & ~black_promotion;
        promotions = attacks & board.colors[WHITE] & black_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square - 7, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square - 7, target_square, CAPTURES);
            pop_bit(captures);
        }

        // generates en passant
        if (board.en_passant_square != no_square)
        {
            attacks = pawn_attacks[WHITE][board.en_passant_square];
            captures = attacks & bitboard;
            while (captures)
            {
                source_square = lsb(captures);
                move_list.insert(source_square, board.en_passant_square, EN_PASSANT_CAPTURE);
                pop_bit(captures);
            }
        }
    }
}

void generate_pawn_capture_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard, attacks;

    // generate pawn moves and castle moves
    uint64_t promotions;
    uint64_t captures;

    if (board.side_to_move == WHITE)
    {
        bitboard = board.bitboard(WHITE_PAWN);

        // generates all capture moves
        // right captures
        attacks = (bitboard & NOT_H_FILE) >> 7;
        // not promotions
        captures = attacks & board.colors[BLACK] & ~white_promotion;
        promotions = attacks & board.colors[BLACK] & white_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square + 7, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square + 7, target_square, CAPTURES);
            pop_bit(captures);
        }

        // left captures
        attacks = (bitboard & NOT_A_FILE) >> 9;
        // not promotions
        captures = attacks & board.colors[BLACK] & ~white_promotion;
        promotions = attacks & board.colors[BLACK] & white_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square + 9, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square + 9, target_square, CAPTURES);
            pop_bit(captures);
        }

        // generates en passant
        if (board.en_passant_square != no_square)
        {
            attacks = pawn_attacks[BLACK][board.en_passant_square];
            captures = attacks & bitboard;
            while (captures)
            {
                source_square = lsb(captures);
                move_list.insert(source_square, board.en_passant_square, EN_PASSANT_CAPTURE);
                pop_bit(captures);
            }
        }
    }
    else
    {
        bitboard = board.bitboard(BLACK_PAWN);

        // generates pawn capture moves
        // right captures
        attacks = (bitboard & NOT_H_FILE) << 9;
        // not promotions
        captures = attacks & board.colors[WHITE] & ~black_promotion;
        promotions = attacks & board.colors[WHITE] & black_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square - 9, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square - 9, target_square, CAPTURES);
            pop_bit(captures);
        }

        // left captures
        attacks = (bitboard & NOT_A_FILE) << 7;
        captures = attacks & board.colors[WHITE] & ~black_promotion;
        promotions = attacks & board.colors[WHITE] & black_promotion;
        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square - 7, target_square, true, move_list);
            pop_bit(promotions);
        }
        while (captures)
        {
            target_square = lsb(captures);
            move_list.insert(target_square - 7, target_square, CAPTURES);
            pop_bit(captures);
        }

        // generates en passant
        if (board.en_passant_square != no_square)
        {
            attacks = pawn_attacks[WHITE][board.en_passant_square];
            captures = attacks & bitboard;
            while (captures)
            {
                source_square = lsb(captures);
                move_list.insert(source_square, board.en_passant_square, EN_PASSANT_CAPTURE);
                pop_bit(captures);
            }
        }
    }
}

void generate_knight_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;

    bitboard = board.bitboard(WHITE_KNIGHT + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = knight_attacks[source_square] & ~board.colors[board.side_to_move];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, (board.mailbox[target_square] == NO_PIECE) ? QUIET_MOVE : CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_knight_capture_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;

    bitboard = board.bitboard(WHITE_KNIGHT + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = knight_attacks[source_square] & ~board.colors[board.side_to_move];

        attacks &= board.colors[board.side_to_move ^ 1];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_bishop_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;
    uint64_t blocking_pieces = board.blockers();

    bitboard = board.bitboard(WHITE_BISHOP + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = get_bishop_attacks(source_square, blocking_pieces) & ~board.colors[board.side_to_move];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, (board.mailbox[target_square] == NO_PIECE) ? QUIET_MOVE : CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_bishop_capture_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;
    uint64_t blocking_pieces = board.blockers();

    bitboard = board.bitboard(WHITE_BISHOP + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = get_bishop_attacks(source_square, blocking_pieces) & ~board.colors[board.side_to_move];

        attacks &= board.colors[board.side_to_move ^ 1];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_rook_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;
    uint64_t blocking_pieces = board.blockers();

    bitboard = board.bitboard(WHITE_ROOK + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = get_rook_attacks(source_square, blocking_pieces) & ~board.colors[board.side_to_move];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, (board.mailbox[target_square] == NO_PIECE) ? QUIET_MOVE : CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_rook_capture_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;
    uint64_t blocking_pieces = board.blockers();

    bitboard = board.bitboard(WHITE_ROOK + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = get_rook_attacks(source_square, blocking_pieces) & ~board.colors[board.side_to_move];

        attacks &= board.colors[board.side_to_move ^ 1];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_queen_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;
    uint64_t blocking_pieces = board.blockers();

    bitboard = board.bitboard(WHITE_QUEEN + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = get_queen_attacks(source_square, blocking_pieces) & ~board.colors[board.side_to_move];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, (board.mailbox[target_square] == NO_PIECE) ? QUIET_MOVE : CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_queen_capture_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;
    uint64_t blocking_pieces = board.blockers();

    bitboard = board.bitboard(WHITE_QUEEN + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = get_queen_attacks(source_square, blocking_pieces) & ~board.colors[board.side_to_move];

        attacks &= board.colors[board.side_to_move ^ 1];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

// generate castling separately
void generate_sliding_king_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;

    bitboard = board.bitboard(WHITE_KING + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = king_attacks[source_square] & ~board.colors[board.side_to_move];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, (board.mailbox[target_square] == NO_PIECE) ? QUIET_MOVE : CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

// generate castling separately
void generate_sliding_king_capture_moves(Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard;
    uint64_t attacks;

    bitboard = board.bitboard(WHITE_KING + board.side_to_move);

    while (bitboard)
    {
        source_square = lsb(bitboard);

        // filters out all attacks that attack their own pieces
        attacks = king_attacks[source_square] & ~board.colors[board.side_to_move];

        attacks &= board.colors[board.side_to_move ^ 1];

        while (attacks)
        {
            target_square = lsb(attacks);

            move_list.insert(source_square, target_square, CAPTURES);

            pop_bit(attacks);
        }

        pop_bit(bitboard);
    }
}

void generate_castling_moves(Board &board, MoveList &move_list)
{
    uint64_t blocking_pieces = board.blockers();

    if (board.side_to_move == WHITE)
    {
        // checks if there's any pieces to prevent castling
        if ((board.rights & WHITE_KING_CASTLE) && !(blocking_pieces & 0x6000000000000000ULL) && !board.is_square_attacked(e1, board.side_to_move ^ 1) && !board.is_square_attacked(f1, board.side_to_move ^ 1) && !board.is_square_attacked(g1, board.side_to_move ^ 1))
            move_list.insert(e1, g1, KING_CASTLE);
        if ((board.rights & WHITE_QUEEN_CASTLE) && !(blocking_pieces & 0xe00000000000000ULL) && !board.is_square_attacked(e1, board.side_to_move ^ 1) && !board.is_square_attacked(d1, board.side_to_move ^ 1) && !board.is_square_attacked(c1, board.side_to_move ^ 1))
            move_list.insert(e1, c1, QUEEN_CASTLE);
    }
    else
    {
        // checks if there's any pieces to prevent castling
        if ((board.rights & BLACK_KING_CASTLE) && !(blocking_pieces & 0x60ULL) && !board.is_square_attacked(e8, board.side_to_move ^ 1) && !board.is_square_attacked(f8, board.side_to_move ^ 1) && !board.is_square_attacked(g8, board.side_to_move ^ 1))
            move_list.insert(e8, g8, KING_CASTLE);

        if ((board.rights & BLACK_QUEEN_CASTLE) && !(blocking_pieces & 0xeULL) && !board.is_square_attacked(e8, board.side_to_move ^ 1) && !board.is_square_attacked(d8, board.side_to_move ^ 1) && !board.is_square_attacked(c8, board.side_to_move ^ 1))
            move_list.insert(e8, c8, QUEEN_CASTLE);
    }
}

void generate_queen_promotions(const Board &board, MoveList &move_list)
{
    uint8_t source_square;
    uint8_t target_square;

    uint64_t bitboard, attacks, blocking_pieces = board.blockers();

    // generate pawn moves and capture moves
    uint64_t promotions;
    uint64_t quiet_moves;
    uint64_t captures;

    if (board.side_to_move == WHITE)
    {
        bitboard = board.bitboard(WHITE_PAWN);

        // generate all single push quiet/promotion pawn moves
        attacks = (bitboard >> 8) & ~blocking_pieces;
        // separates them into promotions and quiet moves
        promotions = attacks & white_promotion;

        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            move_list.insert(target_square + 8, target_square, MOVE_FLAG::QUEEN_PROMOTION_CAPTURE);
            pop_bit(promotions);
        }
    }

    else
    {
        bitboard = board.bitboard(BLACK_PAWN);

        // generate all single push quiet/promotion pawn moves
        attacks = (bitboard << 8) & ~blocking_pieces;
        // separates them into promotions and quiet moves
        promotions = attacks & black_promotion;

        // generates promotion moves
        while (promotions)
        {
            target_square = lsb(promotions);
            generate_promotions(target_square - 8, target_square, false, move_list);
            pop_bit(promotions);
        }
        }
}

void generate_moves(Board &board, MoveList &move_list)
{
    generate_pawn_moves(board, move_list);
    generate_knight_moves(board, move_list);
    generate_bishop_moves(board, move_list);
    generate_rook_moves(board, move_list);
    generate_queen_moves(board, move_list);
    generate_sliding_king_moves(board, move_list);
    generate_castling_moves(board, move_list);
}

void generate_capture_moves(Board &board, MoveList &move_list)
{
    generate_pawn_capture_moves(board, move_list);
    generate_knight_capture_moves(board, move_list);
    generate_bishop_capture_moves(board, move_list);
    generate_rook_capture_moves(board, move_list);
    generate_queen_capture_moves(board, move_list);
    generate_sliding_king_capture_moves(board, move_list);
}