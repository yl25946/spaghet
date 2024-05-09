#include "magic_bitboards.h"

// bishop relevant occupancy bit count for every square on board
// i.e. how many bits on the bitboard at the corresponding square
// constexpr uint8_t bishop_relevant_bits[64] = {
//     6,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     6,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     7,
//     7,
//     7,
//     7,
//     5,
//     5,
//     5,
//     5,
//     7,
//     9,
//     9,
//     7,
//     5,
//     5,
//     5,
//     5,
//     7,
//     9,
//     9,
//     7,
//     5,
//     5,
//     5,
//     5,
//     7,
//     7,
//     7,
//     7,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     6,
//     5,
//     5,
//     5,
//     5,
//     5,
//     5,
//     6,
// };

// rook relevant occupancy bit count for every square on board
// i.e. how many bits are on the bitboard at the corresponding square
// constexpr uint8_t rook_relevant_bits[64] = {
//     12,
//     11,
//     11,
//     11,
//     11,
//     11,
//     11,
//     12,
//     11,
//     10,
//     10,
//     10,
//     10,
//     10,
//     10,
//     11,
//     11,
//     10,
//     10,
//     10,
//     10,
//     10,
//     10,
//     11,
//     11,
//     10,
//     10,
//     10,
//     10,
//     10,
//     10,
//     11,
//     11,
//     10,
//     10,
//     10,
//     10,
//     10,
//     10,
//     11,
//     11,
//     10,
//     10,
//     10,
//     10,
//     10,
//     10,
//     11,
//     11,
//     10,
//     10,
//     10,
//     10,
//     10,
//     10,
//     11,
//     12,
//     11,
//     11,
//     11,
//     11,
//     11,
//     11,
//     12,
// };

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
        uint8_t square = lsb(attack_mask);

        // pop this bit in the attack mask
        remove_bit(attack_mask, square);

        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            set_bit(occupancy, square);
    }

    return occupancy;
}

uint64_t find_magic_number(uint8_t square, int relevant_bits, int bishop_flag)
{
    // init occupancies
    uint64_t occupancies[4096];

    // initialize attacks
    uint64_t attacks[4096];

    // initialize used attacks
    uint64_t used_attacks[4096];

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

        attacks[i] = bishop_flag ? bishop_attacks_on_the_fly(square, occupancies[i]) : rook_attacks_on_the_fly(square, occupancies[i]);
    }

    // test magic numbers until we find one
    while (true)
    {
        // generate a magic number candidate
        uint64_t magic_number_candidate = generate_magic_number();

        // filter magic numbers that have less than  6 bits in the first 8 after hashing, because it'll be an ineffective hashing function
        if (count_bits((attack_mask * magic_number_candidate) & 0xFF00000000000000) < 6)
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
    for (uint8_t square = 0; square < 64; ++square)
    {
        // init magic numbers
        // printf(" 0x%llxULL,\n", find_magic_number(square, rook_relevant_bits[square], Rook));
        // printf("0x%llxULL,\n", find_magic_number(square, bishop_relevant_bits[square], Bishop));
    }
}