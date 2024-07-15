#pragma once

#include "defs.h"
#include "board.h"
#include "utils.h"
#include "incbin/incbin.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE = 32;
constexpr int SCALE = 400;
constexpr int L1Q = 255;
constexpr int OutputQ = 64;

class Accumulator
{
public:
    std::array<std::array<int16_t, HIDDEN_SIZE>, 2> accumulator;

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
};

struct Network
{
    int16_t feature_weights[INPUT_WEIGHTS][HIDDEN_SIZE];
    int16_t feature_bias[HIDDEN_SIZE];
    int16_t output_weights[2][HIDDEN_SIZE];
    int16_t output_bias;
};

extern Network net;

class NNUE
{
public:
    // using accumulator = std::array<std::array<int16_t, HIDDEN_SIZE>, 2>;

    static void init(const char *file);
    static int eval(const Board &board);
    static int eval(const Accumulator &accumulator, uint8_t side_to_move);
    // void add(NNUE::accumulator &board_accumulator, const int piece, const int to);
    // void update(NNUE::accumulator &board_accumulator, std::vector<NNUEIndices> &NNUEAdd, std::vector<NNUEIndices> &NNUESub);
    // void addSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndices sub);
    // void addSubSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndic es sub1, NNUEIndices sub2);
    // #if defined(USE_AVX2)
    //     [[nodiscard]] int32_t horizontal_add(const __m256i sum);
    // #endif
};
