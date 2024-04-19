# spaghet
Chess engine based on BBC


# Implements:
- Bitboards
- Pregenerated Attack Lookup Tables
- Pregenerated Magic Bitboards (self-generated)
- Copy-Make
- Legality check after making a move
- Pseudolegal Bitboard Move Generation (8.8 million nodes per second on an i7-9700F)

# TODO:
- Write legality check before making a move (usually gains 0-10 elo)