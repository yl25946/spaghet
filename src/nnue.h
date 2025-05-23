#pragma once

#include "defs.h"
#include "board.h"
#include "simd.h"
// for bulletformat
#include "bulletformat.h"
#include "incbin/incbin.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE = 2048;
constexpr int OUTPUT_BUCKETS = 8;
constexpr int SCALE = 400;
constexpr int L1Q = 255;
constexpr int OutputQ = 64;

inline int calculate_bucket(uint64_t occ)
{
    auto count = count_bits(occ);
    return std::min((63 - count) * (32 - count) / 225, 7);
}

inline int calculate_bucket(const Board &board)
{
    return calculate_bucket(board.colors[COLOR::WHITE] | board.colors[COLOR::BLACK]);
}

class Accumulator
{
public:
    alignas(64) std::array<std::array<int16_t, HIDDEN_SIZE>, 2> accumulator;

    Accumulator() {};
    Accumulator(const Board &board);
    Accumulator(const BulletFormat &position);
    std::array<int16_t, HIDDEN_SIZE> &operator[](size_t i)
    {
        return accumulator[i];
    }

    const std::array<int16_t, HIDDEN_SIZE> &operator[](size_t i) const
    {
        return accumulator[i];
    }

    void make_move(const Board &board, Move move);

    // automatically converts a colored piece to a nnue piece
    void add(uint8_t piece, uint8_t square);
    // automatically converts a colored piece to a nnue piece
    void remove(uint8_t piece, uint8_t square);

    void add_sub(uint8_t add_piece, uint8_t add_square, uint8_t sub_piece, uint8_t sub_square);
    void add_sub_sub(uint8_t add_piece, uint8_t add_square, uint8_t sub1_piece, uint8_t sub1_square, uint8_t sub2_piece, uint8_t sub2_square);
    void add_sub_add_sub(uint8_t add1_piece, uint8_t add1_square, uint8_t add2_piece, uint8_t add2_square, uint8_t sub1_piece, uint8_t sub1_square, uint8_t sub2_piece, uint8_t sub2_square);
};

struct Network
{
    alignas(64) int16_t feature_weights[INPUT_WEIGHTS][HIDDEN_SIZE];
    alignas(64) int16_t feature_bias[HIDDEN_SIZE];
    alignas(64) int16_t output_weights[OUTPUT_BUCKETS][2][HIDDEN_SIZE];
    alignas(64) int16_t output_bias[OUTPUT_BUCKETS];
};

extern Network net;

class NNUE
{
public:
    // using accumulator = std::array<std::array<int16_t, HIDDEN_SIZE>, 2>;

    static void init(const char *file);
    static int eval(const Board &board);
    static int eval(const Board &board, int bucket);
    static int eval(const Board &board, const Accumulator &accumulator, int bucket);
    // only one that actually calculates
    static int eval(const Accumulator &accumulator, int bucket, uint8_t stm);
    static int eval(const Board &board, const Accumulator &accumulator);
    // void add(NNUE::accumulator &board_accumulator, const int piece, const int to);
    // // void update(NNUE::accumulator &board_accumulator, std::vector<NNUEIndices> &NNUEAdd, std::vector<NNUEIndices> &NNUESub);
    // void addSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndices sub);
    // void addSubSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndic es sub1, NNUEIndices sub2);
};
