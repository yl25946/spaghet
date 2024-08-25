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

// gets the rank of a piece, zero indexed
uint8_t rank(uint8_t square);

// TAKEN FROM WEISS:
// [depth][move count]
extern int quiet_lmr[256][256];
// [depth][move count]
extern int noisy_lmr[256][256];
void init_lmr_reduction_tables();

bool is_mate_score(int score);
int mate_score_to_moves(int score);

// checks if a Piece (enum) is white
// NOTE: no_piece is white
// bool isWhite(uint8_t piece);