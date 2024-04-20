#pragma once

#include "defs.h"
#include "board.h"

/**
 * Returns the evaluation of the board in centripawnsb from the side_to_move's perspective
 */
int evaluate(Board &board);