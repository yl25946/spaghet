#pragma once

#include "defs.h"
#include "board.h"
#include "utils.h"
#include "incbin/incbin.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE = 16;
constexpr int SCALE = 400;
constexpr int L1Q = 255;
constexpr int OutputQ = 64;
using NNUE_indices = std::pair<std::size_t, std::size_t>;

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

    void init(const char *file);
    int eval(const Board &board);
    // void add(NNUE::accumulator &board_accumulator, const int piece, const int to);
    // void update(NNUE::accumulator &board_accumulator, std::vector<NNUEIndices> &NNUEAdd, std::vector<NNUEIndices> &NNUESub);
    // void addSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndices sub);
    // void addSubSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndic es sub1, NNUEIndices sub2);
#if defined(USE_AVX2)
    [[nodiscard]] int32_t horizontal_add(const __m256i sum);
#endif
};
