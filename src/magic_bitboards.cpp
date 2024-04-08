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
    0xffffffff80000000ULL,
    0xe000150ULL,
    0x8025120ULL,
    0x64000089ULL,
    0xffffffff80280410ULL,
    0xea00041ULL,
    0xffffffff80004000ULL,
    0xffffffff80000014ULL,
    0xffffffff80102040ULL,
    0x43088020ULL,
    0xffffffff90000050ULL,
    0x208a0041ULL,
    0x40b20400ULL,
    0xffffffffc0002000ULL,
    0xffffffff80000000ULL,
    0xffffffff80001002ULL,
    0xa04218ULL,
    0xffffffff90200248ULL,
    0xffffffff80028100ULL,
    0x14802200ULL,
    0x58200002ULL,
    0x588401ULL,
    0xffffffffe0000000ULL,
    0xffffffff8012e000ULL,
    0xffffffffcc030500ULL,
    0x40200018ULL,
    0x10c8004ULL,
    0x201a0000ULL,
    0x482a0000ULL,
    0xa000024ULL,
    0xffffffff80442800ULL,
    0x8802004ULL,
    0x3040000ULL,
    0x2420000ULL,
    0x404002ULL,
    0x101040ULL,
    0x820040ULL,
    0x811240ULL,
    0xffffffff80003010ULL,
    0x18002000ULL,
    0x4600207ULL,
    0x22c000c0ULL,
    0xffffffff82640009ULL,
    0x400480ULL,
    0xc0c00ULL,
    0x18a0ULL,
    0x1012141ULL,
    0xffffffffb21c0004ULL,
    0x6700a04ULL,
    0xffffffff80005000ULL,
    0x64307008ULL,
    0x4028080ULL,
    0x2200508ULL,
    0xffffffff82080200ULL,
    0x10ULL,
    0x1500202ULL,
    0x8002a86ULL,
    0xc811210ULL,
    0xffffffffc0001000ULL,
    0x15002206ULL,
    0xffffffff8a010005ULL,
    0x4801920ULL,
    0xffffffff82000824ULL,
};

// bishop magic numbers
const uint64_t bishop_magic_numbers[64] = {
    0xffffffff90001000ULL,
    0x42160442ULL,
    0xffffffffb0000930ULL,
    0xffffffffa0004000ULL,
    0xffffffff81000006ULL,
    0xffffffff80001100ULL,
    0xffffffff80000000ULL,
    0xffffffff80208100ULL,
    0xffffffff80000000ULL,
    0xffffffffa800808aULL,
    0xffffffff80000000ULL,
    0xffffffff80010000ULL,
    0x21120000ULL,
    0x2a010821ULL,
    0xffffffff92009140ULL,
    0xffffffff96118001ULL,
    0x42000650ULL,
    0x49002b11ULL,
    0xffffffff82000301ULL,
    0xffffffff84042080ULL,
    0xffffffff80000000ULL,
    0xffffffff80805004ULL,
    0x2200640ULL,
    0x341084ULL,
    0x23a0040ULL,
    0xffffffff800c58c0ULL,
    0xffffffffc0126802ULL,
    0x50430ULL,
    0x30021800ULL,
    0x1111008ULL,
    0xffffffff84000000ULL,
    0x4100d8ULL,
    0xffffffff80000000ULL,
    0x44808094ULL,
    0x2024a004ULL,
    0x46400ULL,
    0x210509ULL,
    0x8602ULL,
    0x20200886ULL,
    0x101101acULL,
    0x1c028a40ULL,
    0xffffffff861048c4ULL,
    0x3240000ULL,
    0x50004026ULL,
    0xffffffff80090200ULL,
    0x42040002ULL,
    0x28034048ULL,
    0xe90012ULL,
    0xffffffffa0404010ULL,
    0xffffffffa0800080ULL,
    0xffffffff80000000ULL,
    0x848000ULL,
    0x41102080ULL,
    0x5c06a00ULL,
    0xffffffff98200200ULL,
    0x42068800ULL,
    0xffffffff88400044ULL,
    0x41500020ULL,
    0x81c8ULL,
    0xa045069ULL,
    0x2400540ULL,
    0xd89040ULL,
    0x54014204ULL,
    0x48400484ULL,
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
        occupancies[i] = set_occupancy(i, bishop ? bishop_relevant_bits[square] : rook_relevant_bits[square], attack_mask);

        attacks[i] = bishop ? bishop_attacks_on_the_fly(square, occupancies[i]) : rook_attacks_on_the_fly(square, occupancies[i]);
    }

    // test magic numbers until we find one
    while (true)
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
        for (; !fail && index << occupancy_indices; ++index)
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
                fail = false;
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
        // rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], rook);
        // bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
    }
}