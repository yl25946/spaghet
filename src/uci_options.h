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

    UciOptions() : transposition_table(hash_size) {}

    // resizes the transposition_table
    void resize(int new_hash_size)
    {
        hash_size = new_hash_size;
        transposition_table.resize(new_hash_size);
    }

    void reset()
    {
        hash_size = 16;
        age = 0;
        threads = 0;
        transposition_table.resize(hash_size);
    }
};