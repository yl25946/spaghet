#include "uci.h"

Move parse_move(const std::string &move_string, Board &board)
{
    uint8_t from_square = 8 * (8 - (move_string[1] - '0')) + (move_string[0] - 'a');
    uint8_t to_square = 8 * (8 - (move_string[3] - '0')) + (move_string[2] - 'a');
    uint8_t promotion_piece = move_string.size() > 4 ? colored_to_uncolored(char_pieces[move_string[4]]) : PAWN;
    uint8_t piece_moving = board.mailbox[from_square];
    uint8_t move_flag = promotion_piece != PAWN ? PROMOTION + promotion_piece - 1 : 0;

    // checks for double push
    if (board.mailbox[from_square] / 2 == PAWN && abs((int)from_square - (int)to_square) == 16)
        return Move(from_square, to_square, DOUBLE_PAWN_PUSH);

    // checks for an en-passant capture
    if (colored_to_uncolored(piece_moving) == PAWN && to_square == board.en_passant_square)
        return Move(from_square, to_square, EN_PASSANT_CAPTURE);

    // checks for castling
    if (colored_to_uncolored(piece_moving) == KING && abs((int)from_square - (int)to_square) != 1)
    {
        // checks if the king moves to the g file. If it is, that means we are king side castling
        return Move(from_square, to_square, to_square % 8 == 6 ? KING_CASTLE : QUEEN_CASTLE);
    }

    // checks for captures
    if (board.mailbox[to_square] != NO_PIECE)
        move_flag |= CAPTURES;

    return Move(from_square, to_square, move_flag);
}

Board parse_position(std::string &line)
{
    std::vector<Move> move_list;
    Board board(start_position);
    // this iterator tracks after the moves
    size_t move_it = line.find("moves");
    if (move_it == std::string::npos)
        move_it = line.size();

    if (line.find("startpos") != std::string::npos)
    {
        // don't do anything here
    }
    else
    {
        const uint8_t fen_start = 13;
        uint8_t fen_length = move_it - fen_start;
        std::string fen = line.substr(fen_start, fen_length);
        board = Board(fen);
    }

    board.print();

    return board;
}

void UCI_loop()
{
    std::string line;
    Board board(start_position);
    // Searcher searcher;

    std::cout << "id Spaghet\n"
              << "id author Li Ying\n"
              << "uciok\n";

    // std::cout << "option name Threads type spin default 1 min 1 max 1\n";

    while (true)
    {
        std::getline(std::cin, line);

        if (!line.compare(0, 5, "bench"))
        {
            Searcher searcher;
            searcher.bench();
            break;
        }

        if (line[0] == '\n')
            continue;

        if (!line.compare(0, 7, "isready"))
            std::cout << "readyok\n";
        else if (!line.compare(0, 8, "position"))
        {
            board = parse_position(line);
        }
        else if (!line.compare(0, 2, "go"))
        {
        }
        else if (!line.compare(0, 4, "Hash"))
        {
            // no op because no tt
        }
        else if (!line.compare(0, 7, "Threads"))
        {
            // no op because no multithreading
        }
        else if (!line.compare(0, 10, "ucinewgame"))
        { // no op right now
        }
        else if (!line.compare(0, 3, "uci"))
        {
            std::cout << "id Spaghet\n"
                      << "id author Li Ying\n"
                      << "uciok\n";
        }
        else if (!line.compare(0, 4, "quit"))
        {
            break;
        }
    }
}