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
- Late Move Pruning (+33)
- Static Exchange Evaluation(SEE) Move Ordering (+20-48)
- SEE QS Pruning (+47)

# Testing
https://spaghet.pythonanywhere.com/

# TODO:
- simplification with a "skip_quiets" boolean flag, set that as true when enter lmp, and just skip quiets without doing lmp again
- Add check evasion to qSearch
- Add promotions in qSearch
- Add board.is_in_check() and copy.is_in_check() for late move pruning
- Create a search stack instead of 15 million flags
- Write legality check before making a move (usually gains 0-10 elo)
- Speed up multithreading using conditional

# Special Thanks
- The entirety of Stockfish Discord
- Zuppa, Ciekce, fury, and shawn_xu for dealing with me
- Everyone on my OB instance
- AGE for writing OB
- To Makism Korzh for writing BBC tutorial, which helped me write movegen with magic bitboards and precalculated lookup tables