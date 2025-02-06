#include "nnue_misc.h"

// Converts a Value into (centi)pawns and writes it in a buffer.
// The buffer must have capacity for at least 5 chars.
void format_cp_compact(int centripawn_value, char *buffer)
{

    buffer[0] = (centripawn_value < 0 ? '-' : centripawn_value > 0 ? '+'
                                                                   : ' ');

    centripawn_value = abs(centripawn_value);

    if (centripawn_value >= 10000)
    {
        buffer[1] = '0' + centripawn_value / 10000;
        centripawn_value %= 10000;
        buffer[2] = '0' + centripawn_value / 1000;
        centripawn_value %= 1000;
        buffer[3] = '0' + centripawn_value / 100;
        buffer[4] = ' ';
    }
    else if (centripawn_value >= 1000)
    {
        buffer[1] = '0' + centripawn_value / 1000;
        centripawn_value %= 1000;
        buffer[2] = '0' + centripawn_value / 100;
        centripawn_value %= 100;
        buffer[3] = '.';
        buffer[4] = '0' + centripawn_value / 10;
    }
    else
    {
        buffer[1] = '0' + centripawn_value / 100;
        centripawn_value %= 100;
        buffer[2] = '.';
        buffer[3] = '0' + centripawn_value / 10;
        centripawn_value %= 10;
        buffer[4] = '0' + centripawn_value / 1;
    }
}

// eval in cp
void format_cp_aligned_dot(int eval)
{
    const double pawn = static_cast<double>(std::abs(eval)) / 100.0;

    std::cout << (eval < 0 ? '-' : eval > 0 ? '+'
                                            : ' ')
              << std::setiosflags(std::ios::fixed) << std::setw(6) << std::setprecision(2) << pawn;
}

void print_eval(Board &board)
{
    Accumulator accumulator(board);

    char char_board[3 * 8 + 1][8 * 8 + 2];
    std::memset(char_board, ' ', sizeof(char_board));
    for (int row = 0; row < 3 * 8 + 1; ++row)
        char_board[row][8 * 8 + 1] = '\0';

    // A lambda to output one box of the char_board
    auto write_square = [&char_board, &board](uint8_t square, uint8_t colored_piece, int value)
    {
        const int x = int(file(square)) * 8;
        const int y = (7 - int(rank(square))) * 3;
        for (int i = 1; i < 8; ++i)
            char_board[y][x + i] = char_board[y + 3][x + i] = '-';
        for (int i = 1; i < 3; ++i)
            char_board[y + i][x] = char_board[y + i][x + 8] = '|';
        char_board[y][x] = char_board[y][x + 8] = char_board[y + 3][x + 8] = char_board[y + 3][x] = '+';
        if (colored_piece != Pieces::NO_PIECE)
            char_board[y + 1][x + 4] = ascii_pieces[colored_piece];
        if (value != SCORE_NONE)
            format_cp_compact(value, &char_board[y + 2][x + 2]);
    };

    // We estimate the value of each piece by doing a differential evaluation from
    // the current base eval, simulating the removal of the piece from its square.
    int base_eval = normalize_eval(board, NNUE::eval(board));
    base_eval = board.side_to_move == WHITE ? base_eval : -base_eval;

    for (int square = 0; square < 64; ++square)
    {
        // how much the piece is worth in the evaluation
        uint8_t colored_piece = board.mailbox[square];
        int piece_value = SCORE_NONE;

        if (colored_piece != Pieces::NO_PIECE && colored_to_uncolored(colored_piece) != Bitboard_Pieces::KING)
        {
            Board copy = board;
            copy.remove_piece(square);

            int new_eval = normalize_eval(copy, NNUE::eval(copy));

            piece_value = base_eval - new_eval;
        }

        write_square(square, colored_piece, piece_value);
    }

    std::cout << "\nNNUE derived piece values:\n";
    for (int row = 0; row < 3 * 8 + 1; ++row)
        std::cout << char_board[row] << '\n';
    std::cout << '\n';

    std::cout << "NNUE network contributions "
              << (board.side_to_move == WHITE ? "(White to move)" : "(Black to move)") << std::endl
              << "+------------+------------+\n"
              << "|   Bucket   | Evaluation |\n"
              << "+------------+------------+\n";

    const int correct_bucket = calculate_bucket(board);

    for (int bucket = 0; bucket < OUTPUT_BUCKETS; ++bucket)
    {
        std::cout << "|  " << bucket << "        " //
                  << " |  ";
        format_cp_aligned_dot(NNUE::eval(board, bucket));
        std::cout << "  " //
                  << " |";
        if (bucket == correct_bucket)
            std::cout << " <-- this bucket is used";
        std::cout << '\n';
    }

    std::cout << "+------------+------------+\n\n";

    int unscaled_eval = NNUE::eval(board);
    int scaled_eval = normalize_eval(board, evaluate(board, accumulator));

    unscaled_eval = board.side_to_move == WHITE ? unscaled_eval : -unscaled_eval;
    scaled_eval = board.side_to_move == WHITE ? scaled_eval : -scaled_eval;
    std::cout << "NNUE evaluation        " << 0.01 * unscaled_eval << " (white side)\n";

    std::cout << "Final evaluation       " << 0.01 * scaled_eval << " (white side)";
    std::cout << " [with scaled NNUE, ...]";
    std::cout << "\n\n";
}

std::pair<double, double> win_rate_params(const Board &board)
{
    const int material = count_bits(board.pieces[Bitboard_Pieces::PAWN]) + 3 * count_bits(board.pieces[Bitboard_Pieces::KNIGHT]) + 3 * count_bits(board.pieces[Bitboard_Pieces::BISHOP]) + 5 * count_bits(board.pieces[Bitboard_Pieces::ROOK]) + 9 * count_bits(board.pieces[Bitboard_Pieces::QUEEN]);

    // if we have any ridiculous positions with more material, we will clamp it to within the model
    const double m = std::clamp<double>(material, 10, 78) / 58.0;

    // Formula for normalizing the evaluation without clamping the material, where x is the material count
    // p_a = ((as[0] * m / 58 + as[1]) * x / 58 + a[2]) * x / 58 + a[3]
    // p_b = ((bs[0] * x / 58 + bs[1]) * x / 58 + bs[2]) * x / 58 + bs[3]
    constexpr double as[] = {-176.00534621, 495.96704269, -487.21153840, 405.14842161};
    constexpr double bs[] = {6.54377011, 34.14816513, -42.89469507, 61.12733160};

    const double p_a = (((as[0] * m + as[1]) * m + as[2]) * m) + as[3];
    const double p_b = (((bs[0] * m + bs[1]) * m + bs[2]) * m) + bs[3];

    return {p_a, p_b};
}

int win_rate_model(const Board &board, int eval)
{
    auto [p_a, p_b] = win_rate_params(board);

    // returns the win rate in thousandths in integers
    return static_cast<int>(0.5 + 1000 / (1 + std::exp((p_a - static_cast<double>(eval)) / p_b)));
}

int normalize_eval(const Board &board, int eval)
{
    auto [p_a, p_b] = win_rate_params(board);

    return std::round(100 * eval / p_a);
}