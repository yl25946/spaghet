#pragma once

#include "defs.h"
#include "move.h"
#include "board.h"
#include "movelist.h"
// #include "movepicker.h"

// class MoveList;

class QuietHistory
{
public:
    int16_t butterfly_table[2][64][64];

    QuietHistory();

    void clear();

    // call this after completing a search or before the another search
    // quarters the values in the table
    void update();

    // side_to_move is based on the side that is playing the move

    // inserts a move into the butterfly tables
    void update(Move move, int depth, uint8_t side_to_move, bool good);
    void update(MoveList &move_list, Move best_move, int depth, uint8_t side_to_move);
    int64_t move_value(Move move, uint8_t side_to_move);
};

class CaptureHistory
{
public:
    // [capturing piece][to][captured piece (uncolored)]
    int16_t table[12][64][6];

    CaptureHistory();

    void clear();

    void update(const Board &board, MoveList &move_list, Move failed_high_move, int depth);
    void update(const Board &board, Move move, int depth, bool good);
    int64_t move_value(const Board &board, Move move);
};

class ContinuationHistory
{
public:
    int16_t table[13][64][13][64];

    ContinuationHistory();

    void clear();

    // quarters the values in the table
    void update();

    // board should be the original board where the move has not been made
    void update(const Board &board, Move move, const Board &previous_board, Move previous_move, int depth, bool good);
    void update(const Board &board, MoveList &move_list, Move best_move, const Board &previous_board, Move previous_move, int depth);
    int64_t move_value(const Board &board, Move move, const Board &previous_board, Move previous_move);
};

class Killers
{
public:
    Move killers[2];

    Killers() { killers[0] = Move(a8, a8, 0), killers[1] = Move(a8, a8, 0); };

    void insert(Move move);

    // Move get_killer(int ply) const;;

    size_t size() const { return 2; };
};