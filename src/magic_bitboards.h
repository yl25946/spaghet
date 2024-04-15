#pragma once

#include "defs.h"
#include "attacks.h"
#include "utils.h"

// extern const uint8_t bishop_relevant_bits[64];
// extern const uint8_t rook_relevant_bits[64];

// these are used for getting the number of bitshifts, calculated in init_slider()
extern const uint8_t bishop_shifts[64];
extern const uint8_t rook_shifts[64];

extern const uint64_t rook_magic_numbers[64];
extern const uint64_t bishop_magic_numbers[64];

uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask);

// find approriate magic numbers
uint64_t find_magic_number(uint8_t square, int relevant_bits, int bishop);

// finds all the magic numbers
void init_magic_numbers();