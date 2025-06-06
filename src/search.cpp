#include "search.h"

Searcher::Searcher(Board board, const std::vector<Move> &move_list, TranspositionTable &transposition_table, ThreadData &thread_data, ThreadManager &thread_manager, uint32_t age, bool is_main_thread) : board(board), transposition_table(transposition_table), thread_data(thread_data), thread_manager(thread_manager)
{
    // reserves enough space so we don't have to resize
    game_history.reserve(300 + MAX_PLY);

    game_history.push_back(board.hash);

    for (Move m : move_list)
    {
        board.make_move(m);
        game_history.push_back(board.hash);
    }

    for (int i = 0; i < 64; ++i)
        for (int j = 0; j < 64; ++j)
            nodes_spent_table[i][j] = 0;

    this->board = board;

    thread_data.search_stack[4].board = board;
    thread_data.search_stack[4].in_check = board.is_in_check();
    thread_data.accumulators[0] = Accumulator(board);
    thread_data.search_stack[4].updated_accumulator = true;

    this->age = age;
    this->is_main_thread = is_main_thread;
}

bool Searcher::twofold(Board &board)
{
    // in here, the board's hash is already added into the game_history
    uint64_t hash = board.hash;

    // index of the last element of the array
    size_t last_element_index = game_history.size() - 1;

    int threefold_max_it = std::min((size_t)board.fifty_move_counter, last_element_index);

    for (int i = 4; i <= threefold_max_it; i += 2)
    {
        if (hash == game_history[last_element_index - i])
            return true;
    }

    // did not find a matching hash
    return false;
}

void Searcher::scale_time(int best_move_stability_factor)
{
    constexpr double best_move_scale[5] = {2.43, 1.35, 1.09, 0.88, 0.68};
    const Move best_move = thread_data.search_stack[4].pv[0];
    const double not_best_move_nodes_fraction = 1.0 - static_cast<double>(nodes_spent_table[best_move.from_square()][best_move.to_square()]) / static_cast<double>(nodes);
    const double node_scaling_factor = std::max(2.3 * not_best_move_nodes_fraction + 0.45, 0.55);
    const double best_move_scaling_factor = best_move_scale[best_move_stability_factor];
    // scal9e the time based on how many nodes we spent ond how the best move changed
    optimum_stop_time = std::min<uint64_t>(start_time + optimum_stop_time_duration * node_scaling_factor * best_move_scaling_factor, max_stop_time);
}

void Searcher::update_conthist(SearchStack *ss, Move move, int bonus)
{
    {
        int ply = ss->ply;

        for (int conthist_index : conthist_indices)
            if (ply >= conthist_index && !(ss - conthist_index)->null_moved)
            {
                Board &previous_board = (ss - conthist_index)->board;
                Move previous_move = (ss - conthist_index)->move_played;
                thread_data.conthist[previous_board.moving_piece(previous_move)][previous_move.to_square()][ss->board.moving_piece(move)][move.to_square()] << bonus;
            }
    }
}
void Searcher::update_histories(SearchStack *ss, MoveList &noisies, MoveList &quiets, Move fail_high_move, int depth)
{
    for (Move noisy_move : noisies)
    {
        int bonus = (noisy_move == fail_high_move) ? history_bonus(depth) : history_malus(depth);

        thread_data.capthist[ss->board.moving_piece(noisy_move)][noisy_move.to_square()][colored_to_uncolored(ss->board.captured_piece(noisy_move))] << bonus;
    }
    for (Move quiet_move : quiets)
    {
        int bonus = (quiet_move == fail_high_move) ? history_bonus(depth) : history_malus(depth);

        update_conthist(ss, quiet_move, bonus);

        thread_data.main_history[ss->board.side_to_move][quiet_move.from_square()][quiet_move.to_square()] << bonus;
        thread_data.pawnhist[ss->board.pawn_hash % PAWNHIST_SIZE][ss->board.moving_piece(quiet_move)][quiet_move.to_square()] << bonus;
    }
}

void Searcher::update_corrhists(const Board &board, int score, int static_eval, int depth)
{
    if (is_mate_score(score))
        return;

    const int bonus = corrhist_bonus(score, static_eval, depth);

    thread_data.pawn_corrhist[board.side_to_move][board.pawn_hash % PAWN_CORRHIST_SIZE] << bonus;
    thread_data.material_corrhist[board.side_to_move][board.material_hash % MATERIAL_CORRHIST_SIZE] << bonus;
}

int Searcher::correct_static_eval(const Board &board, int uncorrected_static_eval)
{
    const int pawn_raw_correction = thread_data.pawn_corrhist[board.side_to_move][board.pawn_hash % PAWN_CORRHIST_SIZE];
    const int pawn_correction = pawn_raw_correction * std::abs(pawn_raw_correction) / 5'000;

    const int material_raw_correction = thread_data.material_corrhist[board.side_to_move][board.material_hash % MATERIAL_CORRHIST_SIZE];
    const int material_correction = material_raw_correction * std::abs(material_raw_correction) / 5'000;

    const int correction = (2 * pawn_correction + material_correction) / 3;

    return std::clamp(uncorrected_static_eval + correction, TB_LOSS_IN_MAX_PLY + 1, TB_WIN_IN_MAX_PLY - 1);
}

template <bool inPV>
int Searcher::quiescence_search(int alpha, int beta, SearchStack *ss)
{
    // return evaluate(board);

    if (stopped)
        return 0;

    if (ss->ply > seldepth)
        seldepth = ss->ply;

    ++nodes;
    if (!(nodes & 4095))
        if (get_time() >= max_stop_time)
        {
            stopped = true;
            return 0;
        }

    Board &board = ss->board;

    // we check if the TT has seen this before
    TT_Entry tt_entry = transposition_table.probe(board);

    bool tt_hit = !ss->exclude_tt_move && tt_entry.hash_equals(board) && tt_entry.flag() != BOUND::NONE;
    Move tt_move = ss->exclude_tt_move ? NO_MOVE : tt_entry.best_move;

    // tt cutoff
    // if the tt_entry matches, we can use the score, and the depth is the same or greater, we can just cut the search short
    if (!inPV && !ss->exclude_tt_move && tt_entry.hash_equals(board) && tt_entry.can_use_score(alpha, beta))
    {
        return tt_entry.usable_score(ss->ply);
    }

    // creates a baseline
    const int uncorrected_static_eval = tt_hit ? tt_entry.static_eval : evaluate(board, thread_data.accumulators, ss);
    const int stand_pat = tt_hit ? tt_entry.score : correct_static_eval(board, uncorrected_static_eval);

    if (ss->ply >= MAX_PLY - 1)
        return stand_pat;

    if (stand_pat >= beta)
        return stand_pat; // fail soft

    if (alpha < stand_pat)
        alpha = stand_pat;

    int best_score = stand_pat;
    // int capture_moves = 0;
    MoveList move_list;
    generate_capture_moves(board, move_list);

    // creates a "garbage" move so that when we read from the TT we don't accidentally order a random move first during scoring
    Move best_move = NO_MOVE;

    const int original_alpha = alpha;

    // scores moves to order them
    MovePicker move_picker(move_list);
    move_picker.score(ss, thread_data, tt_move, tt_hit, -107);

    while (move_picker.has_next())
    {
        Board copy = board;
        OrderedMove curr_move = move_picker.next_move();

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

        (ss + 1)->board = copy;
        (ss + 1)->updated_accumulator = false;
        (ss)->move_played = curr_move;

        // prefetch the TT entry
        transposition_table.prefetch(copy);

        int current_score = -quiescence_search<inPV>(-beta, -alpha, ss + 1);

        if (stopped)
            return 0;

        if (current_score > best_score)
        {
            best_score = current_score;
            best_move = curr_move;

            // ++capture_moves;

            if (current_score > alpha)
            {
                alpha = current_score;
                if (alpha >= beta)
                {
                    break; // fail soft
                }
            }
        }
    }

    // add to TT only if we aren't in SE
    if (!ss->exclude_tt_move)
    {
        uint8_t bound_flag = BOUND::FAIL_LOW;

        if (alpha >= beta)
        {
            // beta cutoff, fail high
            bound_flag = BOUND::FAIL_HIGH;
        }

        transposition_table.insert(board, best_move, best_score, uncorrected_static_eval, 0, ss->ply, age, bound_flag);
    }

    // TODO: add check moves
    return best_score;
}

template <bool inPV>
int Searcher::negamax(int alpha, int beta, int depth, bool cutnode, SearchStack *ss)
{
    ++nodes;

    if (stopped)
        return 0;

    if (depth == 0 && ss->ply > seldepth)
        seldepth = ss->ply;

    if (!(nodes & 4095))
        if (get_time() >= max_stop_time)
        {
            stopped = true;
            return 0;
        }

    bool in_root = ss->ply <= 0;

    Board &board = ss->board;

    if (inPV)
    {
        ss->pv.clear();
        (ss + 1)->pv.clear();
    }

    // cut the search short if there's a draw
    // if it's a draw at the root node, we'll play a null move
    if (!in_root && board.fifty_move_counter >= 100)
        return 0;

    // if there's a threefold draw
    if (!in_root && twofold(board))
    {
        // std::cout << "threefold repetition" << "\n";
        return 0;
    }

    // bool inPV = beta - alpha > 1;

    // we check if the TT has seen this before
    TT_Entry tt_entry = transposition_table.probe(board);

    bool tt_hit = !ss->exclude_tt_move && tt_entry.hash_equals(board) && tt_entry.flag() != BOUND::NONE;
    Move tt_move = ss->exclude_tt_move ? NO_MOVE : tt_entry.best_move;
    bool has_tt_move = tt_entry.flag() != BOUND::NONE && tt_entry.hash_equals(board) && tt_entry.best_move != NO_MOVE;

    // tt cutoff
    // if the tt_entry matches, we can use the score, and the depth is the same or greater, we can just cut the search short
    if (!inPV && !ss->exclude_tt_move && tt_entry.hash_equals(board) && tt_entry.can_use_score(alpha, beta) && tt_entry.depth >= depth)
    {
        return tt_entry.usable_score(ss->ply);
    }

    if (depth <= 0)
        return quiescence_search<inPV>(alpha, beta, ss);

    // have this dummy variable here so it doesn't get overwritten when we store it in the TT
    const int uncorrected_static_eval = tt_hit ? tt_entry.static_eval : evaluate(board, thread_data.accumulators, ss);
    int eval = ss->static_eval = correct_static_eval(board, uncorrected_static_eval);

    // tt score in certain circumstances can be used as static eval
    // we use logical & here because if it's exact bound we don't care
    if (tt_hit && tt_entry.score != SCORE_NONE && (tt_entry.flag() & (tt_entry.score > eval ? BOUND::FAIL_HIGH : BOUND::FAIL_LOW)))
        eval = tt_entry.score;

    // implements the improving heuristic, an idea that if our static eval is not improving from two plys ago, we can be more aggressive with pruning and reductions
    bool improving = false;

    if (ss->in_check)
    {
        ss->static_eval = SCORE_NONE;
    }
    else if ((ss - 2)->static_eval != SCORE_NONE)
    {
        improving = ss->static_eval > (ss - 2)->static_eval;
    }
    else if ((ss - 4)->static_eval != SCORE_NONE)
    {
        improving = ss->static_eval > (ss - 4)->static_eval;
    }
    else
    {
        improving = true;
    }

    // Razoring: If alpha is way lower than our evaluation, then we step into qsearch, if it fails low, we do a speculative fail low
    // Too low of a margin will mess with the matefinding
    if (!ss->in_check && !ss->exclude_tt_move && depth <= 3 && ss->static_eval < alpha - 800 * depth)
    {
        int razoring_score = quiescence_search<nonPV>(alpha, alpha + 1, ss);

        if (razoring_score <= alpha)
            return razoring_score;
    }

    // apply reverse futility pruning
    if (!inPV && !ss->exclude_tt_move && !ss->in_check && depth <= 6 && eval - 80 * (depth - improving) >= beta)
        return beta + (eval - beta) / 3;

    // bailout
    if (ss->ply >= MAX_PLY - 1)
        return uncorrected_static_eval;

    // applies null move pruning
    if (!(ss - 1)->null_moved && !inPV && !ss->exclude_tt_move && !ss->in_check && !board.only_pawns(board.side_to_move) && eval >= beta && ss->static_eval >= beta - 20 * depth + 200)
    {
        int r = std::min((eval - beta) / 200, 6) + depth / 3 + 4;

        Board copy = board;
        copy.make_null_move();

        // to help detect threefold in nmp
        game_history.push_back(copy.hash);

        // make sure that immediately after we finishd null moving we set the search stack to false, helps with persistent search stack later down the line
        ss->null_moved = true;
        (ss + 1)->board = copy;
        (ss + 1)->updated_accumulator = false;
        // we will never do nmp in check
        (ss + 1)->in_check = false;

        int null_move_score = -negamax<nonPV>(-beta, -beta + 1, depth - r, !cutnode, ss + 1);

        ss->null_moved = false;

        if (stopped)
            return 0;

        game_history.pop_back();

        if (null_move_score >= beta)
            return null_move_score;
    }

    // Interal Iterative Reduction: If we don't have a TT move, that means our move ordering isn't as good, so we reduce the depth.
    // alternatively if it is a cutnode, that means it's very likely that we fail high, so we can reduce the depth for a quicker search
    if (depth >= 4 && !has_tt_move && (inPV || cutnode))
    {
        depth -= 1;
    }

    // Probcut: If we have a really good capture/queen promotion and a reduced search returns a value much higher than beta, we can prune
    int probcut_beta = beta + 350;
    if (!inPV && !ss->in_check && depth > 3 && !is_mate_score(beta) &&
        // If the value from the transposition table is lower than probcut beta, don't probcut because there's a chance that the transposition table
        // cuts off
        !(tt_hit && tt_entry.depth >= depth - 3 && tt_entry.score < probcut_beta))
    {
        // only stores queen promotions
        MoveList captures_and_promotions;
        int score = -INF;

        generate_queen_promotions(board, captures_and_promotions);
        generate_capture_moves(board, captures_and_promotions);

        // scores moves to order them
        MovePicker move_picker(captures_and_promotions);
        move_picker.score(ss, thread_data, tt_move, tt_hit, 107);

        while (move_picker.has_next())
        {
            Board copy = board;
            OrderedMove curr_move = move_picker.next_move();

            copy.make_move(curr_move);

            if (!copy.was_legal())
                continue;

            // qsearch SEE pruning
            // since we only generate capture moves, if the score of the move is negative, that means it did not pass the SEE threshold, so we can just stop the loop
            // since everything after it will also not pass the SEE threshold
            if (curr_move.score < 0)
                break;

            // updates the search stack
            ss->move_played = curr_move;
            (ss + 1)->board = copy;
            (ss + 1)->in_check = copy.is_in_check();
            (ss + 1)->updated_accumulator = false;

            score = -quiescence_search<nonPV>(-probcut_beta, -probcut_beta + 1, ss + 1);

            // re-search with verification to verify that the move is good
            if (score >= probcut_beta)
                score = -negamax<nonPV>(-probcut_beta, -probcut_beta + 1, depth - 4, !cutnode, ss + 1);

            if (stopped)
                return 0;

            if (score >= probcut_beta)
            {
                // update tranposition table
                transposition_table.insert(board, curr_move, score, uncorrected_static_eval, depth - 3, ss->ply, age, BOUND::FAIL_HIGH);

                return score;
            }
        }
    }

    MoveList move_list;
    MoveList quiets;
    MoveList noisies;

    generate_moves(board, move_list);

    // scores moves to order them
    MovePicker move_picker(move_list);
    move_picker.score(ss, thread_data, tt_move, tt_hit, -107);

    const int original_alpha = alpha;

    // get pvs here
    int best_score = -INF;
    Move best_move = NO_MOVE;
    bool is_quiet;

    const int futility_margin = 150 + 100 * depth;

    while (move_picker.has_next())
    {
        Board copy = board;
        Move curr_move = move_picker.next_move();

        if (ss->exclude_tt_move && curr_move == ss->tt_move)
            continue;

        copy.make_move(curr_move);

        if (!copy.was_legal())
            continue;

        move_picker.update_legal_moves();

        is_quiet = curr_move.is_quiet();

        if (!in_root && best_score > TB_LOSS_IN_MAX_PLY)
        {
            // applies late move pruning
            if (is_quiet && move_picker.moves_seen() >= 3 + depth * depth / (2 - improving))
            {
                move_picker.skip_quiets();
                continue;
            }

            // applies pvs see pruning
            const int see_threshold = is_quiet ? -80 * depth : -30 * depth * depth;

            if (depth <= 8 && move_picker.moves_seen() > 0 && !SEE(board, curr_move, see_threshold))
                continue;

            // applies futility pruning
            if (depth <= 8 && !ss->in_check && is_quiet && ss->static_eval + futility_margin < alpha)
            {
                move_picker.skip_quiets();
                continue;
            }
        }

        const uint64_t nodes_before_search = nodes;
        int64_t history_score = 0;

        if (is_quiet)
        {
            quiets.insert(curr_move);
            history_score = get_quiet_history_score(ss, thread_data, curr_move);
        }
        else
        {
            noisies.insert(curr_move);
            history_score = thread_data.capthist[board.moving_piece(curr_move)][curr_move.to_square()][colored_to_uncolored(board.captured_piece(curr_move))];
        }

        int new_depth = depth - 1;
        int extensions = 0;

        // Singular Extensions: If a TT move exists and its score is accurate enough
        // (close enough in depth), we perform a reduced-depth search with the TT
        // move excluded to see if any other moves can beat it.
        if (!in_root && depth >= 8 && curr_move == tt_move && !ss->exclude_tt_move)
        {
            const bool is_accurate_tt_score = tt_entry.depth + 4 >= depth && tt_entry.flag() != BOUND::FAIL_LOW && std::abs(tt_entry.score) < TB_WIN_IN_MAX_PLY;

            if (is_accurate_tt_score)
            {
                const int reduced_depth = (depth - 1) / 2;
                const int singular_beta = tt_entry.score - depth * 2;

                ss->exclude_tt_move = true;
                ss->tt_move = tt_move;

                const int singular_score = negamax<nonPV>(singular_beta - 1, singular_beta, reduced_depth, cutnode, ss);
                // const int singular_score = INF;

                ss->exclude_tt_move = false;

                if (stopped)
                    return 0;

                // No move was able to beat the TT entries score, so we extend the TT
                // move's search
                if (singular_score < singular_beta)
                {
                    // int double_margin = 290 * inPV - 200 * !tt_move.is_capture();

                    int double_margin = 0;
                    int triple_margin = 200;

                    extensions += 1 + (!inPV && singular_score < singular_beta - double_margin);
                    //   (!inPV && tt_move.is_quiet() && singular_score < singular_beta - triple_margin);
                }

                // Multicut: Since the sigular search failed high, that means that the main search is likely to fail high too, so if our singular_beta
                // is larger than beta, we can cutoff
                else if (singular_beta >= beta)
                    return singular_beta;

                // Negative Extensions: if there is a fail high without TT move but singular beta is not high enough for the seacrh to fail high,
                // it's better to search other moves
                else if (tt_entry.score >= beta)
                    extensions -= 2;

                // if we're in a cut node, we expect it to fail high, so we can reduce the depth using a negative extension
                else if (cutnode)
                    extensions -= 2;
            }
        }

        // check extensions after SE, because we alter the search stack
        if (copy.is_in_check())
        {
            (ss + 1)->in_check = true;
            ++extensions;
        }
        else
        {
            (ss + 1)->in_check = false;
        }

        new_depth += extensions;

        // now that we haven't pruned anything, we can update the search stack
        (ss + 1)->board = copy;
        (ss + 1)->updated_accumulator = false;
        (ss)->move_played = curr_move;

        // we can update threefold
        game_history.push_back(copy.hash);

        // prefetch the TT entry
        transposition_table.prefetch(copy);

        int reduction = 0;
        int current_score;

        if (cutnode)
            reduction += 1;

        if (!improving)
            ++reduction;

        reduction -= history_score / 10'000;

        if (inPV)
            reduction -= 1;

        // Late Move Reduction: we've ordered the move in order of importance. We reduce the
        // the depths of later moves because they are less important
        if (move_picker.moves_seen() > 1)
        {
            if (is_quiet)
                reduction += quiet_lmr[depth][move_picker.moves_seen()];
            // noisy move
            else
                reduction += noisy_lmr[depth][move_picker.moves_seen()];

            // makes sure that depth is always positive and if reduction is negative, we only extend once
            int reduced_depth = std::max(std::min(new_depth - reduction, new_depth + 1), 1);

            current_score = -negamax<nonPV>(-alpha - 1, -alpha, reduced_depth, true, ss + 1);

            // if the search fails high we do a full depth research
            if (current_score > alpha && new_depth > reduced_depth)
            {
                // SF idea:
                // Adjust the depth of the full-depth search based on LMR results - if the results were good
                // enough we should search deeper, if it was bad enough search shallower
                const bool do_deeper_search = current_score > (best_score + 35 + 2 * new_depth);
                const bool do_shallower_search = current_score < best_score + new_depth;

                new_depth += do_deeper_search - do_shallower_search;

                if (new_depth > reduced_depth)
                    current_score = -negamax<nonPV>(-alpha - 1, -alpha, new_depth, !cutnode, ss + 1);
            }
        }

        // full depth search
        else if (!inPV || move_picker.moves_seen() > 0)
        {
            current_score = -negamax<nonPV>(-alpha - 1, -alpha, new_depth, !cutnode, ss + 1);
        }

        // if we are in a PV node, we do a full window search on the first move or a fail high
        if (inPV && (move_picker.moves_seen() == 0 || current_score > alpha))
        {
            current_score = -negamax<PV>(-beta, -alpha, new_depth, false, ss + 1);
        }

        move_picker.update_moves_seen();

        // stopped searching that line, so we can get rid of the hash
        game_history.pop_back();

        if (in_root)
        {
            nodes_spent_table[curr_move.from_square()][curr_move.to_square()] += nodes - nodes_before_search;
        }

        if (stopped)
            return 0;

        // fail soft framework
        if (current_score > best_score)
        {
            best_score = current_score;

            if (current_score > alpha)
            {
                alpha = current_score;
                best_move = curr_move;

                // logic to update the pv when we have a new best_move
                if (inPV)
                {
                    ss->pv.clear();
                    ss->pv.insert(best_move);
                    ss->pv.copy_over((ss + 1)->pv);
                }

                // fail high
                if (alpha >= beta)
                {

                    update_histories(ss, noisies, quiets, best_move, depth);

                    break;
                }
            }
        }
    }

    // uncomment this if it doesn't work
    // write the best move down at the current depth

    if (move_picker.moves_seen() == 0)
    {
        if (ss->in_check)
        {
            // prioritize faster mates
            return -TB_WIN + ss->ply;
        }
        else
        {
            return 0;
        }
    }

    // Bonus for prior countermove if there was a fail low
    if (alpha <= original_alpha && !in_root && !(ss - 1)->null_moved && (ss - 1)->move_played.is_quiet())
    {
        Board &previous_board = (ss - 1)->board;
        Move previous_move = (ss - 1)->move_played;

        int bonus = history_bonus(depth);

        update_conthist((ss - 1), previous_move, bonus);

        thread_data.main_history[previous_board.side_to_move][previous_move.from_square()][previous_move.to_square()] << bonus;
        thread_data.pawnhist[previous_board.pawn_hash % PAWNHIST_SIZE][previous_board.moving_piece(previous_move)][previous_move.to_square()] << bonus;
    }

    // add to TT if we aren't in singular search
    if (!ss->exclude_tt_move)
    {
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
        if (best_score != -INF)
            transposition_table.insert(board, best_move, best_score, uncorrected_static_eval, depth, ss->ply, age, bound_flag);
    }

    //  update corrhist if we're not in check
    if (!ss->in_check && (best_move == NO_MOVE || !best_move.is_capture()) && !(best_score >= beta && best_score <= ss->static_eval) && !(best_move == NO_MOVE && best_score >= ss->static_eval))
        update_corrhists(board, best_score, ss->static_eval, depth);

    return best_score;
}

void Searcher::search()
{
    bool minimal = thread_manager.options.minimal;
    // used to store the last line in uci minimal
    std::stringstream info;
    int best_score = -INF;
    Move previous_best_move(a8, a8, 0);
    Move best_move(a8, a8, 0);
    int best_move_stability_factor = 0;
    uint64_t time_elapsed;
    int alpha, beta;
    int search_again_counter = 0;

    Board board = thread_data.search_stack[4].board;

    // this->start_time = get_time();
    nodes = 0;

    // generates a legal move in that position in case that we didn't finish depth one
    MoveList move_list;
    generate_moves(board, move_list);

    for (int i = 0; i < move_list.size(); ++i)
    {
        Board copy = board;
        copy.make_move(move_list[i]);

        if (copy.was_legal())
        {
            best_move = move_list[i];
            break;
        }
    }

    for (int root_depth = 1; root_depth <= max_depth; ++root_depth)
    {
        this->curr_depth = root_depth;
        this->seldepth = 0;

        // STOCKFISH IMPLEMENTATION OF ASPIRATION WINDOWS

        // stockfish uses 9, let's try that later
        int delta = 9 + average_score * average_score / 10182;
        // int delta = 25;

        alpha = std::max<int>(best_score - delta, -TB_WIN - 1);
        beta = std::min<int>(best_score + delta, TB_WIN + 1);

        // start with a small aspiration window and, in case of a fail high/low, re-search with a bigger window until we don't fail high/low anymore
        int failed_high_count = 0;

        while (true)
        {

            // missing the search again counter but it's always 0?
            int adjusted_depth = std::max(1, root_depth - failed_high_count);
            int root_delta = beta - alpha;
            // we start at 4 beacuse of conthist
            best_score = negamax<PV>(alpha, beta, adjusted_depth, false, &thread_data.search_stack[4]);

            if (stopped)
                break;

            if (best_score <= alpha)
            {
                beta = (alpha + beta) / 2;
                alpha = std::max<int>(best_score - delta, -TB_WIN - 1);
            }
            else if (best_score >= beta)
            {
                beta = std::min<int>(best_score + delta, TB_WIN + 1);
                ++failed_high_count;
            }
            else
                break;

            delta += delta / 3;
        }

        // std::cout << static_cast<int>(aspiration_adjustments) << " " << alpha << " " << beta << "\n";

        if (stopped)
            break;

        // everything after here is only done if we're successfully cleared a depth

        // everything we do down here is about time management and info printing, if we aren't in the main thread, we don't do anything
        if (!is_main_thread)
            continue;

        best_move = thread_data.search_stack[4].pv[0];

        time_elapsed = std::max(get_time() - start_time, (uint64_t)1);

        uint64_t all_thread_node_count = thread_manager.get_nodes();

        // get WDL statistics
        int win, draw, loss;
        if (is_mate_score(best_score))
        {
            win = mate_score_to_moves(best_score) >= 0 ? 1000 : 0;
            draw = 0;
            loss = 1000 - win;
        }
        else
        {
            win = win_rate_model(board, best_score);
            // in a zero sum game, we assume that the opponents evaluation is the negation of oursS
            loss = win_rate_model(board, -best_score);
            draw = 1000 - win - loss;
        }

        if (nodes > max_nodes)
            break;

        if (best_move == previous_best_move)
        {
            best_move_stability_factor = std::min(best_move_stability_factor + 1, 4);
        }
        else
        {
            best_move_stability_factor = 0;
            previous_best_move = best_move;
        }

        // std::cout << "previous best move: " << previous_best_move.to_string() << "\n";

        if (root_depth > 7 && time_set)
        {
            scale_time(best_move_stability_factor);
        }

        average_score = (average_score != -INF) ? (2 * best_score + average_score) / 3 : best_score;

        // resets the stream in preperation for printing the next line
        info.str("");
        info.clear();

        if (is_mate_score(best_score))
            info << "info depth " << static_cast<int>(root_depth) << " seldepth " << seldepth << " score mate " << mate_score_to_moves(best_score) << " wdl " << win << " " << draw << " " << loss << " nodes " << all_thread_node_count << " time " << time_elapsed << " nps " << static_cast<uint64_t>(static_cast<double>(all_thread_node_count) / time_elapsed * 1000) << " hashfull " << transposition_table.hash_full() << " pv " << thread_data.search_stack[4].pv.to_string() << " "
                 << std::endl;
        else
            info << "info depth " << static_cast<int>(root_depth) << " seldepth " << seldepth << " score cp " << normalize_eval(board, best_score) << " wdl " << win << " " << draw << " " << loss << " nodes " << all_thread_node_count << " time " << time_elapsed << " nps " << static_cast<uint64_t>(static_cast<double>(all_thread_node_count) / time_elapsed * 1000) << " hashfull " << transposition_table.hash_full() << " pv " << thread_data.search_stack[4].pv.to_string() << " "
                 << std::endl;

        if (!minimal)
            std::cout << info.str();

        if (get_time() > optimum_stop_time)
            break;
    }

    if (is_main_thread)
    {
        if (minimal)
            std::cout << info.str();

        if (nodes_set)
            std::cout << "bestmove " << previous_best_move.to_string() << " " << std::endl;
        else
            std::cout
                << "bestmove " << best_move.to_string() << " " << std::endl;

        thread_manager.stop();
    }
}
