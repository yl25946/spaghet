#pragma once

#include "defs.h"
#include "move.h"

class MoveList
{
public:
    std::array<OrderedMove, 256> moves;
    uint8_t count = 0;

    uint8_t size() const;

    // MoveList();

    // move order value defaults to 0
    void insert(Move move);
    void insert(uint8_t from, uint8_t to, uint8_t move_flag);
    void insert(uint8_t from, uint8_t to, uint8_t move_flag, uint16_t value);

    // gets the next move with the highest value
    void pop_back() { --count; }
    void clear() { count = 0; }
    void copy_over(MoveList &move_list);

    Move operator[](int index) const { return moves[index]; }

    std::string to_string();
    // iterates through the moves backwards (used for PV Tables)
    std::string reverse_to_string();
    void print();

    using iterator = typename std::array<OrderedMove, 256>::iterator;
    using const_iterator = typename std::array<OrderedMove, 256>::const_iterator;

    iterator begin() { return moves.begin(); }
    iterator end() { return moves.begin() + count; } // Only iterate over valid moves

    const_iterator begin() const { return moves.begin(); }
    const_iterator end() const { return moves.begin() + count; }

    const_iterator cbegin() const { return moves.cbegin(); }
    const_iterator cend() const { return moves.cbegin() + count; }

    // Reverse iterators
    using reverse_iterator = typename std::array<OrderedMove, 256>::reverse_iterator;
    using const_reverse_iterator = typename std::array<OrderedMove, 256>::const_reverse_iterator;

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
};