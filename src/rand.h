#pragma once

#include "defs.h"

#define seed 69420

// generates a seeded pseudo random uint64 number with the mersenne twister
uint64_t random_uint64();

// generates a magic number candidate, which is a uint64 with fewer 1 bits
uint64_t generate_magic_number();
