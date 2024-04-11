#include "board.h"

Board::Board(std::string fen)
{
}

void Board::print()
{
    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            int square = rank * 8 + file;

            int piece = -1;

            //
        }

        // print new line every rnk
        std::cout << "\n";
    }
}
