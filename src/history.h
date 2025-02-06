#pragma once

#include "defs.h"
#include "move.h"
#include "board.h"
#include "movelist.h"
// #include "movepicker.h"

inline int history_bonus(int depth) { return std::min(170 * depth, 1500); }

inline int history_malus(int depth) { return std::max(-450 * depth, -1500); }

inline int corrhist_bonus(int score, int static_eval, int depth)
{
    const int delta = score - static_eval;
    return std::clamp(delta * depth / 8, -CORRHIST_LIMIT / 4, CORRHIST_LIMIT / 4);
}

template <typename T, int limit, T starting_value>
class HistoryEntry
{
    T value = starting_value;

public:
    HistoryEntry &operator=(const T &v)
    {
        value = v;
        return *this;
    }

    operator int() const
    {
        return value;
    }

    T operator&() const
    {
        return value;
    }

    void operator<<(int bonus)
    {
        int clamped_bonus = std::clamp(bonus, -limit, limit);
        value += clamped_bonus - value * abs(clamped_bonus) / limit;
    }
};

// [stm][from][to]
using QuietHistory = std::array<std::array<std::array<HistoryEntry<int16_t, MAX_HISTORY, 0>, 64>, 64>, 2>;
// [mod pawnhash][piece][to]
using PawnHistory = std::array<std::array<std::array<HistoryEntry<int16_t, MAX_HISTORY, 0>, 64>, 12>, PAWNHIST_SIZE>;
// [capturing piece][to][captured piece (uncolored)]
using CaptureHistory = std::array<std::array<std::array<HistoryEntry<int16_t, MAX_HISTORY, 0>, 7>, 64>, 12>;
// [previous piece][previous to][piece][to]
using ContinuationHistory = std::array<std::array<std::array<std::array<HistoryEntry<int16_t, MAX_HISTORY, 0>, 64>, 13>, 64>, 13>;
// [stm][mod pawn hash]
using PawnCorrectionHistory = std::array<std::array<HistoryEntry<int16_t, CORRHIST_LIMIT, 0>, PAWN_CORRHIST_SIZE>, 2>;
// [stm][mod material hash]
using MaterialCorrectionHistory = std::array<std::array<HistoryEntry<int16_t, CORRHIST_LIMIT, 0>, MATERIAL_CORRHIST_SIZE>, 2>;

// class PawnCorrectionHistory
// {
//     // [stm][mod pawn hash]
//     std::array<std::array<int64_t, PAWN_CORRHIST_SIZE>, 2> table;

// public:
//     PawnCorrectionHistory();

//     // filters out mate scores internally
//     void update(const Board &board, int depth, int score, int static_eval);

//     // only returns the correction, not the evaluation
//     int correction(const Board &board);
// };

// class MaterialCorrectionHistory
// {
//     // [stm][mod of material hash]
//     std::array<std::array<int64_t, MATERIAL_CORRHIST_SIZE>, 2> table;

// public:
//     MaterialCorrectionHistory();

//     // filters out mate scores internally
//     void update(const Board &board, int depth, int score, int static_eval);

//     // only returns the correction, not the evaluation
//     int correction(const Board &board);
// };