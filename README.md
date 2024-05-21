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
- Principal Variation Search
- Null Move Pruning 
- Reverse Futility Pruning
- History Heuristic
- Killer Heuristic
- Late Move Reduction
- Null move reduction when null move pruning is not possible

# ELO Gainers
Started from piece counting and standard negamax
- PeSTO's Evaluation Function (+176)
- Alpha-Beta Pruning (+340)
- Quiescence Search with Alpha-Beta (+324)
- 3 fold detection with Zobrist Hashing (+26)
- Transposition Table cutoffs (+17.3)
- TT move ordering (+115)
- MVV-LVA move ordering (+317)
- Principle Variation Search (+20)
- Null Move Pruning (+136)
- Reverse Futility Pruning (+17)
- Fail High in Null Move Pruning (+6.5)
- History Heuristic (+99)
- History Heuristic Bug Fix (only adding non-captures) (+47)
- Killers (+25.7)
- Aspiration Windows (+34)
- malus + persistent history (+10.65)


# Testing
https://spaghet.pythonanywhere.com/

# TODO:
- Add check evasion to qSearch
- Add promotions in qSearch
- Add TT-cutoffs in qSearch
- Create a search stack instead of 15 million flags
- Write legality check before making a move (usually gains 0-10 elo)

# Special Thanks
- The entirety of Stockfish Discord
- Zuppa, Ciekce, fury, and shawn_xu for dealing with me
- AGE for writing OB
- To Makism Korzh for writing BBC tutorial, which helped me write movegen with magic bitboards and precalculated lookup tables