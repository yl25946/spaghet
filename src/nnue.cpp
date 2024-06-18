#include "nnue.h"

#if defined(USE_AVX512) || defined(USE_AVX2)
#include <inmintrin.h>
#endif

// Macro to embed the default efficiently updatable neural network (NNUE) file
// data in the engine binary (using incbin.h, by Dale Weiler).
// This macro invocation will declare the following three variables
//     const unsigned char        gEVALData[];  // a pointer to the embedded data
//     const unsigned char *const gEVALEnd;     // a marker to the end
//     const unsigned int         gEVALSize;    // the size of the embedded file
// Note that this does not work in Microsoft Visual Studio.
#if !defined(_MSC_VER)
INCBIN(EVAL, EVALFILE);
#else
const unsigned char gEVALData[1] = {};
const unsigned char *const gEVALEnd = &gEVALData[1];
const unsigned int gEVALSize = 1;
#endif

Network net;

int32_t screlu(int16_t value)
{
    const int32_t clipped = std::clamp(static_cast<int32_t>(value), 0, L1Q);
    return clipped * clipped;
}

void NNUE::init(const char *file)
{
    // open the nn file
    FILE *nn = fopen(file, "rb");

    // if it's not invalid read the config values from it
    if (nn)
    {
        // initialize an accumulator for every input of the second layer
        size_t read = 0;
        size_t fileSize = sizeof(Network);
        size_t objectsExpected = fileSize / sizeof(int16_t);

        read += fread(net.feature_weights, sizeof(int16_t), INPUT_WEIGHTS * HIDDEN_SIZE, nn);
        read += fread(net.feature_bias, sizeof(int16_t), HIDDEN_SIZE, nn);
        read += fread(net.output_weights, sizeof(int16_t), HIDDEN_SIZE * 2, nn);
        read += fread(&net.output_bias, sizeof(int16_t), 1, nn);

        if (read != objectsExpected)
        {
            std::cout << "Error loading the net, aborting ";
            std::cout << "Expected " << objectsExpected << " shorts, got " << read << "\n";
            exit(1);
        }

        // after reading the config we can close the file
        fclose(nn);
    }
    else
    {
        // if we don't find the nnue file we use the net embedded in the exe
        uint64_t memoryIndex = 0;
        std::memcpy(net.feature_weights, &gEVALData[memoryIndex], INPUT_WEIGHTS * HIDDEN_SIZE * sizeof(int16_t));
        memoryIndex += INPUT_WEIGHTS * HIDDEN_SIZE * sizeof(int16_t);
        std::memcpy(net.feature_bias, &gEVALData[memoryIndex], HIDDEN_SIZE * sizeof(int16_t));
        memoryIndex += HIDDEN_SIZE * sizeof(int16_t);

        std::memcpy(net.output_weights, &gEVALData[memoryIndex], HIDDEN_SIZE * sizeof(int16_t) * 2);
        memoryIndex += HIDDEN_SIZE * sizeof(int16_t) * 2;
        std::memcpy(&net.output_bias, &gEVALData[memoryIndex], 1 * sizeof(int16_t));
    }
}

int NNUE::eval(const Board &board)
{
    std::array<std::array<int16_t, HIDDEN_SIZE>, 2> accumulator;

    // copies over all the biases

    for (int i = 0; i < HIDDEN_SIZE; ++i)
        accumulator[0][i] = net.feature_bias[i];

    for (int i = 0; i < HIDDEN_SIZE; ++i)
    {
        accumulator[1][i] = net.feature_bias[i];
        // std::cout << accumulator[0][i] << " " << accumulator[1][i] << " ";
    }

    for (int color = 0; color < 2; ++color)
    {
        for (int piece = 0; piece <= BITBOARD_PIECES::KING; ++piece)
        {
            uint64_t bitboard = board.bitboard(uncolored_to_colored(piece, color));
            while (bitboard)
            {
                int white_square = lsb(bitboard);
                int black_square = flip(white_square);
                int nnue_white_piece = uncolored_to_nnue(piece, color);

                // std::cout << nnue_white_piece << " " << square_to_coordinate[white_square] << " ";
                int nnue_black_piece = uncolored_to_nnue(piece, color ^ 1);

                int nnue_white_input_index = 64 * nnue_white_piece + white_square;
                int nnue_black_input_index = 64 * nnue_black_piece + black_square;

                // updates all the pieces in the accumulators
                for (int i = 0; i < HIDDEN_SIZE; ++i)
                    accumulator[WHITE][i] += net.feature_weights[nnue_white_input_index][i];

                for (int i = 0; i < HIDDEN_SIZE; ++i)
                    accumulator[BLACK][i] += net.feature_weights[nnue_black_input_index][i];

                pop_bit(bitboard);
            }
        }
    }

    int eval = 0;
    // feed everything forward to get the final value
    for (int i = 0; i < HIDDEN_SIZE; ++i)
        eval += screlu(accumulator[board.side_to_move][i]) * net.output_weights[0][i];

    for (int i = 0; i < HIDDEN_SIZE; ++i)
        eval += screlu(accumulator[board.side_to_move ^ 1][i]) * net.output_weights[1][i];

    eval /= L1Q;
    eval += net.output_bias;
    eval = (eval * SCALE) / (L1Q * OutputQ);

    return eval;
}