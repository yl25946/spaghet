#pragma once

#include "defs.h"

// stolen from nek.saikou on Discord
class Board
{
public:
    std::array<uint64_t, 6> pieces;
    std::array<uint64_t, 2> colors;

    // mailbox for speedup
    std::array<Pieces, 64> board;

    uint8_t side_to_move = white;
    uint8_t epSq = no_square;
    // castling rights,
    uint8_t rights = 0;

    uint64_t hash = 0;

    uint8_t hmc;

    //   std::array<Accumulator, 2> acc;

    // std::string fen();
};