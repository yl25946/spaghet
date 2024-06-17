#pragma once

#include "defs.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE = 1024;
using NNUE_indices = std::pair<std::size_t, std::size_t>;

class Network
{
    int16_t feature_weights[INPUT_WEIGHTS][HIDDEN_SIZE];
    int16_t feature_bias[HIDDEN_SIZE];
    int16_t output_weights[HIDDEN_SIZE][2];
    int16_t output_bias;
};

extern Network net;

class NNUE
{
};