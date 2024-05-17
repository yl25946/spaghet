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
    if (colored_to_uncolored(piece_moving) == KING && abs((int)from_square - (int)to_square) == 2)
    {
        // checks if the king moves to the g file. If it is, that means we are king side castling
        return Move(from_square, to_square, (to_square & 7) == 6 ? KING_CASTLE : QUEEN_CASTLE);
    }

    // checks for captures
    if (board.mailbox[to_square] != NO_PIECE)
        move_flag |= CAPTURES;

    return Move(from_square, to_square, move_flag);
}

Board parse_position(std::string &line)
{
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
        // position fen
        const uint8_t fen_start = 13;
        uint8_t fen_length = move_it - fen_start;
        std::string fen = line.substr(fen_start, fen_length);
        // std::cout << fen;
        board = Board(fen);
    }

    return board;
}

// creates a copy of the board because we need to modify the board
void parse_moves(std::string &line, std::vector<Move> &moves, Board board)
{
    size_t move_it = line.find("moves");
    if (move_it == std::string::npos)
        return;

    // start at full move
    move_it += 6;
    std::string move;
    size_t next_space;
    while (move_it < line.size())
    {
        next_space = line.find(' ', move_it);
        if (next_space == std::string::npos)
            next_space = line.size();
        move = line.substr(move_it, next_space - move_it);
        Move m = parse_move(move, board);
        // std::cout << (int)m.move_flag();
        moves.push_back(m);
        board.make_move(m);
        move_it = next_space + 1;
    }

    return;
}

void UCI_loop()
{
    std::string line;
    Board board(start_position);
    std::vector<Move> move_list;
    int hash_size = 16;
    uint32_t age = 0;
    TranspositionTable transposition_table(hash_size);
    // dummy variable, should almost never be used other than in bench
    // Searcher searcher(board, move_list, UINT64_MAX);
    QuietHistory history;

    std::cout << "id Spaghet\n"
              << "id author Li Ying\n"
              << "option name Hash type spin default 16 min 1 max 1024\n"
              << "uciok\n";

    // std::cout << "option name Threads type spin default 1 min 1 max 1\n";

    while (true)
    {
        std::getline(std::cin, line);

        if (line[0] == '\n')
            continue;

        if (!line.compare(0, 7, "isready"))
            std::cout << "readyok\n";
        else if (!line.compare(0, 8, "position"))
        {
            // make sure we don't accidentially stack on previous position moves
            move_list.clear();

            board = parse_position(line);
            parse_moves(line, move_list, board);

            // Move m = move_list[move_list.size() - 1];

            // m.print();
            // std::cout << "flag: " << (int)m.move_flag();

            // Searcher searcher(board, move_list);
            // searcher.board.print();
        }
        else if (!line.compare(0, 2, "go"))
        {

            Time time(line);

            Searcher searcher(board, move_list, transposition_table, age);

            // gets the endtime
            searcher.end_time = time.get_move_time(searcher.board.side_to_move);

            // std::cout << searcher.board.only_pawns(WHITE) << "\n";

            // std::cout << searcher.board.hash << "\n";

            // perft_debug(searcher.board, 1, 1);

            // starts searching
            searcher.search();

            // now that we've called go once, we can increase the age
            ++age;
        }
        else if (!line.compare(0, 14, "setoption Hash"))
        {
            hash_size = std::stoi(line.substr(15));
            // std::cout << hash_size << "\n";
            transposition_table.resize(hash_size);
        }
        else if (!line.compare(0, 14, "option Threads"))
        {
            // no op because no multithreading
        }
        else if (!line.compare(0, 10, "ucinewgame"))
        {
            age = 0;
            transposition_table = TranspositionTable(hash_size);
            history.clear();
        }
        else if (!line.compare(0, 3, "uci"))
        {
            std::cout << "id Spaghet\n"
                      << "id author Li Ying\n"
                      << "option name Hash type spin default 16 min 1 max 1024\n"
                      << "uciok\n";
        }
        else if (!line.compare(0, 4, "quit"))
        {
            break;
        }
    }
}