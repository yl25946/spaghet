#include "magic_bitboards.h"

// basicaly a utility function for magic bitboards
// index is the index for blocker configurations
// Youre mapping the set bits of index to the set bits in the attack mask and setting the occupancy mask depending on which bits are set in index
// at index 0, you get occupancy with no bit set;
// at index 1, you get occupancy with the square of the first LSB from attack_mask set;
// at index 2, you get occupancy with the square of the second LSB from attack_mask set;
// at index 3 (which is 2**1 + 2**0), you get occupancy with the squares of the first and second LSBs from attack_mask set;
uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask)
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