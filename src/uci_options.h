#pragma once

#include "defs.h"
#include "transposition_table.h"

class UciOptions
{
public:
    bool minimal = false;
    int hash_size = 16;
    int age = 0;
    int threads = 1;
    TranspositionTable transposition_table;

    UciOptions() : transposition_table(hash_size, threads) {}

    // resizes the transposition_table
    void resize(size_t new_hash_size)
    {
        hash_size = new_hash_size;
        transposition_table.resize(new_hash_size, threads);
    }

    void reset()
    {
        hash_size = 16;
        age = 0;
        threads = 1;
        transposition_table.resize(hash_size, threads);
    }
};