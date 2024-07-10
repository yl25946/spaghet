#pragma once

#include <iostream>
#include <stdint.h>
#include <array>
#include <optional>
#include <cstring>
#include <string>
// #include <string_view>
#include <random>
#include <map>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <list>
#include <algorithm>
#include <thread>
#include <exception>
#include <sstream>
#include <memory>

// #include "utils.h"
// #include "attacks.h"
// #include "magic_bitboards.h"
// #include "board.h"
// #include "utils.h"
// #include "movegen.h"

// toss random imports and shit in here
#define NAME "   _____                   _          _   \n  / ____|                 | |        | |  \n | (___  _ __   __ _  __ _| |__   ___| |_ \n  \\___ \\| '_ \\ / _` |/ _` | '_ \\ / _ \\ __|\n  ____) | |_) | (_| | (_| | | | |  __/ |_ \n |_____/| .__/ \\__,_|\\__, |_| |_|\\___|\\__|\n        | |           __/ |               \n        |_|          |___/                "

// FEN dedug positions
extern std::string empty_board;
extern std::string start_position;
extern std::string tricky_position;
extern std::string killer_position;
extern std::string cmk_position;
extern std::string repetitions;

// bit macros
inline void set_bit(uint64_t &bitboard, uint8_t square)
{
    bitboard |= (1ULL << (square));
}
inline uint8_t get_bit(uint64_t bitboard, uint8_t square)
{
    return (bitboard & (1ULL << (square)));
}
inline void remove_bit(uint64_t &bitboard, uint8_t square)
{
    bitboard &= ~(1ULL << (square));
}
inline uint8_t count_bits(uint64_t bitboard)
{
    return __builtin_popcountll(bitboard);
}
// Returns the index of the least significant 1-bit of bitboard (zero-indexed), or -1 if bitboard is 0
inline uint8_t lsb(uint64_t bitboard)
{
    return __builtin_ffsll(bitboard) - 1;
}
#define pop_bit(bitboard) (bitboard &= (bitboard - 1))

inline uint8_t flip(uint8_t square)
{
    return square ^ 56;
}

inline uint8_t colored_to_uncolored(uint8_t piece)
{
    return piece >> 1;
}
inline uint8_t uncolored_to_colored(uint8_t piece, uint8_t color)
{
    return piece << 1 | color;
}

// highest possible score in engine
constexpr int16_t INF = 32000;
constexpr int16_t SCORE_NONE = INF + 1;
// this constant is the baseline mate score where we use ply to ajdjust
constexpr int16_t MATE = 30000;
constexpr int16_t MAX_PLY = 255;
// the max the lowest a mate can reach
constexpr int16_t MIN_MATE_SCORE = -MATE + MAX_PLY;
// the max the highest a mate can reach
constexpr int16_t MAX_MATE_SCORE = MATE - MAX_PLY;

// these two are used for reverse futility pruning
constexpr int DEPTH_MARGIN = 6;
constexpr int MARGIN = 80;

// used for move ordering
constexpr int64_t MAX_MOVE_ORDERING_SCORE = INT32_MAX;
constexpr int64_t CAPTURE_BONUS = 1LL << 21;
// same as max history
constexpr int64_t PROMOTION_BONUS = 1 << 14;

// this is a clamp value used for history
constexpr int64_t MAX_HISTORY = 1 << 14;
constexpr int64_t MIN_HISTORY = -MAX_HISTORY;

// this is the move ordering value for killers, make sure it's above histories
constexpr int64_t MAX_KILLERS = 1 << 20;

// entries in our thing
constexpr int CORRHIST_SIZE = 16384;
constexpr int CORRHIST_LIMIT = 1024;

// SEE values
constexpr int SEEValue[7] = {
    100,
    422,
    422,
    642,
    1015,
    0,
    0,
};

// debug SEE values
// constexpr int SEEValue[7] = {
//     100,
//     300,
//     300,
//     500,
//     900,
//     0,
//     0,
// };

// board squares
enum square
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
    no_square,
};

enum CHAR_PIECES
{
    P,
    p,
    N,
    n,
    B,
    b,
    R,
    r,
    Q,
    q,
    K,
    k,
};

// convert squares to coordinates
// takes in an int (from enum) and converts it to a string
const std::string square_to_coordinate[]{
    "a8",
    "b8",
    "c8",
    "d8",
    "e8",
    "f8",
    "g8",
    "h8",
    "a7",
    "b7",
    "c7",
    "d7",
    "e7",
    "f7",
    "g7",
    "h7",
    "a6",
    "b6",
    "c6",
    "d6",
    "e6",
    "f6",
    "g6",
    "h6",
    "a5",
    "b5",
    "c5",
    "d5",
    "e5",
    "f5",
    "g5",
    "h5",
    "a4",
    "b4",
    "c4",
    "d4",
    "e4",
    "f4",
    "g4",
    "h4",
    "a3",
    "b3",
    "c3",
    "d3",
    "e3",
    "f3",
    "g3",
    "h3",
    "a2",
    "b2",
    "c2",
    "d2",
    "e2",
    "f2",
    "g2",
    "h2",
    "a1",
    "b1",
    "c1",
    "d1",
    "e1",
    "f1",
    "g1",
    "h1",
};
constexpr char ascii_pieces[] = "PpNnBbRrQqKk";
// constexpr char *unicode_pieces[12] = {
//     "♙", "♟︎", "♘", "♞", "♗", "♝", "♖", "♜", "♕", "♛", "♔", "♚"};

// convert ASCII character pieces to encoded constants
extern std::map<char, uint8_t> char_pieces;

// used for mvv vla, takes in a colored piece value
// tuned values from clarty
constexpr uint16_t mvv_values[13] = {
    91,
    91,
    401,
    401,
    502,
    502,
    736,
    763,
    1192,
    1192,
    0,
    0,
    0,
};

// sides to move (colors)
enum COLOR
{
    WHITE,
    BLACK,
    BOTH
};

// used for accessing the bitboards
enum BITBOARD_PIECES
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
};

// distinguish betwene rook and bishop in magic bitboard calculation
// if(bishop) returns true while if(rook) return false
// enum
// {
//     Rook,
//     Bishop
// };

// pieces for a mailbox implementation
// NOTE: this enum and the char pieces enum are the same, it is okay to mix and match
enum PIECES
{
    WHITE_PAWN,
    BLACK_PAWN,
    WHITE_KNIGHT,
    BLACK_KNIGHT,
    WHITE_BISHOP,
    BLACK_BISHOP,
    WHITE_ROOK,
    BLACK_ROOK,
    WHITE_QUEEN,
    BLACK_QUEEN,
    WHITE_KING,
    BLACK_KING,
    NO_PIECE
};

// which side can castle
enum CASTLING_RIGHTS
{
    WHITE_KING_CASTLE = 1,
    WHITE_QUEEN_CASTLE = 2,
    BLACK_KING_CASTLE = 4,
    BLACK_QUEEN_CASTLE = 8,
};

// used for TT
enum BOUND
{
    NONE,
    FAIL_LOW,
    FAIL_HIGH,
    EXACT,
};