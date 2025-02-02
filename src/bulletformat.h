#pragma once

#include "defs.h"

struct __attribute__((packed)) BulletFormat
{
    uint64_t occ;
    // msb to lsb
    uint8_t pcs[16];
    int16_t score;
    uint8_t result;
    uint8_t king_square;
    uint8_t opp_king_square;
    uint8_t extra[3];
};