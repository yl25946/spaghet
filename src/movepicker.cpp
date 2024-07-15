#include "movepicker.h"

int64_t get_quiet_history_score(SearchStack *ss, ThreadData &thread_data, Move quiet_move)
{
    int64_t quiet_move_score = thread_data.main_history.move_value(quiet_move, ss->board.side_to_move);

    int ply = ss->ply;

    // adds followup move history bonus
    if (ply >= 2 && !(ss - 2)->null_moved)
        quiet_move_score += thread_data.conthist.move_value(ss->board, quiet_move, (ss - 2)->board, (ss - 2)->move_played);

    // adds counter move history bonus
    if (ply >= 1 && !(ss - 1)->null_moved)
        quiet_move_score += thread_data.conthist.move_value(ss->board, quiet_move, (ss - 1)->board, (ss - 1)->move_played);

    return quiet_move_score;
}

MovePicker::MovePicker(MoveList &move_list) : move_list(move_list)
{
    // this->move_list = move_list;

    for (int i = 0; i < move_list.size(); ++i)
    {
        if (move_list.moves[i].is_quiet())
            ++quiet_moves;
    }

    moves_remaining = move_list.size();
}

void MovePicker::score(SearchStack *ss, ThreadData &thread_data, Move tt_move, bool has_tt_move, int threshold)
{

    // just compare the info because they are different objects
    for (int i = 0; i < move_list.size(); ++i)
    {
        Move current_move = move_list.moves[i];
        uint8_t move_flag = current_move.move_flag();

        // make sure we don't get any weird values floating around in value of the orderedmove
        move_list.moves[i].score = 0;

        if (has_tt_move && tt_move == current_move)
        {
            // this ensures that the move comes first
            move_list.moves[i].score = MAX_MOVE_ORDERING_SCORE;
            continue;
        }
        // if it is a promotion, forcibly makes queen promotions captures, queen promotions, knight promotion captures, knight promotions
        // if (move_flag & PROMOTION)
        // {
        //     switch (move_flag)
        //     {
        //     case MOVE_FLAG::QUEEN_PROMOTION_CAPTURE:
        //         moves[i].value = INF - 1;
        //         continue;
        //     case MOVE_FLAG::QUEEN_PROMOTION:
        //         moves[i].value = INF - 2;
        //         continue;
        //     case MOVE_FLAG::KNIGHT_PROMOTION_CAPTURE:
        //         moves[i].value = INF - 3;
        //         continue;
        //     case MOVE_FLAG::KNIGHT_PROMOTION:
        //         moves[i].value = INF - 4;
        //         continue;
        //     default:
        //         break;
        //     }
        // }
        if (!move_list.moves[i].is_quiet())
        {

            move_list.moves[i].score += thread_data.capthist.move_value(ss->board, move_list.moves[i]);
            // if (capthist.move_value(board, move_list.moves[i]) > 0)
            //  std::cout << capthist.move_value(board, move_list.moves[i]) << " ";

            // we just deal with this specific case and die
            if (move_flag == MOVE_FLAG::EN_PASSANT_CAPTURE)
            {
                // just hardcoded
                move_list.moves[i].score += mvv_values[PIECES::WHITE_PAWN] + (SEE(ss->board, move_list.moves[i], threshold) ? CAPTURE_BONUS : -CAPTURE_BONUS);
                continue;
            }

            uint8_t source_square = current_move.from_square();
            uint8_t target_square = current_move.to_square();

            // use mvvto find the move value
            int captured_piece_value = mvv_values[ss->board.mailbox[source_square]];

            // apply a promotion bonus if necessary
            int promotion_piece_value = 0;
            if (move_list.moves[i].is_promotion())
            {
                uint8_t promotion_piece = move_list.moves[i].promotion_piece();

                // if the piece is a queen or a knight, we apply it's promotion value
                if (promotion_piece == BITBOARD_PIECES::QUEEN)
                    promotion_piece_value = mvv_values[PIECES::WHITE_QUEEN] + PROMOTION_BONUS;
                else if (promotion_piece == BITBOARD_PIECES::KNIGHT)
                    promotion_piece_value = mvv_values[PIECES::WHITE_KNIGHT] + PROMOTION_BONUS;
            }

            move_list.moves[i].score += captured_piece_value + promotion_piece_value + (SEE(ss->board, move_list.moves[i], threshold) ? CAPTURE_BONUS : -CAPTURE_BONUS);

            // we give a promotion bonus if the promotion is "meaningful"
            // if (promotion_piece_value != 0)
            //     move_list.moves[i].score += PROMOTION_BONUS;

            continue;
        }
        // not a capture, use history table
        else
        {
            move_list.moves[i].score = get_quiet_history_score(ss, thread_data, move_list.moves[i]);

            // check killer moves
            for (int j = 0; j < ss->killers.size(); ++j)
            {
                if (move_list.moves[i] == ss->killers[j])
                {
                    move_list.moves[i].score = MAX_KILLERS - j;
                }
            }

            // move_list.moves[i].score += (ss - 4)->conthist->move_value(board, move_list.moves[i]) / 2;
        }
    }
}

OrderedMove MovePicker::next_move()
{
    // tracks the index and the value of the greatest index value in this entire list
    int max_entry_index = -1;
    int64_t max_entry_value = INT64_MIN;

    int search_index = left_swap_index;

    for (; search_index < move_list.size(); ++search_index)
    {
        if (skip_quiet_moves && move_list.moves[search_index].is_quiet())
        {
            continue;
        }

        if (move_list.moves[search_index].score > max_entry_value)
        {
            max_entry_index = search_index;
            max_entry_value = move_list.moves[search_index].score;
        }
    }

    // swaps the two values so the greates is at the start
    std::swap(move_list.moves[left_swap_index], move_list.moves[max_entry_index]);

    // updates all the information in the move picker
    --moves_remaining;

    if (move_list.moves[left_swap_index].is_quiet())
        --quiet_moves;

    // increments it
    return move_list.moves[left_swap_index++];
}

bool MovePicker::has_next()
{
    return moves_remaining != 0;
}