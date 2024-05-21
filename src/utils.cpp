#include "utils.h"

void print_bitboard(uint64_t bitboard)
{
    std::cout << "\n";

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            // convert file & rank into square index
            uint8_t square = rank * 8 + file;

            // prints ranks
            if (!file)
                std::cout << "  " << (8 - rank) << " ";

            // prints out a 1 if there is a 1 at the bit location, 0 otherwise
            std::cout
                << " " << (get_bit(bitboard, square) ? 1 : 0);
        }

        // add a new line every rank
        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n\n";

    std::cout << "Bitboard: " << bitboard << "\n";
}

// random number generator
std::mt19937_64 random_uint64_generator{seed};

uint64_t random_uint64()
{
    return random_uint64_generator();
}

uint64_t generate_magic_number()
{
    return random_uint64() & random_uint64() & random_uint64();
}

// bool isWhite(uint8_t piece)
// {
//     return !(piece & 1);
// }

uint64_t get_time()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint8_t file(uint8_t square)
{
    return square & 7;
}

uint8_t rank(uint8_t square)
{
    return square >> 3;
}

int lmr_reduction_quiet(int depth, int move_number)
{
    return 1.35 + ((std::log(depth) * std::log(move_number)) / 2.75);
}

int lmr_reduction_captures_promotions(int depth, int move_number)
{
    return 0.2 + ((std::log(depth) * std::log(move_number)) / 3.35);
}

bool is_mate_score(int score)
{
    return score >= MAX_MATE_SCORE || score <= MIN_MATE_SCORE;
}

int mate_score_to_moves(int score)
{
    int mate_ply = score > 0 ? MATE - score + 1 : -MATE - score;

    return mate_ply / 2;
}