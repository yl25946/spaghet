#include "transposition_table.h"

uint64_t megabytes_to_bytes = 1 << 20;

TT_Entry::TT_Entry()
{
}

TT_Entry::TT_Entry(uint64_t hash, Move best_move, uint16_t best_score, uint8_t depth, uint32_t age, Bound flag)
{
    this->hash = hash;
    this->best_move = best_move;
    this->depth = depth;

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

TranspositionTable::TranspositionTable(uint64_t size)
{
    TT_Entry Dummy;

    uint64_t entry_size = sizeof(Dummy);

    uint64_t tt_entry_count = (size * megabytes_to_bytes) / entry_size;

    this->hashtable.resize(tt_entry_count);
}

void TranspositionTable::insert(const TT_Entry &entry)
{
    uint64_t hash_location = entry.hash % hashtable.size();

    // TODO: ADD BETTER TT REPLACEMENT ALGO
    hashtable[hash_location] = entry;
}

bool TranspositionTable::contains(uint64_t hash)
{
    uint64_t hash_location = hash % hashtable.size();

    TT_Entry &entry = hashtable[hash_location];

    return entry.hash == hash;
}

TT_Entry &TranspositionTable::get(uint64_t hash)
{
    uint64_t hash_location = hash % hashtable.size();

    return hashtable[hash_location];
}
