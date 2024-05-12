# spaghet
Chess engine based on BBC


# Implements:
- Bitboards
- Pregenerated Attack Lookup Tables
- Pregenerated Magic Bitboards (self-generated)
- Copy-Make
- Legality check after making a move
- Pseudolegal Bitboard Move Generation (42.7 million nodes per second on an i7-9700F)
- PeSTO evaluation function
- negamax search algorithm
- alpha-beta pruning
- quiescence search
- Zobrist hashing
- Transposition Tables
- Move Ordering using MVV-LVA

# ELO Gainers
Started from piece counting and standard negamax
- PeSTO's Evaluation Function (+176)
- Alpha-Beta Pruning (+340)
- Quiescence Search with Alpha-Beta (+324)
- 3 fold detection with Zobrist Hashing (+14.7)
- Transposition Table cutoffs (+17.3)
- TT move ordering (+115)
- MVV-LVA move ordering (+317)


# Testing
https://spaghet.pythonanywhere.com/

# TODO:
- Add check evasion to qSearch
- Write legality check before making a move (usually gains 0-10 elo)

# Special Thanks
- Zuppa for dealing with me
- AGE for writing OB