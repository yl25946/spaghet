#include "datagen.h"

void relabel_eval(const std::string &input_file, const std::string &output_file, uint64_t buffer)
{
    // how many entries we can have
    uint64_t size = buffer * 32'768;

    std::vector<uint64_t> occ(size);
    // msb to lsb
    std::vector<uint8_t[16]> pcs(size);
    std::vector<uint8_t> score(size);
    std::vector<uint8_t> result(size);
    std::vector<uint8_t> king_square(size);
    std::vector<uint8_t> opp_king_square(size);
    std::vector<uint8_t[3]> extra(size);
}