#include "see.h"

bool SEE(const Board &board, Move move, int threshold)
{
    if (move.is_castle())
        return threshold <= 0;

    uint8_t from_square = move.from_square();
    uint8_t to_square = move.to_square();
    uint8_t move_flag = move.move_flag();

    uint8_t target = move_flag == EN_PASSANT_CAPTURE ? PAWN : colored_to_uncolored(board.mailbox[to_square]);
    uint8_t promo = move.promotion_piece();
    int value = SEEValue[target] - threshold;

    // If we promote, we get the promoted piece and lose the pawn
    if (move.is_promotion())
        value += SEEValue[promo] - SEEValue[PAWN];

    // If we can't beat the threshold despite capturing the piece,
    // it is impossible to beat the threshold
    if (value < 0)
        return false;

    uint8_t attacker = colored_to_uncolored(board.mailbox[from_square]);

    // If we get captured, we lose the moved piece,
    // or the promoted piece in the case of promotion
    value -= move.is_promotion() ? SEEValue[promo] : SEEValue[attacker];

    // If we still beat the threshold after losing the piece,
    // we are guaranteed to beat the threshold
    if (value >= 0)
        return true;

    // doesn't matter if the square is occupied or not
    uint64_t occupied = board.blockers() ^ (1ULL << from_square) ^ (1ULL << to_square);

    // removed the piece on the from_square from the attackers bitboard, because we already used the piece to capture
    uint64_t attackers = board.attackers(to_square) ^ (1ULL << from_square);
    uint8_t side = board.side_to_move ^ 1;

    // manually adds in the ep attacks
    if (move_flag == MOVE_FLAG::DOUBLE_PAWN_PUSH)
    {
        int en_passant_square = to_square + (board.side_to_move == COLOR::WHITE ? 8 : -8);
        attackers |= board.side_to_move == WHITE ? (pawn_attacks[BLACK][en_passant_square] & board.bitboard(WHITE_PAWN)) : (pawn_attacks[WHITE][en_passant_square] & board.bitboard(BLACK_PAWN));
        // we need to look a move ahead, so we use the same color
        attackers |= board.side_to_move == WHITE ? (pawn_attacks[WHITE][en_passant_square] & board.bitboard(BLACK_PAWN)) : (pawn_attacks[BLACK][en_passant_square] & board.bitboard(WHITE_PAWN));
    }

    // print_bitboard(attackers);
    uint64_t bishops = board.pieces[BITBOARD_PIECES::BISHOP] | board.pieces[BITBOARD_PIECES::QUEEN];
    uint64_t rooks = board.pieces[BITBOARD_PIECES::ROOK] | board.pieces[BITBOARD_PIECES::QUEEN];

    // make captures until one sides run out, or fail to beat threshold
    while (true)
    {
        // removed used pieces from attackers
        attackers &= occupied;
        uint64_t my_attackers = attackers & board.colors[side];

        if (!my_attackers)
            break;

        // picks the next least valuable piece to capture with
        int piece_type;
        for (piece_type = PAWN; piece_type <= BITBOARD_PIECES::KING; ++piece_type)
        {
            if (my_attackers & board.pieces[piece_type])
                break;
        }
        side ^= 1;

        // the -1 prioritizes moves that stop trading faster?
        value = -value - 1 - SEEValue[piece_type];

        // value beats threshold, or can't beat threshold (negamaxed)
        if (value >= 0)
        {
            if (piece_type == KING && (attackers & board.colors[side]))
                side ^= 1;
            break;
        }

        // removed the used piece from the occupied
        occupied ^= 1ULL << (lsb(my_attackers & board.bitboard(uncolored_to_colored(piece_type, side ^ 1))));

        if (piece_type == BITBOARD_PIECES::PAWN || piece_type == BITBOARD_PIECES::BISHOP || piece_type == BITBOARD_PIECES::QUEEN)
            attackers |= get_bishop_attacks(to_square, occupied) & bishops;
        if (piece_type == BITBOARD_PIECES::ROOK || piece_type == BITBOARD_PIECES::QUEEN)
            attackers |= get_rook_attacks(to_square, occupied) & rooks;
    }

    return side != board.side_to_move;
}