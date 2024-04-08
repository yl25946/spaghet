#pragma once

#include <iostream>
#include <stdint.h>
#include <array>
#include <optional>
#include <cstring>
#include <string>
#include <random>

#include "utils.h"
#include "mask.h"
#include "magic_bitboards.h"
// #include "board.h"
#include "rand.h"
#include "movegen.h"

// toss random imports and shit in here
#define NAME "   _____                   _          _   \n  / ____|                 | |        | |  \n | (___  _ __   __ _  __ _| |__   ___| |_ \n  \\___ \\| '_ \\ / _` |/ _` | '_ \\ / _ \\ __|\n  ____) | |_) | (_| | (_| | | | |  __/ |_ \n |_____/| .__/ \\__,_|\\__, |_| |_|\\___|\\__|\n        | |           __/ |               \n        |_|          |___/                "

// FEN dedug positions
#define empty_board "8/8/8/8/8/8/8/8 b - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
#define repetitions "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

// bit macros
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define remove_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define count_bits(bitboard) (__builtin_popcountll(bitboard))
// Returns the index of the least significant 1-bit of bitboard (zero-indexed), or -1 if bitboard is 0
#define lsb(bitboard) (__builtin_ffsll(bitboard) - 1)

// board squares
enum
{
    a8,
    b8,
    c8,
    d8,
    e8,
    f8,
    g8,
    h8,
    a7,
    b7,
    c7,
    d7,
    e7,
    f7,
    g7,
    h7,
    a6,
    b6,
    c6,
    d6,
    e6,
    f6,
    g6,
    h6,
    a5,
    b5,
    c5,
    d5,
    e5,
    f5,
    g5,
    h5,
    a4,
    b4,
    c4,
    d4,
    e4,
    f4,
    g4,
    h4,
    a3,
    b3,
    c3,
    d3,
    e3,
    f3,
    g3,
    h3,
    a2,
    b2,
    c2,
    d2,
    e2,
    f2,
    g2,
    h2,
    a1,
    b1,
    c1,
    d1,
    e1,
    f1,
    g1,
    h1,
    no_square
};

// convert squares to coordinates
extern const char *square_to_coordinate[];

// encode pieces
enum
{
    P,
    N,
    B,
    R,
    Q,
    K,
    p,
    n,
    b,
    r,
    q,
    k
};

// sides to move (colors)
enum
{
    white,
    black,
    both
};

// distinguish betwene rook and bishop in magic bitboard calculation
// if(bishop) returns true while if(rook) return false
enum
{
    rook,
    bishop
};