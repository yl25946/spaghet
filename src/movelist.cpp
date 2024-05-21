#include "movelist.h"

void MoveList::insert(Move move)
{
    moves[count++] = OrderedMove(move);
}

void MoveList::insert(uint8_t from, uint8_t to, uint8_t move_flag)
{
    moves[count++] = OrderedMove(from, to, move_flag);
}

void MoveList::insert(uint8_t from, uint8_t to, uint8_t move_flag, uint16_t value)
{
    moves[count++] = OrderedMove(from, to, move_flag, value);
}

uint8_t MoveList::size() const
{
    return count;
}

void MoveList::print()
{
    for (int i = 0; i < count; ++i)
    {
        moves[i].print();
        std::cout << "\n";
    }
};

void MoveList::score(const Board &board, TranspositionTable &transposition_table, QuietHistory &history, Killer &killers, int ply)
{
    TT_Entry &tt_entry = transposition_table.probe(board);
    Move tt_move;
    bool has_tt_move = false;

    if (tt_entry.hash == board.hash && tt_entry.flag() != BOUND::NONE)
    {
        tt_move = tt_entry.best_move;
        has_tt_move = true;
    }

    // just compare the info because they are different objects
    for (int i = 0; i < size(); ++i)
    {
        Move current_move = moves[i];
        uint8_t move_flag = current_move.move_flag();

        // make sure we don't get any weird values floating around in value of the orderedmove
        moves[i].value = 0;

        if (has_tt_move && tt_move.info == current_move.info)
        {
            // this ensures that the move comes first
            moves[i].value = MAX_MOVE_ORDERING_SCORE;
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
        if (move_flag & CAPTURES)
        {
            // we just deal with this specific case and die
            if (move_flag == MOVE_FLAG::EN_PASSANT_CAPTURE)
            {
                // just hardcoded
                moves[i].value = 1400 + CAPTURE_BONUS;
                continue;
            }

            uint8_t source_square = current_move.from_square();
            uint8_t target_square = current_move.to_square();

            // use mvv-lva to find the move value

            uint8_t attacking_piece = board.mailbox[source_square];
            uint8_t captured_piece = board.mailbox[target_square];

            moves[i].value = 15 * piece_value[captured_piece] - piece_value[attacking_piece] + CAPTURE_BONUS;

            continue;
        }
        // TODO: add SEE

        // not a capture, use history table
        else
        {
            // std::cout << history.move_value(moves[i]) << "\n";
            moves[i].value = history.move_value(moves[i], board.side_to_move);

            // check killer moves
            const int killers_size = killers.size(ply);
            for (int j = 0; j < killers_size; ++j)
            {
                if (moves[i].info == killers.killers[ply][j].info)
                {
                    moves[i].value = MAX_KILLERS - j;
                }
            }
        }
    }
}

Move MoveList::next_move()
{
    // tracks the index and the value of the greatest index value in this entire list
    int max_entry_index = -1;
    int64_t max_entry_value = INT64_MIN;

    int search_index = left_swap_index;

    for (; search_index < size(); ++search_index)
    {
        if (moves[search_index].value > max_entry_value)
        {
            max_entry_index = search_index;
            max_entry_value = moves[search_index].value;
        }
    }

    // swaps the two values so the greates is at the start
    std::swap(moves[left_swap_index], moves[max_entry_index]);

    // increments it
    return moves[left_swap_index++];
}