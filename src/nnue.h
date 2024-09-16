#pragma once

#include "defs.h"
#include "board.h"
#include "simd.h"
#include "incbin/incbin.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE = 512;
constexpr int KING_BUCKETS = 4;
constexpr int OUTPUT_BUCKETS = 8;
constexpr int SCALE = 400;
constexpr int L1Q = 255;
constexpr int OutputQ = 64;

/*
.input(inputs::ChessBucketsMirrored::new([
            0, 0, 1, 1,
            0, 2, 2, 2,
            3, 3, 3, 3,
            3, 3, 3, 3,
            3, 3, 3, 3,
            3, 3, 3, 3,
            3, 3, 3, 3,
            3, 3, 3, 3,
        ]))
*/
constexpr int buckets[] = {
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    0,
    2,
    2,
    2,
    0,
    0,
    1,
    1,
};

constexpr uint64_t flipped_bitmask = 0xf0f0f0f0f0f0f0f0;

inline bool should_hm(const Board &board, uint8_t side_to_move)
{
    return board.bitboard(uncolored_to_colored(BITBOARD_PIECES::KING, side_to_move)) & flipped_bitmask;
}

inline int
calculate_output_bucket(const Board &board)
{
    int piece_count = count_bits(board.colors[COLOR::WHITE] | board.colors[COLOR::BLACK]);

    return std::min((63 - piece_count) * (32 - piece_count) / 225, 7);
}

inline int get_king_bucket(const Board &board, uint8_t side_to_move)
{
    uint64_t king_bitboard = board.bitboard(uncolored_to_colored(BITBOARD_PIECES::KING, side_to_move));
    uint8_t king_square = lsb(king_bitboard);

    if (side_to_move == BLACK)
        king_square = flip(king_square);

    if (should_hm(board, side_to_move))
        king_square = horizontally_flip(king_square);

    return king_buckets[king_square];
}

class Accumulator
{
public:
    // all indexed stm
    std::array<int, 2> king_buckets;
    std::array<int, 2> horizontally_mirrored;
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

    // if it isn't clean that means we need to refresh
    inline bool is_clean(const Board &board)
    {
        int white_king_bucket = get_king_bucket(board, board.side_to_move);
        int black_king_bucket = get_king_bucket(board, board.side_to_move ^ 1);

        return white_king_bucket == king_buckets[WHITE] && black_king_bucket == king_buckets[WHITE] && horizontally_mirrored[WHITE] == should_hm(board, WHITE) && horizontally_mirrored[BLACK] == should_hm(board, BLACK);
    }

    void make_move(const Board &board, Move move);

    void refresh(const Board &board);

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
    alignas(64) int16_t feature_weights[KING_BUCKETS][INPUT_WEIGHTS][HIDDEN_SIZE];
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
    static int eval(const Board &board, const Accumulator &accumulator);
    // void add(NNUE::accumulator &board_accumulator, const int piece, const int to);
    // // void update(NNUE::accumulator &board_accumulator, std::vector<NNUEIndices> &NNUEAdd, std::vector<NNUEIndices> &NNUESub);
    // void addSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndices sub);
    // void addSubSub(NNUE::accumulator &board_accumulator, NNUEIndices add, NNUEIndic es sub1, NNUEIndices sub2);
};
