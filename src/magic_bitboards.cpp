#include "magic_bitboards.h"

// bishop relevant occupancy bit count for every square on board
// i.e. how many bits on the bitboard at the corresponding square
const uint8_t bishop_relevant_bits[64] = {
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    7,
    7,
    7,
    7,
    5,
    5,
    5,
    5,
    7,
    9,
    9,
    7,
    5,
    5,
    5,
    5,
    7,
    9,
    9,
    7,
    5,
    5,
    5,
    5,
    7,
    7,
    7,
    7,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
};

// rook relevant occupancy bit count for every square on board
// i.e. how many bits are on the bitboard at the corresponding square
const uint8_t rook_relevant_bits[64] = {
    12,
    11,
    11,
    11,
    11,
    11,
    11,
    12,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    11,
    12,
    11,
    11,
    11,
    11,
    11,
    11,
    12,
};

// rook magic numbers
const uint64_t rook_magic_numbers[64]{
    0x124c0104ULL,
    0xffffffff90001000ULL,
    0xffffffff80000000ULL,
    0xe000150ULL,
    0x2200c900ULL,
    0x1c1a0900ULL,
    0x8025120ULL,
    0xffffffffa0004000ULL,
    0xffffffff90001003ULL,
    0xffffffff80280410ULL,
    0xea00041ULL,
    0xffffffff80000000ULL,
    0xffffffff90280048ULL,
    0xffffffff80004000ULL,
    0xffffffff90202020ULL,
    0xffffffff80000000ULL,
    0x2a091880ULL,
    0xffffffff82821001ULL,
    0xffffffff80000588ULL,
    0xffffffff80120440ULL,
    0xffffffff80102040ULL,
    0x2a100100ULL,
    0x43088020ULL,
    0x20801403ULL,
    0x4010000ULL,
    0x700000b8ULL,
    0xffffffff890800d1ULL,
    0x10a02900ULL,
    0x8c00000ULL,
    0x10300b09ULL,
    0x10280810ULL,
    0x40b20400ULL,
    0x80401ULL,
    0x60400000ULL,
    0x291400ULL,
    0x10100200ULL,
    0xc000260ULL,
    0x20020058ULL,
    0x2820a010ULL,
    0xffffffffc0002000ULL,
    0x23400800ULL,
    0x4c4ULL,
    0xffffffff90001880ULL,
    0xffffffff94100601ULL,
    0x2110002ULL,
    0x426003c0ULL,
    0x20202040ULL,
    0x8080a00ULL,
    0x20428200ULL,
    0x20000020ULL,
    0x10280080ULL,
    0x408ULL,
    0x3201ULL,
    0x80008ULL,
    0x1000204ULL,
    0x4080008ULL,
    0x50010041ULL,
    0xffffffff80000000ULL,
    0x208806ULL,
    0x10000509ULL,
    0x18140012ULL,
    0x6000009ULL,
    0x18001a23ULL,
    0x1410020dULL,
};

// bishop magic numbers
const uint64_t bishop_magic_numbers[64] = {

    0xffffffff90001000ULL,
    0xffffffff80000000ULL,
    0xffffffffb0000930ULL,
    0xffffffffa020d024ULL,
    0xffffffff8c0000a0ULL,
    0xffffffffa0004000ULL,
    0xffffffff80000200ULL,
    0x9210100ULL,
    0xffffffff90001003ULL,
    0x202d01ULL,
    0x20cc0d08ULL,
    0xea00041ULL,
    0x61000600ULL,
    0xffffffff80000000ULL,
    0xa4840ULL,
    0x8824100ULL,
    0xffffffff94001400ULL,
    0x220e20ULL,
    0xffffffff8000a180ULL,
    0x6000c00ULL,
    0xffffffff80084000ULL,
    0x4022102ULL,
    0x4680600ULL,
    0x4452400cULL,
    0xffffffff80000000ULL,
    0x2282204ULL,
    0xffffffff80c40002ULL,
    0x90011ULL,
    0x2424044ULL,
    0xffffffff80000000ULL,
    0x10300b09ULL,
    0xffffffff800000b0ULL,
    0xffffffffc060000cULL,
    0x402011c1ULL,
    0x223218ULL,
    0xffffffffc0002000ULL,
    0xffffffffa0110012ULL,
    0x200200aULL,
    0xffffffff80000000ULL,
    0x8800442ULL,
    0xffffffffa0000034ULL,
    0xffffffff80808840ULL,
    0xffffffff80000000ULL,
    0x10000444ULL,
    0x42a0082ULL,
    0x304282ULL,
    0xffffffff80100c00ULL,
    0xffffffff80080008ULL,
    0xffffffff80400004ULL,
    0xffffffffe0000000ULL,
    0x4030002ULL,
    0xd00000ULL,
    0x48006208ULL,
    0xffffffff90040401ULL,
    0x42d0868cULL,
    0xffffffff88025884ULL,
    0x13800840ULL,
    0x192018ULL,
    0x11080042ULL,
    0x220c200ULL,
    0x100064ULL,
    0x102260ULL,
    0xffffffff800c58c0ULL,
    0x40911108ULL,
};

// basicaly a utility function for magic bitboards
// index is the index for blocker configurations
// Youre mapping the set bits of index to the set bits in the attack mask and setting the occupancy mask depending on which bits are set in index
// at index 0, you get occupancy with no bit set;
// at index 1, you get occupancy with the square of the first LSB from attack_mask set;
// at index 2, you get occupancy with the square of the second LSB from attack_mask set;
// at index 3 (which is 2**1 + 2**0), you get occupancy with the squares of the first and second LSBs from attack_mask set;
uint64_t
set_occupancy(int index, int bits_in_mask, uint64_t attack_mask)
{
    // occupancy map
    uint64_t occupancy = 0ULL;

    // loop over the range of bits within the attack mask
    for (int count = 0; count < bits_in_mask; ++count)
    {
        // get least signifinant 1 bit of attack_mask
        int square = lsb(attack_mask);

        // pop this bit in the attack mask
        remove_bit(attack_mask, square);

        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            set_bit(occupancy, square);
    }

    return occupancy;
}

uint64_t find_magic_number(int square, int relevant_bits, int bishop_flag)
{
    // init occupancies
    uint64_t occupancies[4096];

    // initialize attacks
    uint64_t attacks[4096];

    // initialize used attacks
    uint64_t used_attacks[64];

    // get current attack mask for a current piece
    // if it is a bishop generate bishop attacks, otherwise generate rook attakcs
    uint64_t attack_mask = bishop_flag ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    // initialize occupancy indices
    // limits the indicies to a certain number for magic bitboards
    int occupancy_indices = 1 << relevant_bits;

    // loop over all possible occupancy indices, and find the valid ones
    for (int i = 0; i < occupancy_indices; ++i)
    {
        // initialize our occupancies
        occupancies[i] = set_occupancy(i, relevant_bits, attack_mask);

        attacks[i] = bishop ? bishop_attacks_on_the_fly(square, occupancies[i]) : rook_attacks_on_the_fly(square, occupancies[i]);
    }

    // test magic numbers until we find one
    for (int i = 0; i < 100000000; ++i)
    {
        // generate a magic number candidate
        int magic_number_candidate = generate_magic_number();

        // filter magic numbers that have less than  6 bits in the first 8 after hashing, because it'll be an ineffective hashing function
        if (count_bits((attack_mask * magic_number_candidate) & 0xFF0000000000000) < 6)
            continue;

        // initialize used attacks with 0 bits
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // creates flags
        // fail finds when a magic number doesn't work
        int index = 0;
        bool fail = false;

        // test magic numbers by looping through all occupancies testing them individually
        // compares every attack to a used attacks table to test for collisions, if it finds one, then we move on
        for (; !fail && index < occupancy_indices; ++index)
        {
            // initialze magic index, found on programming wiki
            int magic_index = (int)((occupancies[index] * magic_number_candidate) >> (64 - relevant_bits));

            // comparing magic_index in used_attacks to index because we already initialized attacks
            // if we find an empty index, we have no collision, put in to keep track of what we've already hashed
            if (used_attacks[magic_index] == 0ULL)
                // initialize used attacks
                used_attacks[magic_index] = attacks[index];
            // basically detects a collision
            else if (used_attacks[magic_index] != attacks[index])
            {
                // magic index doesn't work
                fail = true;
                break;
            }
        }

        // if we didn't fail, that means the magic number works
        if (!fail)
            return magic_number_candidate;
    }

    // print failed magic number
    std::cout << " magic number fails";
    return 0ULL;
}

void init_magic_numbers()
{
    // loop over all board squares
    for (int square = 0; square < 64; ++square)
    {
        // init magic numbers
        // printf(" 0x%llxULL,\n", find_magic_number(square, rook_relevant_bits[square], rook));
        printf("0x%llxULL,\n", find_magic_number(square, bishop_relevant_bits[square], bishop));
    }
}