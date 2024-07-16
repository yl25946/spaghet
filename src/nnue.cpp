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

void Accumulator::make_move(const Board &board, Move move)
{
    uint8_t from = move.from_square();
    uint8_t to = move.to_square();
    uint8_t move_flag = move.move_flag();
    uint8_t moving_piece = board.mailbox[from];

    // std::cout << static_cast<int>(move_flag) << " ";

    if (move_flag & PROMOTION)
    {
        remove(uncolored_to_colored(BITBOARD_PIECES::PAWN, board.side_to_move), from);
        add(uncolored_to_colored(move.promotion_piece(), board.side_to_move), to);

        if (move_flag & CAPTURES)
        {
            uint8_t captured_piece = board.mailbox[to];
            remove(captured_piece, to);
        }

        // early return to avoid unecessary iteration, because that's expensive
        return;
    }

    if (move_flag == EN_PASSANT_CAPTURE)
    {
        uint8_t remove_square = board.en_passant_square + ((board.side_to_move == WHITE) ? 8 : -8);
        remove(uncolored_to_colored(BITBOARD_PIECES::PAWN, board.side_to_move ^ 1), remove_square);
    }
    else if (move_flag & CAPTURES)
    {
        uint8_t captured_piece = board.mailbox[to];
        remove(captured_piece, to);
    }

    // moves the piece
    remove(moving_piece, from);
    add(moving_piece, to);

    uint8_t rook_from;
    uint8_t rook_to;
    // castling
    if (move_flag == KING_CASTLE)
    {
        // shifts the rook
        rook_from = from + 3;
        rook_to = to - 1;

        remove(uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_from);
        add(uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_to);
    }
    else if (move_flag == QUEEN_CASTLE)
    {
        // shifts the rook
        rook_from = from - 4;
        rook_to = to + 1;

        remove(uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_from);
        add(uncolored_to_colored(BITBOARD_PIECES::ROOK, board.side_to_move), rook_to);
    }
}

inline int32_t screlu(int16_t value)
{
    const int32_t clipped = std::clamp(static_cast<int32_t>(value), 0, L1Q);
    return clipped * clipped;
}

inline int32_t calculate_bucket(const Board &board)
{
    int active_neurons = 0;

    for (int i = 0; i < 6; ++i)
        active_neurons += count_bits(board.pieces[i]);

    return (active_neurons - 2) / BUCKET_DIVISOR;
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

    // for (int stm = 0; stm < 2; ++stm)
    //     for (int weight = 0; weight < HIDDEN_SIZE; ++weight)
    //         for (int bucket = 0; bucket < OUTPUT_BUCKETS; ++bucket)
    //         {
    //             const int srcIdx = stm * weight * OUTPUT_BUCKETS + bucket;
    //             const int dstIdx = bucket * 2 * HIDDEN_SIZE + stm * weight;

    //             if (dstIdx / (HIDDEN_SIZE * OUTPUT_BUCKETS) >= 2 || (dstIdx % (HIDDEN_SIZE * OUTPUT_BUCKETS)) / HIDDEN_SIZE >= OUTPUT_BUCKETS || dstIdx % HIDDEN_SIZE >= HIDDEN_SIZE)
    //                 std::cout << "error";

    //             net.output_weights[bucket][stm][weight] = untransposed_output_weights[srcIdx];
    //         }

    // std::memcpy(net.output_weights, transposedL1Weights, HIDDEN_SIZE * sizeof(int16_t) * 2 * OUTPUT_BUCKETS);
}

int NNUE::eval(const Board &board)
{
    Accumulator accumulator(board);

    return eval(board, accumulator);
}

int NNUE::eval(const Board &board, const Accumulator &accumulator)
{
    const int bucket = calculate_bucket(board);

    int eval = 0;
    // feed everything forward to get the final value
    for (int i = 0; i < HIDDEN_SIZE; ++i)
        eval += screlu(accumulator[board.side_to_move][i]) * net.output_weights[bucket][0][i];

    for (int i = 0; i < HIDDEN_SIZE; ++i)
        eval += screlu(accumulator[board.side_to_move ^ 1][i]) * net.output_weights[bucket][1][i];

    eval /= L1Q;
    eval += net.output_bias[bucket];
    eval = (eval * SCALE) / (L1Q * OutputQ);

    return eval;
}