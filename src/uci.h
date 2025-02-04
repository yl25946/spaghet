#pragma once

#include "defs.h"

#include "move.h"
#include "board.h"
#include "nnue_misc.h"
#include "search.h"
#include "time.h"
#include "perft.h"
#include "threads.h"
#include "search_stack.h"
#include "datagen.h"
class ThreadManger;

Move parse_move(const std::string &move_string, Board &board);

void UCI_loop();

// position fen
// position startpos
// ... moves e2e4 e7e5
// automatically adds " fen " to the end of the command
Board parse_position(const std::string &line);
void parse_moves(const std::string &line, std::vector<Move> &moves, Board board);

// parses the file from the "relabel" command
std::pair<std::string, std::string> parse_relabel_files(const std::string &line);

// Board &parse_position(const std::string &command);