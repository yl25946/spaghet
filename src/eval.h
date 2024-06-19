#pragma once

#include "defs.h"
#include "board.h"
#include "nnue.h"

// for debugging purposes
extern int *eg_pesto_table[6];
extern int *mg_pesto_table[6];

extern NNUE nnue;

/**
 * Creates the psqt for pesto
 */
void init_pesto_tables();

/**
 * Returns the evaluation of the board in centripawnsb from the side_to_move's perspective
 */
int evaluate(const Board &board, const Accumulator &accumulator);