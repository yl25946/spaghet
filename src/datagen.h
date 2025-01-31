#pragma once

#include "defs.h"

#include "move.h"
#include "board.h"
#include "nnue.h"

/**
 * Takes in a bulletformat file and then relabels it with the evaluation of the NNUE
 */
void relabel_eval(const std::string &input_file, const std::string &output_file, uint64_t buffer);