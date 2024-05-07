#pragma once

#include "defs.h"
#include "board.h"
#include "move.h"

class TT_Entry
{
public:
    // hash of the board state
    uint64_t hash;
    Move best_move;
    int16_t score;
    uint8_t depth;
    // least 2 significant bits is the flag, and the rest stores the age, or the number of times we've called "go"
    uint8_t flag_and_age;

    // dummy tt entry that allows us to find the size of the TT
    TT_Entry();
    TT_Entry(const Board &board, Move best_move, uint16_t score, uint8_t depth, uint32_t age, uint8_t flag);
    uint8_t flag();
    // moded to 64
    uint8_t age();
    bool can_use_score(int alpha, int beta);
};

class TranspositionTable
{
    std::vector<TT_Entry> hashtable;
    // number of elements stored in here
    uint64_t size = 0;

public:
    // Size of TT in MB
    TranspositionTable(uint64_t size);
    // clears the transposition table
    void resize(uint64_t size);
    void insert(const Board &board, Move best_move, uint16_t best_score, uint8_t depth, uint32_t age, uint8_t flag);
    // bool contains(uint64_t hash);
    TT_Entry &get(const Board &board);
};