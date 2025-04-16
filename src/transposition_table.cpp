#include "transposition_table.h"

uint64_t megabytes_to_bytes = 1 << 20;

TT_Entry::TT_Entry()
{
    hash = 0;
    score = SCORE_NONE;
    static_eval = SCORE_NONE;
    depth = 0;
    best_move = NO_MOVE;
    flag_and_age = BOUND::NONE;
}

TT_Entry::TT_Entry(const Board &board, Move best_move, int16_t score, int16_t static_eval, uint8_t depth, uint8_t ply, uint32_t age, uint8_t flag)
{
    this->hash = static_cast<uint16_t>(board.hash);
    this->best_move = best_move;
    this->static_eval = static_eval;
    this->depth = depth;

    // treat mate scores so that they're relative to the position instead of the root
    if (score >= TB_WIN_IN_MAX_PLY)
    {
        // the mate is relative to the root, we have to add the ply to account for the additional depth searched
        this->score = score + ply;
    }
    else if (score <= TB_LOSS_IN_MAX_PLY)
    {
        // same idea as above
        this->score = score - ply;
    }
    else
    {
        this->score = score;
    }

    // basically mod 64
    uint8_t modular_age = age & 63;
    this->flag_and_age = (modular_age << 2) | flag;
}

uint8_t TT_Entry::flag() const
{
    return this->flag_and_age & 3;
}

uint8_t TT_Entry::age() const
{
    return this->flag_and_age >> 2;
}

bool TT_Entry::can_use_score(int alpha, int beta) const
{
    uint8_t bound_flag = this->flag();
    return ((bound_flag == BOUND::FAIL_LOW && score <= alpha) ||
            (bound_flag == BOUND::FAIL_HIGH && score >= beta) || bound_flag == BOUND::EXACT);
}

int16_t TT_Entry::usable_score(int ply) const
{
    // readjusts the mating score so that it's now relative to the root instead of the position
    if (score <= TB_LOSS_IN_MAX_PLY)
    {
        // adds the depth to the root onto the mate
        // std::cout << score + ply << "\n";
        return score + ply;
    }
    else if (score >= TB_WIN_IN_MAX_PLY)
    {
        // std::cout << score - ply << "\n";
        // same idea as above
        return score - ply;
    }

    return score;
}

TranspositionTable::TranspositionTable(size_t mib, int thread_count)
{
    resize(mib, thread_count);
}

TranspositionTable::~TranspositionTable()
{
    if (hashtable != nullptr)
        std::free(hashtable);
}

void TranspositionTable::resize(size_t mib, int thread_count)
{
    if (hashtable != nullptr)
        std::free(hashtable);

    const size_t bytes = mib * 1024 * 1024;
    hashtable = static_cast<TT_Entry *>(std::malloc(bytes));
    size = mib * 1024 * 1024 / sizeof(TT_Entry);

    clear(thread_count);
}

void TranspositionTable::prefetch(const Board &board)
{
    const size_t hash_location = index(board);
    __builtin_prefetch(&hashtable[hash_location]);
}

void TranspositionTable::insert(const Board &board, Move best_move, int16_t best_score, int16_t static_eval, uint8_t depth, uint8_t ply, uint32_t age, uint8_t flag)
{
    uint64_t hash_location = index(board);

    TT_Entry &entry = hashtable[hash_location];

    // replacement policy
    if (entry.hash_equals(board) && flag != BOUND::EXACT && depth <= entry.depth - 4)
        return;

    // Takes the last 16 bits
    entry.hash = static_cast<uint16_t>(board.hash);
    entry.score = best_score;
    entry.static_eval = static_eval;
    entry.depth = depth;

    // only time we will not have a tt move is with a fail low
    if (flag != BOUND::FAIL_LOW)
        entry.best_move = best_move;

    // treat mate scores so that they're relative to the position instead of the root
    if (best_score >= TB_WIN_IN_MAX_PLY)
    {
        // the mate is relative to the root, we have to add the ply to account for the additional depth searched
        entry.score = best_score + ply;
    }
    else if (best_score <= TB_LOSS_IN_MAX_PLY)
    {
        // same idea as above
        entry.score = best_score - ply;
    }
    else
    {
        entry.score = best_score;
    }

    uint8_t modular_age = age & 63;
    entry.flag_and_age = (modular_age << 2) | flag;
}

TT_Entry &TranspositionTable::probe(const Board &board)
{
    size_t hash_location = index(board);

    return hashtable[hash_location];
}

void TranspositionTable::clear(int thread_count)
{
    std::vector<std::jthread> threads{};
    threads.reserve(thread_count);

    const size_t chunk_size = ceil_division(size, thread_count);

    for (int i = 0; i < thread_count; ++i)
    {
        threads.emplace_back([this, chunk_size, i]
                             { const int start = chunk_size * i;
                             const int end = std::min(start + chunk_size, size);
                             
                            const int count = end - start;
                            
                            std::memset(&hashtable[start], 0, count * sizeof(TT_Entry)); });
    }

    for (std::jthread &thread : threads)
        thread.join();
}

int TranspositionTable::hash_full()
{
    size_t filled_entries = 0;

    for (int i = 0; i < 1000; ++i)
    {
        TT_Entry &entry = hashtable[i];

        if (entry.flag() != BOUND::NONE)
            ++filled_entries;
    }

    return filled_entries;
}