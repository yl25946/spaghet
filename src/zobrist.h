#pragma once

#include "defs.h"
#include "utils.h"

// Zobrist table for pieces [pieces][square]
extern uint64_t zobrist_pieces[12][64];

// Zobrist table for castling rights, takes in a single int from the board's castling right uint8_t,
// and maps to the zobrist hash  [castling rights]
extern uint64_t zobrist_castling_rights[16];

// only mask if its black side to move
// just xor every time you make a move, they cancel out
extern uint64_t zobrist_side_to_move;

// encodes the file of the en_passant square [file]
extern uint64_t zobrist_en_passant[8];

// initializes the zobrist tables
void init_zobrist_tables();