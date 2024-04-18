#pragma once

#include "defs.h"
#include "movegen.h"

uint64_t perft(Board &board, uint8_t depth);

uint64_t perft_debug(Board &board, uint8_t depth, uint8_t start_depth);