#include "board.h"

Board::Board(std::string fen)
{
    board.fill(NO_PIECE);
}

void Board::print()
{
    std::cout << "\n";

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            // convert file & rank into square index
            int square = rank * 8 + file;

            // prints ranks
            if (!file)
                std::cout << "  " << (8 - rank) << " ";

            // prints out a 1 if there is a 1 at the bit location, 0 otherwise
            std::cout
                << " " << (board[square] != NO_PIECE ? (ascii_pieces[board[square]]) : ('.'));
        }

        // add a new line every rank
        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n\n";
}
