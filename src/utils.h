#pragma once

#include "defs.h"

void print_bitboard(uint64_t bitboard);

constexpr int seed = 69420;

// generates a seeded pseudo random uint64 number with the mersenne twister
uint64_t random_uint64();

// generates a magic number candidate, which is a uint64 with fewer 1 bits
uint64_t generate_magic_number();

// gets it in ms from the epoch
uint64_t get_time();

// gets the file of a piece, a-h is 0-7 respectively
uint8_t file(uint8_t square);

// gets the rank of a piece, zero indexed and flipped
uint8_t rank(uint8_t square);

// TAKEN FROM WEISS:

// obtains how much we should reduce the depth by for late move reduction
// move_number is how far we've iterated in the movelist
int lmr_reduction_quiet(int depth, int move_number);
int lmr_reduction_captures_promotions(int depth, int move_number);

bool is_mate_score(int score);
int mate_score_to_moves(int score);

// checks if a Piece (enum) is white
// NOTE: no_piece is white
// bool isWhite(uint8_t piece);