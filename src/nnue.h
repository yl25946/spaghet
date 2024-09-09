#pragma once

#include "defs.h"
#include "board.h"
#include "simd.h"
#include "incbin/incbin.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE = 512;
constexpr int OUTPUT_BUCKETS = 8;
constexpr int SCALE = 400;
constexpr int L1Q = 255;
constexpr int OutputQ = 64;

/**
 * Pairwise Multiplication Inference: Instead of feeding forward the entire network from L1,
 * you only feed forward half the number of neurons. You activate the the i and i + L1_SIZE/2,
 * multiply them together, and multiply the i-th L1 weight
 */
constexpr int PAIRWISE_OFFSET = HIDDEN_SIZE / 2;
// how many bits we want to shift to the right during pairwise mul
constexpr int FEATURE_SHIFT = 20;

inline int calculate_bucket(const Board &board)
{
    int piece_count = count_bits(board.colors[COLOR::WHITE] | board.colors[COLOR::BLACK]);

    return std::min((63 - piece_count) * (32 - piece_count) / 225, 7);
}

class Accumulator
{
public:
    alignas(64) std::array<std::array<int16_t, HIDDEN_SIZE>, 2> accumulator;

    Accumulator() {};
    Accumulator(const Board &board);
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
    alignas(64) int16_t output_weights[OUTPUT_BUCKETS][2][HIDDEN_SIZE / 2];
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
    static int eval(const Board &board, const Accumulator &accumulator);
    // void add(NNUE::accumulator &board_accumulator, const int piece, const int to);
    // // void update(NNUE::accumulator &board_accumulator, std::vector<NNUEIndices> &NNUEAdd, std::vector<NNUEIndices> &NNUESub);
    // void addSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndices sub);
    // void addSubSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndic es sub1, NNUEIndices sub2);
};
