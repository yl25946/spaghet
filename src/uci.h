#pragma once

#include "defs.h"

#include "move.h"
#include "board.h"
#include "search.h"
#include "time.h"
#include "perft.h"
#include "threads.h"
#include "search_stack.h"

class Threads;

Move parse_move(const std::string &move_string, Board &board);

class UciOptions
{
public:
    int hash_size = 16;
    int age = 0;
    int threads = 0;

    void reset();
};

void UCI_loop();

// position fen
// position startpos
// ... moves e2e4 e7e5
// automatically adds " fen " to the end of the command
Board parse_position(const std::string &line);
void parse_moves(const std::string &line, std::vector<Move> &moves, Board board);

// Board &parse_position(const std::string &command);