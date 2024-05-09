#include "attacks.h"

// pawn attacks table [side][square]
uint64_t pawn_attacks[2][64];

// knight attacks table [square]
// regardless of color, knights can always move the same way
uint64_t knight_attacks[64];

// king attacks table [square]
// regardless of color, king can move the same way
uint64_t king_attacks[64];

// use occupancies to find the exact attacks
// [square] [occupancy]
uint64_t bishop_attacks[64][512];
uint64_t rook_attacks[64][4096];

// bishop attack masks [square][occupancies]
uint64_t bishop_masks[64];

// rook attack masks
uint64_t rook_masks[64];

// returns the bitboard of all the pawn attacks of a certain color and square
uint64_t mask_pawn_attacks(int side, uint8_t square)
{
    // result attacks bitboard
    uint64_t attacks = 0ULL;

    // piece bitboard
    uint64_t bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // white pawns
    if (!side)
    {
        // Since we are doing right capture moves, if we end up in the a file,
        // that means we must've "captured" outside of the board's boundary,
        // so we do not include that in our attacks
        if ((bitboard >> 7) & NOT_A_FILE)
            attacks |= (bitboard >> 7);
        // same idea for this but left capture moves an the ending up in the h file
        if ((bitboard >> 9) & NOT_H_FILE)
            attacks |= (bitboard >> 9);
    }

    // black pawns
    else
    {
        // Since we are doing right capture moves, if we end up in the a file,
        // that means we must've "captured" outside of the board's boundary,
        // so we do not include that in our attacks
        if ((bitboard << 7) & NOT_H_FILE)
            attacks |= (bitboard << 7);
        // same idea for this but left capture moves an the ending up in the h file
        if ((bitboard << 9) & NOT_A_FILE)
            attacks |= (bitboard << 9);
    }

    // return attack map
    return attacks;
}

// returns the bitboard of all the possible knight attacks from a certain square
uint64_t mask_knight_attacks(uint8_t square)
{
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // piece bitboard
    uint64_t bitboard = 0ULL;

    // puts the knight piece on the bitboard square
    set_bit(bitboard, square);

    // 17, 15, 10, 6 in both directions for all possible knight moves
    if ((bitboard >> 17) & NOT_H_FILE)
        attacks |= bitboard >> 17;
    if ((bitboard >> 15) & NOT_A_FILE)
        attacks |= bitboard >> 15;
    if ((bitboard >> 10) & NOT_HG_FILE)
        attacks |= bitboard >> 10;
    if ((bitboard >> 6) & NOT_AB_FILE)
        attacks |= bitboard >> 6;

    if ((bitboard << 17) & NOT_A_FILE)
        attacks |= bitboard << 17;
    if ((bitboard << 15) & NOT_H_FILE)
        attacks |= bitboard << 15;
    if ((bitboard << 10) & NOT_AB_FILE)
        attacks |= bitboard << 10;
    if ((bitboard << 6) & NOT_HG_FILE)
        attacks |= bitboard << 6;

    return attacks;
}

// returns the bitboard of all the possible knight attacks from a certain square
uint64_t mask_king_attacks(uint8_t square)
{
    // attacks bitboard
    uint64_t attacks = 0ULL;

    // piece bitboard
    uint64_t bitboard = 0ULL;

    // puts the knight piece on the bitboard square
    set_bit(bitboard, square);

    // 1, 7, 8, 9 bitshift in both directions for the king
    if ((bitboard >> 1) & NOT_H_FILE)
        attacks |= bitboard >> 1;
    if ((bitboard >> 7) & NOT_A_FILE)
        attacks |= bitboard >> 7;
    // this one always works because ifit is at the edge of the board it'll just bitshift off of the board
    attacks |= bitboard >> 8;
    if ((bitboard >> 9) & NOT_H_FILE)
        attacks |= bitboard >> 9;

    if ((bitboard << 1) & NOT_A_FILE)
        attacks |= bitboard << 1;
    if ((bitboard << 7) & NOT_H_FILE)
        attacks |= bitboard << 7;
    // this one always works because if it is at the edge of the board it'll just bitshift off the board
    attacks |= bitboard << 8;
    if ((bitboard << 9) & NOT_A_FILE)
        attacks |= bitboard << 9;

    return attacks;
}

// returns the bitboard of all possible bishop attacks from a certain square, regardles of color
uint64_t mask_bishop_attacks(uint8_t square)
{
    // result attacks bitboard
    uint64_t attacks = 0ULL;

    // init ranks & files
    int rank, file;

    // rank and file for the square
    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant bishop occupancy bits
    // by bounding it <= 6 we do not need to mask the edge bits
    for (rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; ++rank, ++file)
        attacks |= (1ULL << (rank * 8 + file));

    for (rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; --rank, ++file)
        attacks |= (1ULL << (rank * 8 + file));

    for (rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; ++rank, --file)
        attacks |= (1ULL << (rank * 8 + file));

    for (rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; --rank, --file)
        attacks |= (1ULL << (rank * 8 + file));

    return attacks;
}

// returns the bitboard of all possible rook attacks from a certain square, regardles of color
uint64_t mask_rook_attacks(uint8_t square)
{
    // result attacks bitboard
    uint64_t attacks = 0ULL;

    // init ranks & files
    int rank, file;

    // rank and file for the square
    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant bishop occupancy bits
    // the <= 6 makes it so that we don't include the edge of the board when masking
    for (rank = target_rank + 1; rank <= 6; ++rank)
        attacks |= (1ULL << (rank * 8 + target_file));

    for (rank = target_rank - 1; rank >= 1; --rank)
        attacks |= (1ULL << (rank * 8 + target_file));

    for (file = target_file + 1; file <= 6; ++file)
        attacks |= (1ULL << (target_rank * 8 + file));

    for (file = target_file - 1; file >= 1; --file)
        attacks |= (1ULL << (target_rank * 8 + file));

    return attacks;
}

// generates bishop attacks naively (for magic bitboards)
uint64_t bishop_attacks_on_the_fly(uint8_t square, uint64_t block)
{
    // result attacks bitboard
    uint64_t attacks = 0ULL;

    // init ranks & files
    int rank, file;

    // rank and file for the square
    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant bishop occupancy bits
    // by bounding it <= 6 we do not need to mask the edge bits
    for (rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; ++rank, ++file)
    {
        attacks |= (1ULL << (rank * 8 + file));
        // if we hit a blocker, we first encode the attack and then exit the loop
        if ((1ULL << (rank * 8 + file)) & block)
            break;
    }

    for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; --rank, ++file)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block)
            break;
    }

    for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; ++rank, --file)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block)
            break;
    }

    for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; --rank, --file)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block)
            break;
    }

    return attacks;
}

// generates rook attacks naively (for magic bitboards)
uint64_t rook_attacks_on_the_fly(uint8_t square, uint64_t block)
{
    // result attacks bitboard
    uint64_t attacks = 0ULL;

    // init ranks & files
    int rank, file;

    // rank and file for the square
    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant bishop occupancy bits
    // the <= 6 makes it so that we don't include the edge of the board when masking
    for (rank = target_rank + 1; rank <= 7; ++rank)
    {
        attacks |= (1ULL << (rank * 8 + target_file));
        if ((1ULL << (rank * 8 + target_file)) & block)
            break;
    }

    for (rank = target_rank - 1; rank >= 0; --rank)
    {
        attacks |= (1ULL << (rank * 8 + target_file));
        if ((1ULL << (rank * 8 + target_file)) & block)
            break;
    }

    for (file = target_file + 1; file <= 7; ++file)
    {
        attacks |= (1ULL << (target_rank * 8 + file));
        if ((1ULL << (target_rank * 8 + file)) & block)
            break;
    }

    for (file = target_file - 1; file >= 0; --file)
    {
        attacks |= (1ULL << (target_rank * 8 + file));
        if ((1ULL << (target_rank * 8 + file)) & block)
            break;
    }

    return attacks;
}

// get bishop attacks with a certain square and blockers
uint64_t get_bishop_attacks(uint8_t square, uint64_t occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= bishop_shifts[square];

    return bishop_attacks[square][occupancy];
}

// get bishop attacks with a certain square and blockers
uint64_t get_rook_attacks(uint8_t square, uint64_t occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= rook_shifts[square];

    return rook_attacks[square][occupancy];
}

// gets queen attacks with a certain square and blockers
uint64_t get_queen_attacks(uint8_t square, uint64_t occupancy)
{
    return get_bishop_attacks(square, occupancy) | get_rook_attacks(square, occupancy);
}

// initiates all the piece's attack tables
void init_leapers_attacks()
{
    for (uint8_t square = 0; square < 64; ++square)
    {
        pawn_attacks[WHITE][square] = mask_pawn_attacks(WHITE, square);
        pawn_attacks[BLACK][square] = mask_pawn_attacks(BLACK, square);

        knight_attacks[square] = mask_knight_attacks(square);

        king_attacks[square] = mask_king_attacks(square);
    }
}

// init all slider piece's attack tables
void init_sliders_attacks()
{
    // create bishop and rook masks (attack masks without occupancy)
    for (uint8_t square = 0; square < 64; ++square)
    {
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init  current masks
        uint64_t bishop_attack_mask = bishop_masks[square];
        uint64_t rook_attack_mask = rook_masks[square];

        // init occpancy indices
        int bishop_occupancy_indices = 1 << count_bits(bishop_attack_mask);
        int rook_occupancy_indices = 1 << count_bits(rook_attack_mask);

        // loop over bishop and rook occupancies
        for (int index = 0; index < bishop_occupancy_indices; ++index)
        {
            // creates current occupancy
            uint64_t occupancy = set_occupancy(index, 64 - bishop_shifts[square], bishop_attack_mask);

            // init magic index
            int magic_index = (occupancy * bishop_magic_numbers[square]) >> (bishop_shifts[square]);

            // creates bishop attacks
            bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
        }

        for (int index = 0; index < rook_occupancy_indices; ++index)
        {
            // creates current occupancy
            uint64_t occupancy = set_occupancy(index, 64 - rook_shifts[square], rook_attack_mask);

            // init magic index
            int magic_index = (occupancy * rook_magic_numbers[square]) >> (rook_shifts[square]);

            // creates rook attacks
            rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
        }
    }
}