#include "nnue.h"
#include "datagen.h"

void relabel_eval(const std::string &input_file, const std::string &output_file)
{
    BulletFormat position{};

    std::ifstream stream(input_file, std::ios::binary);
    std::ofstream ostream(output_file, std::ios::binary);

    uint64_t start_time = get_time();
    uint64_t processed_positions = 0;

    uint64_t sample_time = get_time();
    uint64_t sample_positions = 0;

    while (stream.read(reinterpret_cast<char *>(&position), sizeof(BulletFormat)))
    {
        ++processed_positions;
        ++sample_positions;

        Accumulator acc(position);

        position.score = NNUE::eval(acc, calculate_bucket(position.occ), WHITE);

        ostream.write(reinterpret_cast<const char *>(&position), sizeof(BulletFormat));

        if (processed_positions % 1048576)
        {
            uint64_t curr_time = get_time();

            std::cout << "info nodes " << processed_positions << " time " << curr_time - start_time << " nps " << sample_positions / (curr_time - sample_time) << std::endl;

            sample_time = sample_positions = 0;
        }
    }
}