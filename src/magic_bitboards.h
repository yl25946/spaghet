#pragma once

#include "defs.h"

extern const uint8_t bishop_relevant_bits[64];

extern const uint8_t rook_relevant_bits[64];

uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask);