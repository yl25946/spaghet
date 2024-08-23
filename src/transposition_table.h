#pragma once

#include "defs.h"
#include "board.h"
#include "move.h"

// allows us to convert mate scores into a usable format that we can use to detect checkmates
int ttscore_to_score(uint16_t tt_score, int ply);

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
    int16_t static_eval;

    // dummy tt entry that allows us to find the size of the TT
    TT_Entry();
    TT_Entry(const Board &board, Move best_move, int16_t score, int16_t static_eval, uint8_t depth, uint8_t ply, uint32_t age, uint8_t flag);
    uint8_t flag() const;
    // moded to 64
    uint8_t age() const;
    bool can_use_score(int alpha, int beta) const;
    // converts the score into the TT into a score that can used to detect and play mates
    int16_t usable_score(int ply) const;
};

class TranspositionTable
{
    std::vector<TT_Entry> hashtable;
    // number of elements stored in here
    uint64_t size = 0;

public:
    // Size of TT in MB
    TranspositionTable(size_t size);
    // clears the transposition table
    void resize(size_t size);
    void prefetch(const Board &board);
    void insert(const Board &board, Move best_move, int16_t best_score, int16_t static_eval, uint8_t depth, uint8_t ply, uint32_t age, uint8_t flag);
    void clear();
    // prints out permille what percent of the hashtable is full
    int hash_full();
    TT_Entry &probe(const Board &board);

private:
    inline size_t index(const Board &board) { return board.hash % hashtable.size(); };
};