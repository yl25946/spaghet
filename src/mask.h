#include "defs.h"

// for testing purposes:
extern uint64_t pawn_attacks[2][64];
extern uint64_t knight_attacks[64];
extern uint64_t king_attacks[64];

// leaper pieces
uint64_t mask_pawn_attacks(int side, int square);
uint64_t mask_knight_attacks(int square);
uint64_t mask_king_attacks(int square);

// sliding pieces
uint64_t mask_bishop_attacks(int square);
uint64_t mask_rook_attacks(int square);

// magic bitboard stuff
uint64_t bishop_attacks_on_the_fly(int square, uint64_t block);
uint64_t rook_attacks_on_the_fly(int square, uint64_t block);

// actual functions start here:
void init_leapers_attacks();