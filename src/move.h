#pragma once

#include "defs.h"

enum move_flags
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
    Move(){};

    Move(uint8_t from, uint8_t to, uint8_t move_flag);

    uint8_t from_square();
    uint8_t to_square();
    uint8_t move_flag();

    // uci compliant
    std::string to_string();
    void print();
};