#include "magic_bitboards.h"

// bishop relevant occupancy bit count for every square on board
// i.e. how many bits on the bitboard at the corresponding square
// const uint8_t bishop_relevant_bits[64] = {
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
// const uint8_t rook_relevant_bits[64] = {
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

const uint8_t bishop_shifts[64] = {
    58,
    59,
    59,
    59,
    59,
    59,
    59,
    58,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    57,
    57,
    57,
    57,
    59,
    59,
    59,
    59,
    57,
    55,
    55,
    57,
    59,
    59,
    59,
    59,
    57,
    55,
    55,
    57,
    59,
    59,
    59,
    59,
    57,
    57,
    57,
    57,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    59,
    58,
    59,
    59,
    59,
    59,
    59,
    59,
    58,
};

const uint8_t rook_shifts[64] = {
    52,
    53,
    53,
    53,
    53,
    53,
    53,
    52,
    53,
    54,
    54,
    54,
    54,
    54,
    54,
    53,
    53,
    54,
    54,
    54,
    54,
    54,
    54,
    53,
    53,
    54,
    54,
    54,
    54,
    54,
    54,
    53,
    53,
    54,
    54,
    54,
    54,
    54,
    54,
    53,
    53,
    54,
    54,
    54,
    54,
    54,
    54,
    53,
    53,
    54,
    54,
    54,
    54,
    54,
    54,
    53,
    52,
    53,
    53,
    53,
    53,
    53,
    53,
    52,
};

// rook magic numbers
const uint64_t rook_magic_numbers[64]{
    0x2080022240001080ULL,
    0x4c0004010082000ULL,
    0x808010002000800cULL,
    0x2080080080041000ULL,
    0x1600200830046200ULL,
    0x8300280400020100ULL,
    0x2000420a8012200ULL,
    0x200018403304302ULL,
    0x41800021814004ULL,
    0x2004400220100042ULL,
    0x802000100080ULL,
    0x2a4801000840800ULL,
    0x8a01001008010004ULL,
    0x80c0800400020080ULL,
    0x82000401080200ULL,
    0x42200010c208842ULL,
    0x880004000200054ULL,
    0x40a0008020804000ULL,
    0x8c21858020009000ULL,
    0x42f2000a004020ULL,
    0x4301003c110800ULL,
    0x20808004000200ULL,
    0x8808002000100ULL,
    0x20021008044ULL,
    0x8012802180004000ULL,
    0x50004000200cULL,
    0x2000100080200080ULL,
    0x400100100090020ULL,
    0x108020040400400ULL,
    0xc0210029000a1400ULL,
    0x82008040400100ULL,
    0x8200040041ULL,
    0x400804000800022ULL,
    0x250002010400042ULL,
    0x64aa2200820031c0ULL,
    0x2d48801000800800ULL,
    0x4001009005000800ULL,
    0x1012800200800400ULL,
    0x20411004000288ULL,
    0x8100208402000059ULL,
    0x2220804000208009ULL,
    0x500020024000ULL,
    0x42c8801200420021ULL,
    0x290080010008080ULL,
    0x4710080004008080ULL,
    0x8002000410020008ULL,
    0x2810002008080ULL,
    0x10230501409a0004ULL,
    0x120800040002080ULL,
    0x2010002000400c40ULL,
    0x240901200410c100ULL,
    0x1140500080080280ULL,
    0x8000400098180ULL,
    0x2802000204008080ULL,
    0x5000200040100ULL,
    0x1080004504840600ULL,
    0x301008010402202ULL,
    0x6020410282012116ULL,
    0x180208a0012c2ULL,
    0x4084050010000821ULL,
    0x281002408001013ULL,
    0x4842000104181026ULL,
    0x2050080082013004ULL,
    0x411000210040840eULL,
};

// bishop magic numbers
const uint64_t bishop_magic_numbers[64] = {
    0x4040020202020010ULL,
    0x404440424002844ULL,
    0x222020042010000ULL,
    0x3011040b80801002ULL,
    0x802021008802010ULL,
    0x90120324d0000ULL,
    0x102c008208202082ULL,
    0x700240104012000ULL,
    0x8806200250020082ULL,
    0x12a310040f0140ULL,
    0x4000884212421000ULL,
    0x5040c404c800048ULL,
    0x200240420810000ULL,
    0x22091002100000ULL,
    0x4000288410484424ULL,
    0x2821450048020840ULL,
    0x20410802040814ULL,
    0x1008083110510044ULL,
    0x44000840540100ULL,
    0x3020800802004040ULL,
    0x2004003188a00000ULL,
    0x8001004280600240ULL,
    0x8c004042080500ULL,
    0x501248212090400ULL,
    0x2018d48040506200ULL,
    0x5802021030148801ULL,
    0x8241202080201ULL,
    0x51090048004100ULL,
    0x102840085802000ULL,
    0x4c2020001880514ULL,
    0x8404004004220200ULL,
    0x2288200030104c0ULL,
    0x27228c010c49000ULL,
    0x4201044206208800ULL,
    0x4802080100880ULL,
    0xa0100401c0040ULL,
    0x60440440440100ULL,
    0x830008201010904ULL,
    0x14480480220080ULL,
    0x24040022088081ULL,
    0xa040820841112050ULL,
    0x10401a0040400ULL,
    0x464a0c0404004200ULL,
    0x1a0124080201ULL,
    0x1083010100900ULL,
    0x8001214102000100ULL,
    0xa020920c00411901ULL,
    0x928a604420400c0ULL,
    0x200490818c00000ULL,
    0x5024442221100000ULL,
    0x1000210822000dULL,
    0xb84280084040009ULL,
    0x282202015240804ULL,
    0x20419002022090ULL,
    0x232840800a40006ULL,
    0x8a21110111010012ULL,
    0x4012020601040300ULL,
    0x80030061042001ULL,
    0x28a0205008820ULL,
    0x4040c00022c21200ULL,
    0x21c840008030400ULL,
    0x4200180860088084ULL,
    0x410100200a2ULL,
    0x8010044808042020ULL,
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