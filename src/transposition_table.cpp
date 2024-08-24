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

TT_Entry::TT_Entry(const Board &board, Move best_move, int16_t score, int16_t static_eval, uint8_t depth, uint8_t ply, uint32_t age, bool inPV, uint8_t flag)
{
    this->hash = static_cast<uint16_t>(board.hash);
    this->best_move = best_move;
    this->static_eval = static_eval;
    this->depth = depth;

    // treat mate scores so that they're relative to the position instead of the root
    if (score >= MAX_MATE_SCORE)
    {
        // the mate is relative to the root, we have to add the ply to account for the additional depth searched
        this->score = score + ply;
    }
    else if (score <= MIN_MATE_SCORE)
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
    this->flag_and_age = (modular_age << 3) | inPV << 1 | flag;
}

uint8_t TT_Entry::flag() const
{
    return flag_and_age & 3;
}

bool TT_Entry::ttPV() const
{
    // gets the 3rd bit
    return flag_and_age & 4;
}

uint8_t TT_Entry::age() const
{
    return this->flag_and_age >> 3;
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
    if (score <= MIN_MATE_SCORE)
    {
        // adds the depth to the root onto the mate
        // std::cout << score + ply << "\n";
        return score + ply;
    }
    else if (score >= MAX_MATE_SCORE)
    {
        // std::cout << score - ply << "\n";
        // same idea as above
        return score - ply;
    }

    return score;
}

TranspositionTable::TranspositionTable(size_t size)
{
    size_t entry_size = sizeof(TT_Entry);

    size_t tt_entry_count = (size * megabytes_to_bytes) / entry_size;

    // size is now 0
    hashtable.clear();

    this->hashtable.resize(tt_entry_count);
}

void TranspositionTable::resize(size_t size)
{
    size_t entry_size = sizeof(TT_Entry);

    size_t tt_entry_count = (size * megabytes_to_bytes) / entry_size;

    // std::cout << tt_entry_count << "\n";

    // size is now 0
    hashtable.clear();

    hashtable.resize(tt_entry_count, TT_Entry());
}

void TranspositionTable::prefetch(const Board &board)
{
    const size_t hash_location = index(board);
    __builtin_prefetch(&hashtable[hash_location]);
}

void TranspositionTable::insert(const Board &board, Move best_move, int16_t best_score, int16_t static_eval, uint8_t depth, uint8_t ply, uint32_t age, bool inPV, uint8_t flag)
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
    if (best_score >= MAX_MATE_SCORE)
    {
        // the mate is relative to the root, we have to add the ply to account for the additional depth searched
        entry.score = best_score + ply;
    }
    else if (best_score <= MIN_MATE_SCORE)
    {
        // same idea as above
        entry.score = best_score - ply;
    }
    else
    {
        entry.score = best_score;
    }

    uint8_t modular_age = age & 31;
    entry.flag_and_age = (modular_age << 3) | inPV << 1 | flag;
}

TT_Entry &TranspositionTable::probe(const Board &board)
{
    size_t hash_location = index(board);

    return hashtable[hash_location];
}

void TranspositionTable::clear()
{
    size_t original_size = hashtable.size();

    hashtable.clear();

    hashtable.resize(original_size);
}

int TranspositionTable::hash_full()
{
    return 0;
}