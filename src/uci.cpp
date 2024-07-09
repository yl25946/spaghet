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

Board parse_position(const std::string &line)
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
        const size_t fen_start = 13;
        size_t fen_length = move_it - fen_start;
        std::string fen = line.substr(fen_start, fen_length);
        // std::cout << fen;
        board = Board(fen);
        // std::cout << board.fen();
    }

    return board;
}

// creates a copy of the board because we need to modify the board
void parse_moves(const std::string &line, std::vector<Move> &moves, Board board)
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

void UciOptions::reset()
{
    hash_size = 16;
    age = 0;
    threads = 0;
}

void UCI_loop()
{
    std::string line;
    Board board(start_position);
    UciOptions info;
    std::vector<Move> move_list;
    TranspositionTable transposition_table(info.hash_size);
    std::vector<ThreadData> thread_data(1);
    Threads threads(info);
    // dummy variable, should almost never be used other than in bench
    // Searcher searcher(board, move_list, UINT64_MAX);

    std::cout
        << "id name Spaghet MariNNara 1.0\n"
        << "id author Li Ying\n"
        << "option name Hash type spin default 16 min 1 max 1024\n"
        << "option name Threads type spin default 1 min 1 max 1\n"
        << "uciok" << std::endl;

    // std::cout << "option name Threads type spin default 1 min 1 max 1\n";

    while (true)
    {
        std::getline(std::cin, line);

        // int64_t min = 0;
        // int64_t max = 0;
        // for (int i = 0; i < 13; ++i)
        //     for (int j = 0; j < 64; ++j)
        //         for (int k = 0; k < 13; ++k)
        //             for (int l = 0; l < 64; ++l)
        //                 std::cout << thread_data[0].conthist.table[i][j][k][l] << " ";
        //             // min = std::min(min, history.butterfly_table[i][j][k]);
        //             // max = std::max(max, history.butterfly_table[i][j][k]);
        //         }
        //     }
        // }

        // std::cout << min << " " << max << "\n";

        if (line[0] == '\n')
            continue;

        if (!line.compare(0, 7, "isready"))
            std::cout << "readyok" << std::endl;
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
            // if we're calling on this, we assume that you've already gotten the moves, so we can just kill any rogue processes
            threads.terminate();
            // update history before searching to prevent race conditions
            // for (int i = 0; i < thread_data.size(); ++i)
            //     thread_data[i].main_history.update();

            // now that we've called go, we can increase the age
            ++info.age;

            for (int i = 0; i < thread_data.size(); ++i)
            {
                Searcher searcher(board, move_list, transposition_table, thread_data[i], info.age);

                // implements the go infinite command
                if (!line.compare(0, 11, "go infinite"))
                {
                    Time time("go depth 255");

                    time.set_time(searcher);
                }
                else
                {
                    Time time(line);

                    time.set_time(searcher);
                }

                threads.insert(searcher);
            }

            threads.go();
        }
        else if (!line.compare(0, 4, "stop"))
        {
            threads.terminate();
        }

        // put this before perft command otherwise it won't catch it
        else if (!line.compare(0, 11, "perft debug"))
        {
            std::cout << "here";
            for (Move move : move_list)
                board.make_move(move);

            // parses the depth
            int depth;
            size_t end_line;
            size_t go_pt = line.find("depth");
            if (go_pt != std::string::npos)
            {
                go_pt += 6;
                end_line = line.find(" ", go_pt);
                depth = stoi(line.substr(go_pt, end_line - go_pt));
            }

            perft_debug_driver(board.fen(), depth);
        }

        else if (!line.compare(0, 5, "perft"))
        {
            for (Move move : move_list)
                board.make_move(move);

            // parses the depth
            int depth;
            size_t end_line;
            size_t go_pt = line.find("depth");
            if (go_pt != std::string::npos)
            {
                go_pt += 6;
                end_line = line.find(" ", go_pt);
                depth = stoi(line.substr(go_pt, end_line - go_pt));
            }

            perft_driver(board.fen(), depth);
        }

        else if (!line.compare(0, 25, "setoption name Hash value"))
        {
            info.hash_size = std::stoi(line.substr(26));
            // std::cout << info.hash_size << "\n";
            transposition_table.resize(info.hash_size);
        }
        else if (!line.compare(0, 28, "setoption name Threads value"))
        {
            // no op because no multithreading
        }
        else if (!line.compare(0, 10, "ucinewgame"))
        {
            threads.terminate();
            info.reset();
            transposition_table = TranspositionTable(info.hash_size);
            thread_data.clear();
            thread_data.resize(1);
        }
        else if (!line.compare(0, 3, "uci"))
        {
            std::cout << "id name Spaghet MariNNara 1.0\n"
                      << "id author Li Ying\n"
                      << "option name Hash type spin default 16 min 1 max 1024\n"
                      << "option name Threads type spin default 1 min 1 max 1\n"
                      << "uciok" << std::endl;
        }
        else if (!line.compare(0, 4, "quit"))
        {
            threads.terminate();
            break;
        }
    }
}