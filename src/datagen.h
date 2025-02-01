#pragma once

#include "defs.h"

#include "move.h"
#include "board.h"

struct __attribute__((packed)) BulletFormat
{
    uint64_t occ;
    // msb to lsb
    uint8_t pcs[16];
    uint8_t score;
    uint8_t result;
    uint8_t king_square;
    uint8_t opp_king_square;
    uint8_t extra[3];
};

/**
 * Takes in a bulletformat file and then relabels it with the evaluation of the NNUE
 */
void relabel_eval(const std::string &input_file, const std::string &output_file);