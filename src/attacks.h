#pragma once

#include "defs.h"
#include "magic_bitboards.h"

// Contains all the attack tables
extern uint64_t pawn_attacks[2][64];
extern uint64_t knight_attacks[64];
extern uint64_t king_attacks[64];
// use occupancies to find the exact attacks
// [square] [occupancy]
extern uint64_t bishop_attacks[64][512];
extern uint64_t rook_attacks[64][4096];

// constasts for pawn bitshifts
// taken from
/*
     not A file          not H file         not HG files      not AB files
      bitboard            bitboard            bitboard          bitboard

 8  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 7  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 6  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 5  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 4  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 3  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 2  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 1  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1

    a b c d e f g h    a b c d e f g h    a b c d e f g h    a b c d e f g h

*/

// not A file constant
constexpr uint64_t NOT_A_FILE = 18374403900871474942ULL;

// not H file constant
constexpr uint64_t NOT_H_FILE = 9187201950435737471ULL;

// not HG file constant
constexpr uint64_t NOT_HG_FILE = 4557430888798830399ULL;

// not AB file constant
constexpr uint64_t NOT_AB_FILE = 18229723555195321596ULL;

// masks for generating precalculated attack tables
extern uint64_t bishop_masks[64];
extern uint64_t rook_masks[64];

// leaper pieces
uint64_t mask_pawn_attacks(uint8_t side, uint8_t square);
uint64_t mask_knight_attacks(uint8_t square);
uint64_t mask_king_attacks(uint8_t square);

// sliding pieces
uint64_t mask_bishop_attacks(uint8_t square);
uint64_t mask_rook_attacks(uint8_t square);

uint64_t get_bishop_attacks(uint8_t square, uint64_t occupancy);
uint64_t get_rook_attacks(uint8_t square, uint64_t occupancy);
uint64_t get_queen_attacks(uint8_t square, uint64_t occupancy);

// magic bitboard stuff
uint64_t bishop_attacks_on_the_fly(uint8_t square, uint64_t block);
uint64_t rook_attacks_on_the_fly(uint8_t square, uint64_t block);

// actual functions start here:
void init_leapers_attacks();
void init_sliders_attacks();