#pragma once

#include "defs.h"
#include "eval.h"
#include "utils.h"
#include "nnue.h"

// normalized based on how much material is on the board
// normalizes it so that 0.5 is equal to a 50% chance of winning
// assume we don't have mate scores
int normalize_eval(const Board &board, int eval);
// gives percentage of winning in the thousandths truncated to an integer
int win_rate_model(const Board &board, int eval);
void print_eval(Board &board);