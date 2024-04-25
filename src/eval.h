#pragma once

#include "defs.h"
#include "board.h"

/**
 * Creates the psqt for pesto
 */
void init_pesto_tables();

/**
 * Returns the evaluation of the board in centripawnsb from the side_to_move's perspective
 */
int evaluate(Board &board);