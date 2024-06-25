#pragma once

#include "defs.h"

enum MOVE_FLAG
{
    QUIET_MOVE,
    DOUBLE_PAWN_PUSH,
    KING_CASTLE,
    QUEEN_CASTLE,
    CAPTURES,
    EN_PASSANT_CAPTURE,
    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE,
};

// use this to check if it is a promotion
constexpr uint8_t PROMOTION = 8;

class Move
{
public:
    uint16_t info;

    // undefined behavior, do not use
    Move() {};

    Move(uint8_t from, uint8_t to, uint8_t move_flag);

    bool operator==(const Move &move) const
    {
        return info == move.info;
    };

    bool operator!=(const Move &move) const
    {
        return info != move.info;
    }

    uint8_t from_square() const;
    uint8_t to_square() const;
    uint16_t from_to() const;
    uint8_t move_flag() const;

    bool is_quiet() const;
    bool is_castle() const;
    bool is_promotion() const;
    bool is_capture() const; // NOT ERROR CHECKED!
    // returns an uncolored pieec
    uint8_t promotion_piece() const;

    // bool is_null_move();

    // uci compliant
    std::string to_string() const;
    void print() const;
};

const Move NO_MOVE = Move(a8, a8, 0);

class OrderedMove : public Move
{
public:
    // Move move;
    // value of the move for sorting
    int32_t score;

    // undefined behavior, do not use
    OrderedMove() {};
    OrderedMove(Move move);
    OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag);
    OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag, int64_t score);

    // std::string to_string();
};