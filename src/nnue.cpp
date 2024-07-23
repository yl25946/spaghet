#include "nnue.h"

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

int chunk_size = 1;

Network net;

inline int uncolored_to_nnue(int piece, int color)
{
    return (piece + (color == WHITE ? 0 : 6));
}

inline int colored_to_nnue(int piece)
{
    return (piece >> 1) + (6 * (piece & 1));
}

Accumulator::Accumulator(const Board &board)
{
    for (int i = 0; i < HIDDEN_SIZE; ++i)
        accumulator[0][i] = net.feature_bias[i];

    for (int i = 0; i < HIDDEN_SIZE; ++i)
        accumulator[1][i] = net.feature_bias[i];

    for (int color = 0; color < 2; ++color)
    {
        for (int piece = 0; piece <= BITBOARD_PIECES::KING; ++piece)
        {
            uint64_t bitboard = board.bitboard(uncolored_to_colored(piece, color));
            while (bitboard)
            {
                // board uses a8 = 0, while we want a1 = 0, so we flip the white square
                int black_square = lsb(bitboard);
                int white_square = flip(black_square);
                int nnue_white_piece = uncolored_to_nnue(piece, color);

                int nnue_black_piece = uncolored_to_nnue(piece, color ^ 1);

                int nnue_white_input_index = 64 * nnue_white_piece + white_square;
                int nnue_black_input_index = 64 * nnue_black_piece + black_square;

                // std::cout << color << " " << piece << "\n";

                // std::cout << nnue_white_input_index << " " << nnue_black_input_index << " ";

                // updates all the pieces in the accumulators
                for (int i = 0; i < HIDDEN_SIZE; ++i)
                    accumulator[WHITE][i] += net.feature_weights[nnue_white_input_index][i];

                for (int i = 0; i < HIDDEN_SIZE; ++i)
                    accumulator[BLACK][i] += net.feature_weights[nnue_black_input_index][i];

                pop_bit(bitboard);
            }
        }
    }
}

void Accumulator::add(uint8_t piece, uint8_t square)
{
    // board uses a8 = 0, while we want a1 = 0, so we flip the white square
    int black_square = square;
    int white_square = flip(black_square);
    int nnue_white_piece = colored_to_nnue(piece);

    int nnue_black_piece = colored_to_nnue(piece ^ 1);

    // std::cout << static_cast<int>(piece ^ 1) << " " << nnue_black_piece << " ";

    int nnue_white_input_index = 64 * nnue_white_piece + white_square;
    int nnue_black_input_index = 64 * nnue_black_piece + black_square;

    // std::cout << color << " " << piece << "\n";

    // std::cout << nnue_white_input_index << " " << nnue_black_input_index << " ";

    // updates all the pieces in the accumulators
    for (int i = 0; i < HIDDEN_SIZE; ++i)
        accumulator[WHITE][i] += net.feature_weights[nnue_white_input_index][i];

    for (int i = 0; i < HIDDEN_SIZE; ++i)
        accumulator[BLACK][i] += net.feature_weights[nnue_black_input_index][i];
}

void Accumulator::remove(uint8_t piece, uint8_t square)
{
    // board uses a8 = 0, while we want a1 = 0, so we flip the white square
    int black_square = square;
    int white_square = flip(black_square);

    int nnue_white_piece = colored_to_nnue(piece);
    int nnue_black_piece = colored_to_nnue(piece ^ 1);

    int nnue_white_input_index = 64 * nnue_white_piece + white_square;
    int nnue_black_input_index = 64 * nnue_black_piece + black_square;

    // std::cout << color << " " << piece << "\n";

    // std::cout << nnue_white_input_index << " " << nnue_black_input_index << " ";

    // updates all the pieces in the accumulators
    for (int i = 0; i < HIDDEN_SIZE; ++i)
        accumulator[WHITE][i] -= net.feature_weights[nnue_white_input_index][i];

    for (int i = 0; i < HIDDEN_SIZE; ++i)
        accumulator[BLACK][i] -= net.feature_weights[nnue_black_input_index][i];
}

void Accumulator::add_sub(uint8_t add_piece, uint8_t add_square, uint8_t sub_piece, uint8_t sub_square)
{
    const int black_add_square = add_square;
    const int white_add_square = flip(black_add_square);
    const int black_sub_square = sub_square;
    const int white_sub_square = flip(black_sub_square);

    const int nnue_add_white_piece = colored_to_nnue(add_piece);
    const int nnue_add_black_piece = colored_to_nnue(add_piece ^ 1);
    const int nnue_sub_white_piece = colored_to_nnue(sub_piece);
    const int nnue_sub_black_piece = colored_to_nnue(sub_piece ^ 1);

    const int nnue_add_white_input_index = 64 * nnue_add_white_piece + white_add_square;
    const int nnue_add_black_input_index = 64 * nnue_add_black_piece + black_add_square;
    const int nnue_sub_white_input_index = 64 * nnue_sub_white_piece + white_sub_square;
    const int nnue_sub_black_input_index = 64 * nnue_sub_black_piece + black_sub_square;

    for (int i = 0; i < HIDDEN_SIZE; ++i)
    {
        accumulator[WHITE][i] += net.feature_weights[nnue_add_white_input_index][i];
        accumulator[WHITE][i] -= net.feature_weights[nnue_sub_white_input_index][i];
    }

    for (int i = 0; i < HIDDEN_SIZE; ++i)
    {
        accumulator[BLACK][i] += net.feature_weights[nnue_add_black_input_index][i];
        accumulator[BLACK][i] -= net.feature_weights[nnue_sub_black_input_index][i];
    }
}

void Accumulator::add_sub_sub(uint8_t add_piece, uint8_t add_square, uint8_t sub1_piece, uint8_t sub1_square, uint8_t sub2_piece, uint8_t sub2_square)
{
    const int black_add_square = add_square;
    const int white_add_square = flip(black_add_square);
    const int black_sub1_square = sub1_square;
    const int white_sub1_square = flip(black_sub1_square);
    const int black_sub2_square = sub2_square;
    const int white_sub2_square = flip(black_sub2_square);

    const int nnue_add_white_piece = colored_to_nnue(add_piece);
    const int nnue_add_black_piece = colored_to_nnue(add_piece ^ 1);
    const int nnue_sub1_white_piece = colored_to_nnue(sub1_piece);
    const int nnue_sub1_black_piece = colored_to_nnue(sub1_piece ^ 1);
    const int nnue_sub2_white_piece = colored_to_nnue(sub2_piece);
    const int nnue_sub2_black_piece = colored_to_nnue(sub2_piece ^ 1);

    const int nnue_add_white_input_index = 64 * nnue_add_white_piece + white_add_square;
    const int nnue_add_black_input_index = 64 * nnue_add_black_piece + black_add_square;
    const int nnue_sub1_white_input_index = 64 * nnue_sub1_white_piece + white_sub1_square;
    const int nnue_sub1_black_input_index = 64 * nnue_sub1_black_piece + black_sub1_square;
    const int nnue_sub2_white_input_index = 64 * nnue_sub2_white_piece + white_sub2_square;
    const int nnue_sub2_black_input_index = 64 * nnue_sub2_black_piece + black_sub2_square;

    for (int i = 0; i < HIDDEN_SIZE; ++i)
    {
        accumulator[WHITE][i] += net.feature_weights[nnue_add_white_input_index][i];
        accumulator[WHITE][i] -= net.feature_weights[nnue_sub1_white_input_index][i];
        accumulator[WHITE][i] -= net.feature_weights[nnue_sub2_white_input_index][i];
    }

    for (int i = 0; i < HIDDEN_SIZE; ++i)
    {
        accumulator[BLACK][i] += net.feature_weights[nnue_add_black_input_index][i];
        accumulator[BLACK][i] -= net.feature_weights[nnue_sub1_black_input_index][i];
        accumulator[BLACK][i] -= net.feature_weights[nnue_sub2_black_input_index][i];
    }
}

void Accumulator::add_sub_add_sub(uint8_t add1_piece, uint8_t add1_square, uint8_t add2_piece, uint8_t add2_square, uint8_t sub1_piece, uint8_t sub1_square, uint8_t sub2_piece, uint8_t sub2_square)
{
    const int black_add1_square = add1_square;
    const int white_add1_square = flip(black_add1_square);
    const int black_add2_square = add2_square;
    const int white_add2_square = flip(black_add2_square);
    const int black_sub1_square = sub1_square;
    const int white_sub1_square = flip(black_sub1_square);
    const int black_sub2_square = sub2_square;
    const int white_sub2_square = flip(black_sub2_square);

    const int nnue_add1_white_piece = colored_to_nnue(add1_piece);
    const int nnue_add1_black_piece = colored_to_nnue(add1_piece ^ 1);
    const int nnue_add2_white_piece = colored_to_nnue(add2_piece);
    const int nnue_add2_black_piece = colored_to_nnue(add2_piece ^ 1);
    const int nnue_sub1_white_piece = colored_to_nnue(sub1_piece);
    const int nnue_sub1_black_piece = colored_to_nnue(sub1_piece ^ 1);
    const int nnue_sub2_white_piece = colored_to_nnue(sub2_piece);
    const int nnue_sub2_black_piece = colored_to_nnue(sub2_piece ^ 1);

    const int nnue_add1_white_input_index = 64 * nnue_add1_white_piece + white_add1_square;
    const int nnue_add1_black_input_index = 64 * nnue_add1_black_piece + black_add1_square;
    const int nnue_add2_white_input_index = 64 * nnue_add2_white_piece + white_add2_square;
    const int nnue_add2_black_input_index = 64 * nnue_add2_black_piece + black_add2_square;
    const int nnue_sub1_white_input_index = 64 * nnue_sub1_white_piece + white_sub1_square;
    const int nnue_sub1_black_input_index = 64 * nnue_sub1_black_piece + black_sub1_square;
    const int nnue_sub2_white_input_index = 64 * nnue_sub2_white_piece + white_sub2_square;
    const int nnue_sub2_black_input_index = 64 * nnue_sub2_black_piece + black_sub2_square;

    for (int i = 0; i < HIDDEN_SIZE; ++i)
    {
        accumulator[WHITE][i] += net.feature_weights[nnue_add1_white_input_index][i];
        accumulator[WHITE][i] += net.feature_weights[nnue_add2_white_input_index][i];
        accumulator[WHITE][i] -= net.feature_weights[nnue_sub1_white_input_index][i];
        accumulator[WHITE][i] -= net.feature_weights[nnue_sub2_white_input_index][i];
    }

    for (int i = 0; i < HIDDEN_SIZE; ++i)
    {
        accumulator[BLACK][i] += net.feature_weights[nnue_add1_black_input_index][i];
        accumulator[BLACK][i] += net.feature_weights[nnue_add2_black_input_index][i];
        accumulator[BLACK][i] -= net.feature_weights[nnue_sub1_black_input_index][i];
        accumulator[BLACK][i] -= net.feature_weights[nnue_sub2_black_input_index][i];
    }
}

void Accumulator::make_move(const Board &board, Move move)
{
    uint8_t from = move.from_square();
    uint8_t to = move.to_square();
    uint8_t move_flag = move.move_flag();
    uint8_t moving_piece = board.mailbox[from];

    if (move_flag & PROMOTION)
    {
        if (move_flag & CAPTURES)
        {
            uint8_t captured_piece = board.mailbox[to];
            add_sub_sub(uncolored_to_colored(move.promotion_piece(), board.side_to_move), to, moving_piece, from, captured_piece, to);
        }
        else
            add_sub(uncolored_to_colored(move.promotion_piece(), board.side_to_move), to, moving_piece, from);
    }
    else if (move_flag == MOVE_FLAG::EN_PASSANT_CAPTURE)
    {
        uint8_t en_passant_captured_pawn_square = board.en_passant_square + ((board.side_to_move == WHITE) ? 8 : -8);
        add_sub_sub(moving_piece, to, moving_piece, from, moving_piece ^ 1, en_passant_captured_pawn_square);
    }
    else if (move_flag & CAPTURES)
    {
        uint8_t captured_piece = board.mailbox[to];
        add_sub_sub(moving_piece, to, moving_piece, from, captured_piece, to);
    }
    // castling
    else if (move_flag == MOVE_FLAG::KING_CASTLE)
    {
        // shifts the rook
        uint8_t rook_from = from + 3;
        uint8_t rook_to = to - 1;

        add_sub_add_sub(moving_piece, to, uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_to, moving_piece, from, uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_from);
    }
    else if (move_flag == MOVE_FLAG::QUEEN_CASTLE)
    {
        // shifts the rook
        uint8_t rook_from = from - 4;
        uint8_t rook_to = to + 1;

        add_sub_add_sub(moving_piece, to, uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_to, moving_piece, from, uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_from);
    }
    // quiet move
    else
    {
        add_sub(moving_piece, to, moving_piece, from);
    }
}

inline int32_t screlu(int16_t value)
{
    const int32_t clipped = std::clamp(static_cast<int32_t>(value), 0, L1Q);
    return clipped * clipped;
}

void NNUE::init(const char *file)
{
    // open the nn file
    FILE *nn = fopen(file, "rb");

    // this format will be [stm][hl neuron][bucket]
    int16_t untransposed_output_weights[2][HIDDEN_SIZE][OUTPUT_BUCKETS];

    // if it's not invalid read the config values from it
    if (nn)
    {
        // initialize an accumulator for every input of the second layer
        size_t read = 0;
        size_t fileSize = sizeof(Network);
        size_t objectsExpected = fileSize / sizeof(int16_t);

        read += fread(net.feature_weights, sizeof(int16_t), INPUT_WEIGHTS * HIDDEN_SIZE, nn);
        read += fread(net.feature_bias, sizeof(int16_t), HIDDEN_SIZE, nn);
        read += fread(untransposed_output_weights, sizeof(int16_t), HIDDEN_SIZE * 2 * OUTPUT_BUCKETS, nn);
        read += fread(net.output_bias, sizeof(int16_t), OUTPUT_BUCKETS, nn);

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
        std::memcpy(untransposed_output_weights, &gEVALData[memoryIndex], HIDDEN_SIZE * OUTPUT_BUCKETS * sizeof(int16_t) * 2);
        memoryIndex += HIDDEN_SIZE * OUTPUT_BUCKETS * sizeof(int16_t) * 2;
        std::memcpy(net.output_bias, &gEVALData[memoryIndex], OUTPUT_BUCKETS * sizeof(int16_t));
    }

    for (int stm = 0; stm < 2; ++stm)
        for (int weight = 0; weight < HIDDEN_SIZE; ++weight)
            for (int bucket = 0; bucket < OUTPUT_BUCKETS; ++bucket)
                net.output_weights[bucket][stm][weight] = untransposed_output_weights[stm][weight][bucket];
}

int NNUE::eval(const Board &board)
{
    return eval(board, calculate_bucket(board));
}

int NNUE::eval(const Board &board, int bucket)
{
    Accumulator accumulator(board);

    return eval(board, accumulator, bucket);
}

int NNUE::eval(const Board &board, const Accumulator &accumulator)
{
    return eval(board, accumulator, calculate_bucket(board));
}

int NNUE::eval(const Board &board, const Accumulator &accumulator, int bucket)
{
    int eval = 0;

#if defined(USE_SIMD)
    vepi32 sum = zero_epi32();

    constexpr int chunk_size = sizeof(vepi16) / sizeof(int16_t);
    // our perspective
    for (int i = 0; i < HIDDEN_SIZE; i += chunk_size)
    {
        // load in the data from the weights
        const vepi16 accumulator_data = load_epi16(&accumulator[board.side_to_move][i]);
        const vepi16 weights = load_epi16(&net.output_weights[bucket][0][i]);

        // clip
        const vepi16 clipped_accumulator = clip(accumulator_data, L1Q);

        // multiply with weights
        // still int16s, will not overflow
        const vepi16 intermediate = multiply_epi16(clipped_accumulator, weights);

        // we multiply with clipped acumulator weights, which will overflow, so we use multiply_add and turn them into int32s
        const vepi32 result = multiply_add_epi16(intermediate, clipped_accumulator);

        // add the result we have to the running sum
        sum = add_epi32(sum, result);
    }

    // their perspective
    for (int i = 0; i < HIDDEN_SIZE; i += chunk_size)
    {
        // load in the data from the weights

        const vepi16 accumulator_data = load_epi16(&accumulator[board.side_to_move ^ 1][i]);

        const vepi16 weights = load_epi16(&net.output_weights[bucket][1][i]);

        // clip
        const vepi16 clipped_accumulator = clip(accumulator_data, L1Q);

        // multiply with weights
        // still int16s, will not overflow
        const vepi16 intermediate = multiply_epi16(clipped_accumulator, weights);

        // we multiply with clipped acumulator weights, which will overflow, so we use multiply_add and turn them into int32s
        const vepi32 result = multiply_add_epi16(intermediate, clipped_accumulator);

        // add the result we have to the running sum
        sum = add_epi32(sum, result);
    }

    // finally reduce
    eval = reduce_add_epi32(sum);

#else
    // feed everything forward to get the final value
    for (int i = 0; i < HIDDEN_SIZE; ++i)
        eval += screlu(accumulator[board.side_to_move][i]) * net.output_weights[bucket][0][i];

    for (int i = 0; i < HIDDEN_SIZE; ++i)
        eval += screlu(accumulator[board.side_to_move ^ 1][i]) * net.output_weights[bucket][1][i];

#endif

    eval /= L1Q;
    eval += net.output_bias[bucket];
    eval = (eval * SCALE) / (L1Q * OutputQ);

    return eval;
}