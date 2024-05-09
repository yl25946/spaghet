#include "transposition_table.h"

uint64_t megabytes_to_bytes = 1 << 20;

TT_Entry::TT_Entry()
{
    hash = 0;
}

TT_Entry::TT_Entry(const Board &board, Move best_move, int16_t score, uint8_t depth, uint32_t age, uint8_t flag)
{
    this->hash = board.hash;
    this->best_move = best_move;
    this->depth = depth;
    this->score = score;

    // basically mod 64
    uint8_t modular_move_counter = age & 63;
    this->flag_and_age = (modular_move_counter << 2) | flag;
}

uint8_t TT_Entry::flag()
{
    return this->flag_and_age & 3;
}

uint8_t TT_Entry::age()
{
    return this->flag_and_age >> 2;
}

bool TT_Entry::can_use_score(int alpha, int beta)
{
    uint8_t bound_flag = this->flag();
    return score != BOUND::NONE && ((bound_flag == BOUND::FAIL_LOW && score <= alpha) ||
                                    (bound_flag == BOUND::FAIL_HIGH && score >= beta) || bound_flag == BOUND::EXACT);
}

TranspositionTable::TranspositionTable(uint64_t size)
{
    uint64_t entry_size = sizeof(TT_Entry);

    uint64_t tt_entry_count = (size * megabytes_to_bytes) / entry_size;

    // size is now 0
    hashtable.clear();

    this->hashtable.resize(tt_entry_count, TT_Entry());
}

void TranspositionTable::resize(uint64_t size)
{
    uint64_t entry_size = sizeof(TT_Entry);

    uint64_t tt_entry_count = (size * megabytes_to_bytes) / entry_size;

    // size is now 0
    hashtable.clear();

    hashtable.resize(tt_entry_count, TT_Entry());
}

void TranspositionTable::insert(const Board &board, Move best_move, int16_t best_score, uint8_t depth, uint32_t age, uint8_t flag)
{
    uint64_t hash_location = board.hash % hashtable.size();

    // TODO: ADD BETTER TT REPLACEMENT ALGO
    hashtable[hash_location] = TT_Entry(board, best_move, best_score, depth, age, flag);
}

// bool TranspositionTable::contains(const Board &board)
// {
//     uint64_t hash_location = hash % hashtable.size();

//     TT_Entry &entry = hashtable[hash_location];

//     return entry.hash == hash;
// }

TT_Entry &TranspositionTable::probe(const Board &board)
{
    uint64_t hash_location = board.hash % hashtable.size();

    return hashtable[hash_location];
}

void TranspositionTable::clear()
{
    uint64_t original_size = hashtable.size();

    hashtable.clear();

    hashtable.resize(original_size);
}