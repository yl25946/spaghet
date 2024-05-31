#include "search.h"

int max_depth = 255;

// Searcher::Searcher() : board(Board(start_position))
// {
//     this->board = Board(start_position);
//     this->end_time = UINT64_MAX;
// }

Searcher::Searcher(Board &board, std::vector<Move> &move_list, TranspositionTable &transposition_table, QuietHistory &history, uint32_t age) : board(board), transposition_table(transposition_table), history(history)
{
    threefold_repetition.push_back(board.hash);

    for (Move m : move_list)
    {
        board.make_move(m);
        // if (count_bits(board.bitboard(WHITE_KING)) == 2)
        //     board.print();
        threefold_repetition.push_back(board.hash);
    }

    this->board = board;
    this->age = age;
    this->transposition_table = transposition_table;
    this->history = history;
}

Searcher::Searcher(Board &board, std::vector<Move> &move_list, TranspositionTable &transposition_table, QuietHistory &history, uint32_t age, uint64_t end_time) : board(board), transposition_table(transposition_table), history(history)
{
    threefold_repetition.push_back(board.hash);

    for (Move m : move_list)
    {
        board.make_move(m);
        // if (count_bits(board.bitboard(WHITE_KING)) == 2)
        //     board.print();
        threefold_repetition.push_back(board.hash);
    }

    this->board = board;
    this->age = age;
    this->transposition_table = transposition_table;
    this->history = history;
    this->end_time = end_time;
}
// Searcher::Searcher(Board &board, std::vector<Move> &move_list, uint64_t end_time, uint8_t max_depth)
//     : board(board)
// {

//     for (Move m : move_list)
//     {
//         board.make_move(m);
//     }

//     this->board = board;
//     this->end_time = end_time;
//     this->max_depth = max_depth;
// }

// bool Searcher::is_checkmate(Board &board)
// {
//     return false;
// }

bool Searcher::threefold(Board &board)
{
    // in here, the board's hash is already added into the threefold_repetition
    uint64_t hash = board.hash;

    // the number of hashes matching the board argument's hash
    uint8_t matching_positions = 0;

    // index of the last element of the array
    size_t last_element_index = threefold_repetition.size() - 1;

    int threefold_max_it = std::min((size_t)board.fifty_move_counter, last_element_index);

    for (int i = 4; i <= threefold_max_it; i += 2)
    {
        if (hash == threefold_repetition[last_element_index - i])
            ++matching_positions;
    }

    // did not find a matching hash
    return matching_positions >= 2;
}

bool SEE(const Board &board, Move move, int threshold)
{
    if (move.is_castle())
        return threshold <= 0;

    uint8_t from_square = move.from_square();
    uint8_t to_square = move.to_square();
    uint8_t move_flag = move.move_flag();

    uint8_t target = move_flag == EN_PASSANT_CAPTURE ? PAWN : colored_to_uncolored(board.mailbox[to_square]);
    uint8_t promo = move.promotion_piece();
    int value = SEEValue[target] - threshold;

    // If we promote, we get the promoted piece and lose the pawn
    if (move.is_promotion())
        value += SEEValue[promo] - SEEValue[PAWN];

    // If we can't beat the threshold despite capturing the piece,
    // it is impossible to beat the threshold
    if (value < 0)
        return false;

    uint8_t attacker = colored_to_uncolored(board.mailbox[from_square]);
    // std::cout << "attacker" << static_cast<int>(attacker) << "\n";

    // If we get captured, we lose the moved piece,
    // or the promoted piece in the case of promotion
    value -= move.is_promotion() ? SEEValue[promo] : SEEValue[attacker];

    // If we still beat the threshold after losing the piece,
    // we are guaranteed to beat the threshold
    if (value >= 0)
        return true;

    // doesn't matter if the square is occupied or not
    uint64_t occupied = board.blockers();
    remove_bit(occupied, from_square);
    remove_bit(occupied, to_square);

    if (move_flag == EN_PASSANT_CAPTURE)
        remove_bit(occupied, board.en_passant_square + board.side_to_move == WHITE ? 8 : -8);

    // removed the piece on the from_square from the attackers bitboard, because we already used the piece to capture
    uint64_t attackers = board.attackers(to_square);
    remove_bit(attackers, from_square);
    // print_bitboard(attackers);

    // manually adds in the ep attacks
    if (move_flag == MOVE_FLAG::DOUBLE_PAWN_PUSH)
    {
        int en_passant_square = to_square + (board.side_to_move == COLOR::WHITE ? 8 : -8);
        attackers |= board.side_to_move == WHITE ? (pawn_attacks[BLACK][en_passant_square] & board.bitboard(WHITE_PAWN)) : (pawn_attacks[WHITE][en_passant_square] & board.bitboard(BLACK_PAWN));
        // we need to look a move ahead, so we use the same color
        attackers |= board.side_to_move == WHITE ? (pawn_attacks[WHITE][en_passant_square] & board.bitboard(BLACK_PAWN)) : (pawn_attacks[BLACK][en_passant_square] & board.bitboard(WHITE_PAWN));
    }

    // print_bitboard(attackers);
    uint64_t bishops = board.pieces[BITBOARD_PIECES::BISHOP] | board.pieces[BITBOARD_PIECES::QUEEN];
    uint64_t rooks = board.pieces[BITBOARD_PIECES::ROOK] | board.pieces[BITBOARD_PIECES::QUEEN];
    attackers |= get_bishop_attacks(to_square, occupied) & bishops;
    attackers |= get_rook_attacks(to_square, occupied) & rooks;

    uint8_t side = board.side_to_move ^ 1;

    // make captures until one sides run out, or fail to beat threshold
    while (true)
    {
        // removed used pieces from attackers
        attackers &= occupied;

        uint64_t my_attackers = attackers & board.colors[side];

        // print_bitboard(my_attackers);
        // print_bitboard(occupied);

        if (!my_attackers)
            break;

        // picks the next least valuable piece to capture with
        int piece_type;
        for (piece_type = BITBOARD_PIECES::PAWN; piece_type <= BITBOARD_PIECES::KING; ++piece_type)
        {
            if (my_attackers & board.pieces[piece_type])
                break;
        }

        side ^= 1;

        int see_value = SEEValue[piece_type];

        // std::cout << static_cast<int>(piece_type) << " " << static_cast<int>(rank(to_square)) << " " << static_cast<int>(to_square) << "\n";

        // bool is_promotion = piece_type == PAWN && (side ^ 1 == COLOR::WHITE ? rank(to_square) == 0 : rank(to_square) == 7);

        // if (is_promotion)
        // {
        //     see_value = SEEValue[BITBOARD_PIECES::QUEEN] - SEEValue[BITBOARD_PIECES::PAWN];
        // }

        // the -1 prioritizes moves that stop trading faster?
        value = -value - 1 - see_value;

        // value beats threshold, or can't beat threshold (negamaxed)
        if (value >= 0)
        {
            if (piece_type == KING && (attackers & board.colors[side]))
                side ^= 1;
            break;
        }

        // removed the used piece from the occupied
        occupied ^= 1ULL << (lsb(my_attackers & board.bitboard(uncolored_to_colored(piece_type, side ^ 1))));

        if (piece_type == BITBOARD_PIECES::PAWN || piece_type == BITBOARD_PIECES::BISHOP || piece_type == BITBOARD_PIECES::QUEEN)
            attackers |= get_bishop_attacks(to_square, occupied) & bishops;

        if (piece_type == BITBOARD_PIECES::ROOK || piece_type == BITBOARD_PIECES::QUEEN)
            attackers |= get_rook_attacks(to_square, occupied) & rooks;
    }

    return side != board.side_to_move;
}

int Searcher::quiescence_search(Board &board, int alpha, int beta, int ply, bool in_pv_node)
{
    // return evaluate(board);

    if (stopped)
        return 0;

    ++node_count;
    if (!(node_count & 4095))
        if (get_time() >= end_time)
        {
            stopped = true;
            return 0;
        }

    // we check if the TT has seen this before
    TT_Entry &entry = transposition_table.probe(board);

    // tt cutoff
    // if the entry matches, we can use the score, and the depth is the same or greater, we can just cut the search short
    if (!in_pv_node && entry.hash == board.hash && entry.can_use_score(alpha, beta))
    {
        return entry.usable_score(ply);
    }

    // creates a baseline
    int stand_pat = evaluate(board);

    if (ply >= MAX_PLY - 1)
        return stand_pat;

    if (stand_pat >= beta)
        return stand_pat; // fail soft

    if (alpha < stand_pat)
        alpha = stand_pat;

    int best_eval = stand_pat;
    // int capture_moves = 0;
    MoveList move_list;
    generate_capture_moves(board, move_list);

    Move best_move;

    const int original_alpha = alpha;

    // scores moves to order them
    move_list.score(board, transposition_table, history, killers, -7, ply);

    for (int i = 0; i < move_list.size(); ++i)
    {
        Board copy = board;
        OrderedMove curr_move = move_list.next_move();

        copy.make_move(curr_move);

        if (!copy.was_legal())
            continue;

        // do we need to check for checkmate in qsearch?
        // if (is_checkmate(copy))
        // {
        //     return -50000 + depth
        // }

        // qsearch SEE pruning
        // since we only generate capture moves, if the score of the move is negative, that means it did not pass the SEE threshold, so we can just stop the loop
        // since everything after it will also not pass the SEE threshold
        if (curr_move.score < 0)
            break;

        int current_eval = -quiescence_search(copy, -beta, -alpha, ply + 1, in_pv_node);

        if (stopped)
            return 0;

        if (current_eval > best_eval)
        {
            best_eval = current_eval;
            best_move = curr_move;

            // ++capture_moves;

            if (current_eval > alpha)
            {
                alpha = current_eval;
                if (alpha >= beta)
                {
                    break; // fail soft
                }
            }
        }
    }

    // add to TT
    uint8_t bound_flag = BOUND::EXACT;

    if (alpha >= beta)
    {
        // beta cutoff, fail high
        bound_flag = BOUND::FAIL_HIGH;
    }
    else if (alpha <= original_alpha)
    {
        // failed to raise alpha, fail low
        bound_flag = BOUND::FAIL_LOW;
    }
    transposition_table.insert(board, best_move, best_eval, 0, ply, age, bound_flag);

    // TODO: add check moves
    return best_eval;
}

int Searcher::negamax(Board &board, int alpha, int beta, int depth, int ply, bool in_pv_node, bool null_moved)
{
    ++node_count;

    if (stopped)
        return 0;

    if (!(node_count & 4095))
        if (get_time() >= end_time)
        {
            stopped = true;
            return 0;
        }

    bool in_root = ply <= 0;

    // cut the search short if there's a draw
    // if it's a draw at the root node, we'll play a null move
    if (!in_root && board.fifty_move_counter >= 100)
        return 0;

    // if there's a threefold draw
    if (!in_root && threefold(board))
    {
        // std::cout << "threefold repetition" << "\n";
        return 0;
    }

    // bool in_pv_node = beta - alpha > 1;

    // we check if the TT has seen this before
    TT_Entry &entry = transposition_table.probe(board);

    // tt cutoff
    // if the entry matches, we can use the score, and the depth is the same or greater, we can just cut the search short
    if (!in_pv_node && entry.hash == board.hash && entry.can_use_score(alpha, beta) && entry.depth >= depth)
    {
        return entry.usable_score(ply);
    }

    if (depth <= 0)
        return quiescence_search(board, alpha, beta, ply);

    int static_eval = evaluate(board);

    // apply reverse futility pruning
    if (!in_pv_node && !board.is_in_check() && depth <= DEPTH_MARGIN && static_eval - depth * MARGIN >= beta)
        return static_eval;

    // bailout if
    if (ply >= MAX_PLY - 1)
        return static_eval;

    // applies null move pruning
    if (!null_moved && !in_pv_node && !board.is_in_check() && !board.only_pawns(board.side_to_move) && static_eval >= beta)
    {

        Board copy = board;
        copy.make_null_move();

        // to help detect threefold in nmp
        threefold_repetition.push_back(copy.hash);

        int null_move_score = -negamax(copy, -beta, -beta + 1, depth - NULL_MOVE_DEPTH_REDUCTION, ply + 1, false, true);

        if (stopped)
            return 0;

        threefold_repetition.pop_back();

        if (null_move_score >= beta)
            return null_move_score;
    }

    MoveList move_list;
    MoveList quiet_moves;

    generate_moves(board, move_list);

    // scores moves to order them
    move_list.score(board, transposition_table, history, killers, -107, ply);

    int legal_moves = 0;
    int moves_seen = 0;

    const int original_alpha = alpha;

    // get pvs here
    int best_eval = -INF - 1;
    Move best_move;
    bool is_quiet;

    for (int i = 0; i < move_list.size(); ++i)
    {
        Board copy = board;
        Move curr_move = move_list.next_move();
        copy.make_move(curr_move);

        if (!copy.was_legal())
            continue;

        ++legal_moves;

        is_quiet = curr_move.is_quiet();

        if (!in_root && best_eval > MIN_MATE_SCORE)
        {
            // applies late move pruning
            if (is_quiet && moves_seen >= 3 + depth * depth)
                continue;

            // applies pvs see pruning
            const int see_threshold = is_quiet ? -80 * depth : -30 * depth * depth;

            if (depth <= 8 && moves_seen > 0 && !SEE(board, curr_move, see_threshold))
                continue;
        }

        if (is_quiet)
            quiet_moves.insert(curr_move);

        int current_eval;

        // don't do pvs on the first node
        if (moves_seen == 0)
        {
            // we can check for threefold repetition later, updates the state though
            threefold_repetition.push_back(copy.hash);

            current_eval = -negamax(copy, -beta, -alpha, depth - 1, ply + 1, in_pv_node, false);

            if (stopped)
                return 0;

            // stopped searching that line, so we can get rid of the hash
            threefold_repetition.pop_back();
        }
        else
        {
            // implements late move reduction
            // no reduction
            int reduction = 1;

            // applies the late move reduction
            if (moves_seen > 1)
            {
                if (is_quiet)
                    // legal moves - 1 counts the number of legal moves from 0
                    reduction += lmr_reduction_quiet(depth, moves_seen);
                // noisy move
                else
                    reduction += lmr_reduction_captures_promotions(depth, moves_seen);
            }

            // we can check for threefold repetition later, updates the state though
            threefold_repetition.push_back(copy.hash);

            // null windows search, basically checking if if returns alpha or alpha + 1 to indicate if there's a better move
            current_eval = -negamax(copy, -alpha - 1, -alpha, depth - reduction, ply + 1, false, false);

            if (stopped)
                return 0;

            // stopped searching that line, so we can get rid of the hash
            threefold_repetition.pop_back();

            // if this node raises alpha that means that we should investigate a bit more with a full length search, but still null-window
            // if this one fails high, using PVS we assume that it is a PV-node, so we re-search with a full window
            if (current_eval > alpha)
            {
                threefold_repetition.push_back(copy.hash);

                current_eval = -negamax(copy, -alpha - 1, -alpha, depth - 1, ply + 1, false, false);

                if (stopped)
                    return 0;

                threefold_repetition.pop_back();

                // pvs implementation, if we don 't have a fail low from that search, that means that our previous move wasn't our best move,
                // so we'll assume that this node is the pv move, and then do a full window search.
                if (current_eval > alpha && in_pv_node)
                {
                    threefold_repetition.push_back(copy.hash);

                    current_eval = -negamax(copy, -beta, -alpha, depth - 1, ply + 1, true, false);

                    if (stopped)
                        return 0;

                    // stopped searching that line, so we can get rid of the hash
                    threefold_repetition.pop_back();
                }
            }
        }

        ++moves_seen;

        if (stopped)
            return 0;

        // fail soft framework
        if (current_eval > best_eval)
        {
            best_eval = current_eval;
            best_move = curr_move;

            if (current_eval > alpha)
            {
                alpha = current_eval;
                best_move = curr_move;

                // fail high
                if (alpha >= beta)
                {

                    // std::cout << (int)curr_move.move_flag() << "\n";
                    // we update the history table if it's not a capture
                    if (is_quiet)
                    {
                        // std::cout << board.fen() << " " << curr_move.to_string() << "\n";
                        history.update(quiet_moves, curr_move, depth, board.side_to_move);
                        killers.insert(curr_move, ply);
                    }
                    break;
                }
            }
        }
    }

    // uncomment this if it doesn't work
    // write the best move down at the current depth
    if (ply == 0)
        this->current_depth_best_move = best_move;

    if (legal_moves == 0)
    {
        if (board.is_in_check())
        {
            // prioritize faster mates
            return -MATE + ply;
        }
        else
        {
            return 0;
        }
    }

    // add to TT
    uint8_t bound_flag = BOUND::EXACT;

    if (alpha >= beta)
    {
        // beta cutoff, fail high
        bound_flag = BOUND::FAIL_HIGH;
    }
    else if (alpha <= original_alpha)
    {
        // failed to raise alpha, fail low
        bound_flag = BOUND::FAIL_LOW;
    }
    if (best_eval != (-INF - 1))
        transposition_table.insert(board, best_move, best_eval, depth, ply, age, bound_flag);

    return best_eval;
}

void Searcher::search()
{
    int best_score;
    // used to track aspiration windows
    int guess;
    Move best_move;
    uint64_t time_elapsed;
    int alpha = -INF;
    int beta = INF;

    // this->start_time = get_time();
    this->node_count = 0;

    // generates a legal move in that position in case that we didn't finish depth one
    MoveList move_list;
    generate_moves(board, move_list);

    for (int i = 0; i < move_list.size(); ++i)
    {
        Board copy = board;
        copy.make_move(move_list.moves[i]);

        if (copy.was_legal())
        {
            best_move = move_list.moves[i];
            break;
        }
    }

    for (int current_depth = 1; current_depth <= max_depth; ++current_depth)
    {
        this->curr_depth = current_depth;

        Board copy = board;

        best_score = negamax(copy, alpha, beta, curr_depth, 0, true, false);

        // std::cout << get_time() << "\n"
        //           << end_time << "\n";

        if (stopped)
        {
            break;
        }

        guess = best_score;

        // tracks how many times we've had to adjust the aspiration window
        int aspiration_adjustments = 0;

        while (best_score <= alpha || best_score >= beta)
        {
            if (best_score <= alpha)
            {
                // debugging purposes
                int new_alpha = (guess - (25 * (1 << aspiration_adjustments)));

                alpha = std::clamp(new_alpha, static_cast<int>(-INF), alpha);
            }
            else if (best_score >= beta)
            {
                // debugging purposes
                int new_beta = (guess + (25 * (1 << aspiration_adjustments)));

                beta = std::clamp(new_beta, beta, static_cast<int>(INF));
            }

            Board copy = board;

            best_score = negamax(copy, alpha, beta, current_depth, 0, true, false);

            if (stopped)
                break;

            ++aspiration_adjustments;
            // std::cout << aspiration_adjustments << " " << alpha << " " << beta << "\n";
        }

        // std::cout << static_cast<int>(aspiration_adjustments) << " " << alpha << " " << beta << "\n";

        if (stopped)
            break;

        // updates alpha and beta
        alpha = best_score - 25;
        beta = best_score + 25;

        best_move = this->current_depth_best_move;

        time_elapsed = std::max(get_time() - start_time, (uint64_t)1);

        std::cout << "info score cp " << best_score << " depth " << (int)current_depth << " nodes " << node_count << " time " << time_elapsed << " nps " << (uint64_t)((double)node_count / time_elapsed * 1000) << " pv " << best_move.to_string() << std::endl;
    }

    // printf("bestmove %s\n", best_move.to_string().c_str());
    std::cout << "bestmove " << best_move.to_string() << std::endl;
}

// yoinked from stormphrax for tradition
void Searcher::bench()
{
    max_depth = 12;
    end_time = UINT64_MAX;
    node_count = 0;
    std::array<std::string, 50> Fens{// fens from alexandria, ultimately from bitgenie
                                     "r3k2r/2pb1ppp/2pp1q2/p7/1nP1B3/1P2P3/P2N1PPP/R2QK2R w KQkq a6 0 14",
                                     "4rrk1/2p1b1p1/p1p3q1/4p3/2P2n1p/1P1NR2P/PB3PP1/3R1QK1 b - - 2 24",
                                     "r3qbrk/6p1/2b2pPp/p3pP1Q/PpPpP2P/3P1B2/2PB3K/R5R1 w - - 16 42",
                                     "6k1/1R3p2/6p1/2Bp3p/3P2q1/P7/1P2rQ1K/5R2 b - - 4 44",
                                     "8/8/1p2k1p1/3p3p/1p1P1P1P/1P2PK2/8/8 w - - 3 54",
                                     "7r/2p3k1/1p1p1qp1/1P1Bp3/p1P2r1P/P7/4R3/Q4RK1 w - - 0 36",
                                     "r1bq1rk1/pp2b1pp/n1pp1n2/3P1p2/2P1p3/2N1P2N/PP2BPPP/R1BQ1RK1 b - - 2 10",
                                     "3r3k/2r4p/1p1b3q/p4P2/P2Pp3/1B2P3/3BQ1RP/6K1 w - - 3 87",
                                     "2r4r/1p4k1/1Pnp4/3Qb1pq/8/4BpPp/5P2/2RR1BK1 w - - 0 42",
                                     "4q1bk/6b1/7p/p1p4p/PNPpP2P/KN4P1/3Q4/4R3 b - - 0 37",
                                     "2q3r1/1r2pk2/pp3pp1/2pP3p/P1Pb1BbP/1P4Q1/R3NPP1/4R1K1 w - - 2 34",
                                     "1r2r2k/1b4q1/pp5p/2pPp1p1/P3Pn2/1P1B1Q1P/2R3P1/4BR1K b - - 1 37",
                                     "r3kbbr/pp1n1p1P/3ppnp1/q5N1/1P1pP3/P1N1B3/2P1QP2/R3KB1R b KQkq b3 0 17",
                                     "8/6pk/2b1Rp2/3r4/1R1B2PP/P5K1/8/2r5 b - - 16 42",
                                     "1r4k1/4ppb1/2n1b1qp/pB4p1/1n1BP1P1/7P/2PNQPK1/3RN3 w - - 8 29",
                                     "8/p2B4/PkP5/4p1pK/4Pb1p/5P2/8/8 w - - 29 68",
                                     "3r4/ppq1ppkp/4bnp1/2pN4/2P1P3/1P4P1/PQ3PBP/R4K2 b - - 2 20",
                                     "5rr1/4n2k/4q2P/P1P2n2/3B1p2/4pP2/2N1P3/1RR1K2Q w - - 1 49",
                                     "1r5k/2pq2p1/3p3p/p1pP4/4QP2/PP1R3P/6PK/8 w - - 1 51",
                                     "q5k1/5ppp/1r3bn1/1B6/P1N2P2/BQ2P1P1/5K1P/8 b - - 2 34",
                                     "r1b2k1r/5n2/p4q2/1ppn1Pp1/3pp1p1/NP2P3/P1PPBK2/1RQN2R1 w - - 0 22",
                                     "r1bqk2r/pppp1ppp/5n2/4b3/4P3/P1N5/1PP2PPP/R1BQKB1R w KQkq - 0 5",
                                     "r1bqr1k1/pp1p1ppp/2p5/8/3N1Q2/P2BB3/1PP2PPP/R3K2n b Q - 1 12",
                                     "r1bq2k1/p4r1p/1pp2pp1/3p4/1P1B3Q/P2B1N2/2P3PP/4R1K1 b - - 2 19",
                                     "r4qk1/6r1/1p4p1/2ppBbN1/1p5Q/P7/2P3PP/5RK1 w - - 2 25",
                                     "r7/6k1/1p6/2pp1p2/7Q/8/p1P2K1P/8 w - - 0 32",
                                     "r3k2r/ppp1pp1p/2nqb1pn/3p4/4P3/2PP4/PP1NBPPP/R2QK1NR w KQkq - 1 5",
                                     "3r1rk1/1pp1pn1p/p1n1q1p1/3p4/Q3P3/2P5/PP1NBPPP/4RRK1 w - - 0 12",
                                     "5rk1/1pp1pn1p/p3Brp1/8/1n6/5N2/PP3PPP/2R2RK1 w - - 2 20",
                                     "8/1p2pk1p/p1p1r1p1/3n4/8/5R2/PP3PPP/4R1K1 b - - 3 27",
                                     "8/4pk2/1p1r2p1/p1p4p/Pn5P/3R4/1P3PP1/4RK2 w - - 1 33",
                                     "8/5k2/1pnrp1p1/p1p4p/P6P/4R1PK/1P3P2/4R3 b - - 1 38",
                                     "8/8/1p1kp1p1/p1pr1n1p/P6P/1R4P1/1P3PK1/1R6 b - - 15 45",
                                     "8/8/1p1k2p1/p1prp2p/P2n3P/6P1/1P1R1PK1/4R3 b - - 5 49",
                                     "8/8/1p4p1/p1p2k1p/P2npP1P/4K1P1/1P6/3R4 w - - 6 54",
                                     "8/8/1p4p1/p1p2k1p/P2n1P1P/4K1P1/1P6/6R1 b - - 6 59",
                                     "8/5k2/1p4p1/p1pK3p/P2n1P1P/6P1/1P6/4R3 b - - 14 63",
                                     "8/1R6/1p1K1kp1/p6p/P1p2P1P/6P1/1Pn5/8 w - - 0 67",
                                     "1rb1rn1k/p3q1bp/2p3p1/2p1p3/2P1P2N/PP1RQNP1/1B3P2/4R1K1 b - - 4 23",
                                     "4rrk1/pp1n1pp1/q5p1/P1pP4/2n3P1/7P/1P3PB1/R1BQ1RK1 w - - 3 22",
                                     "r2qr1k1/pb1nbppp/1pn1p3/2ppP3/3P4/2PB1NN1/PP3PPP/R1BQR1K1 w - - 4 12",
                                     "2r2k2/8/4P1R1/1p6/8/P4K1N/7b/2B5 b - - 0 55",
                                     "6k1/5pp1/8/2bKP2P/2P5/p4PNb/B7/8 b - - 1 44",
                                     "2rqr1k1/1p3p1p/p2p2p1/P1nPb3/2B1P3/5P2/1PQ2NPP/R1R4K w - - 3 25",
                                     "r1b2rk1/p1q1ppbp/6p1/2Q5/8/4BP2/PPP3PP/2KR1B1R b - - 2 14",
                                     "6r1/5k2/p1b1r2p/1pB1p1p1/1Pp3PP/2P1R1K1/2P2P2/3R4 w - - 1 36",
                                     "rnbqkb1r/pppppppp/5n2/8/2PP4/8/PP2PPPP/RNBQKBNR b KQkq c3 0 2",
                                     "2rr2k1/1p4bp/p1q1p1p1/4Pp1n/2PB4/1PN3P1/P3Q2P/2RR2K1 w - f6 0 20",
                                     "3br1k1/p1pn3p/1p3n2/5pNq/2P1p3/1PN3PP/P2Q1PB1/4R1K1 w - - 0 23",
                                     "2r2b2/5p2/5k2/p1r1pP2/P2pB3/1P3P2/K1P3R1/7R w - - 23 93"};
    start_time = get_time();

    for (std::string &fen : Fens)
    {
        transposition_table.clear();
        this->board = Board(fen);
        history.clear();
        int alpha = -INF;
        int beta = INF;
        int best_score;
        for (int current_depth = 1; current_depth <= max_depth; ++current_depth)
        {
            this->curr_depth = current_depth;

            Board copy = board;

            best_score = negamax(copy, alpha, beta, curr_depth, 0, true, false);

            // std::cout << get_time() << "\n"
            //           << end_time << "\n";

            if (stopped)
            {
                break;
            }

            // tracks how many times we've had to adjust the aspiration window
            int aspiration_adjustments = 0;
            int guess = best_score;

            while (best_score <= alpha || best_score >= beta)
            {
                if (best_score <= alpha)
                {
                    // debugging purposes
                    int new_alpha = (alpha - (25 * std::pow(2, aspiration_adjustments)));

                    alpha = std::clamp(new_alpha, static_cast<int>(-INF), alpha);
                }
                else if (best_score >= beta)
                {
                    // debugging purposes
                    int new_beta = (alpha + (25 * std::pow(2, aspiration_adjustments)));

                    beta = std::clamp(new_beta, new_beta, static_cast<int>(INF));
                }

                Board copy = board;

                best_score = negamax(copy, alpha, beta, current_depth, 0, true, false);

                if (stopped)
                    break;

                ++aspiration_adjustments;
            }

            if (stopped)
                break;

            // updates alpha and beta
            alpha = best_score - 25;
            beta = best_score + 25;
        }
    }

    // time in seconds
    const uint64_t time = get_time() - start_time;

    std::cout << "info string " << time / 1000 << " seconds"
              << "\n";
    std::cout << node_count << " nodes " << (uint64_t)((double)node_count / time * 1000) << " nps"
              << "\n";
}
