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
    QUEEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE,
};

// use this to check if it is a promotion
const uint8_t PROMOTION = 8;

class Move
{
public:
    uint16_t info;

    // undefined behavior, do not use
    Move() {};

    Move(uint8_t from, uint8_t to, uint8_t move_flag);

    uint8_t from_square();
    uint8_t to_square();
    uint8_t move_flag();

    // bool is_null_move();

    // uci compliant
    std::string to_string();
    void print();
};

class OrderedMove : public Move
{
public:
    // Move move;
    // value of the move for sorting
    uint16_t value = 0;

    // default constructor, creates a null move with value 0
    OrderedMove();
    OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag);
    OrderedMove(uint8_t from, uint8_t to, uint8_t move_flag, uint16_t value);

    // std::string to_string();
};