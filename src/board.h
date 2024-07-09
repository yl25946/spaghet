#pragma once

#include "defs.h"
#include "utils.h"
#include "attacks.h"
#include "move.h"
#include "zobrist.h"

// stolen from nek.saikou on Discord
class Board
{
public:
    std::array<uint64_t, 6> pieces;
    std::array<uint64_t, 2> colors;

    // mailbox for speedup
    std::array<uint8_t, 64> mailbox;

    uint8_t side_to_move = WHITE;
    uint8_t en_passant_square = no_square;
    // castling rights,
    uint8_t rights = 0;

    uint64_t hash = 0;
    uint64_t pawn_hash = 0;

    // checks 50 move rule, counts in plys
    uint8_t fifty_move_counter = 0;
    // counts the total number of moves
    uint16_t half_move_counter = 0;

    //   std::array<Accumulator, 2> acc;

    // default Board so the compiler doesn't hate us
    Board();

    // creates a board object with a fen
    Board(const std::string &fen);

    // converts half_move_counter to full_move_counter
    uint16_t full_move_counter() const;

    // gets all the pieces, or all the blockers, of the board
    uint64_t blockers() const;

    // gets WHITE_PAWN, BLACK_PAWN, etc.
    uint64_t bitboard(uint8_t piece) const;

    // help method to get all the possible attacks from a certain side
    // uint64_t get_attack_mask(uint8_t side_attacking);

    // checks whether a given square is attacked by the given side
    bool is_square_attacked(uint8_t square, uint8_t side_attacking) const;
    // gets all the pieces attacking that square, both sides, doesn't not include the square itself
    uint64_t attackers(uint8_t square) const;

    // checks if a move is pseudolegal
    bool is_pseudolegal(Move move) const;

    // checks after a move if we have a valid board state
    // if white is checking the king while it's white's turn to move, we lose
    bool was_legal() const;

    // returns true if the current move's king is in check
    bool is_in_check();

    // checks if there's only pawns (and king) on the board for the side to move
    // used as a rudimentary check for zugzwang
    bool only_pawns(uint8_t side_to_move) const;

    void make_move(Move move);

    // creates a null move, do this for easy debugging
    void make_null_move();

    // converts position into a fen
    std::string fen() const;

    // prints out the bitboard into a human-readable format
    void print() const;

    // prints out all the squares that are attacked by a given side
    void print_attacked_squares(uint8_t side_attacking) const;
};