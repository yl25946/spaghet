#pragma once

#include "defs.h"

extern const uint8_t bishop_relevant_bits[64];

extern const uint8_t rook_relevant_bits[64];

extern const uint64_t rook_magic_numbers[64];

extern const uint64_t bishop_magic_numbers[64];

uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask);

// find approriate magic numbers
uint64_t find_magic_number(int square, int relevant_bits, int bishop);

// finds all the magic numbers
void init_magic_numbers();