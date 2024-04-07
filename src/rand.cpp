#include "rand.h"

// random number generator
std::mt19937_64 random{seed};

uint64_t random_uint64()
{
    return random();
}

uint64_t generate_magic_number()
{
    return random_uint64() & random_uint64() & random_uint64();
}