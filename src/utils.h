#pragma once

#include "defs.h"

void print_bitboard(uint64_t bitboard);

#define seed 69420

// generates a seeded pseudo random uint64 number with the mersenne twister
uint64_t random_uint64();

// generates a magic number candidate, which is a uint64 with fewer 1 bits
uint64_t generate_magic_number();

// gets it in ms from the epoch
uint64_t get_time();

// gets the file of a piece, a-h is 0-7 respectively
uint8_t file(uint8_t square);

// checks if a Piece (enum) is white
// NOTE: no_piece is white
// bool isWhite(uint8_t piece);