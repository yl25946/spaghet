#include "defs.h"

// const char *square_to_coordinate[] = {

// };

std::map<char, uint8_t> char_pieces = {
    {'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K}, {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k}};

std::string empty_board = "8/8/8/8/8/8/8/8 b - - ";
std::string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
std::string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
std::string killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
std::string cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";
std::string repetitions = "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 ";