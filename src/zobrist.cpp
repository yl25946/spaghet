#include "zobrist.h"

uint64_t zobrist_pieces[12][64];

uint64_t zobrist_castling_rights[16];

uint64_t zobrist_side_to_move;

uint64_t zobrist_en_passant[8];

void init_zobrist_piece_table()
{
    for (int i = 0; i < 12; ++i)
    {
        for (int j = 0; j < 64; ++j)
        {
            zobrist_pieces[i][j] = random_uint64();
        }
    }
}

void init_zobrist_castling_table()
{
    uint64_t white_OO = random_uint64();
    uint64_t white_OOO = random_uint64();
    uint64_t black_OO = random_uint64();
    uint64_t black_OOO = random_uint64();

    for (int i = 0; i < 16; ++i)
    {
        zobrist_castling_rights[i] = 0;

        if (i & WHITE_KING_CASTLE)
            zobrist_castling_rights[i] ^= white_OO;
        if (i & WHITE_QUEEN_CASTLE)
            zobrist_castling_rights[i] ^= white_OOO;
        if (i & BLACK_KING_CASTLE)
            zobrist_castling_rights[i] ^= black_OO;
        if (i & BLACK_QUEEN_CASTLE)
            zobrist_castling_rights[i] ^= black_OOO;
    }
}

void init_zobrist_enpassant_table()
{
    for (int i = 0; i < 8; ++i)
    {
        zobrist_en_passant[i] = random_uint64();
    }
}

void init_zobrist_side_to_move()
{
    zobrist_side_to_move = random_uint64();
}

void init_zobrist_tables()
{
    init_zobrist_piece_table();
    init_zobrist_castling_table();
    init_zobrist_enpassant_table();
    init_zobrist_side_to_move();
}