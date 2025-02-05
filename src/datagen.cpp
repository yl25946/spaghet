#include "datagen.h"

void relabel_eval(const std::string &input_file, const std::string &output_file)
{
    BulletFormat position{};

    std::ifstream stream(input_file, std::ios::binary);
    std::ofstream ostream(output_file, std::ios::binary);

    if (!stream.is_open())
    {
        std::cout << "Failed to open input file." << std::endl;
        return;
    }

    if (!ostream.is_open())
    {
        std::cout << "Failed to open output file." << std::endl;
        return;
    }

    uint64_t start_time = get_time();
    uint64_t processed_positions = 0;

    uint64_t sample_time = get_time();
    uint64_t sample_positions = 0;

    while (stream.read(reinterpret_cast<char *>(&position), sizeof(BulletFormat)))
    {
        ++processed_positions;
        ++sample_positions;

        Accumulator acc(position);

        if (processed_positions < 100)
            std::cout << static_cast<int>(position.score) << " ";

        position.score = std::clamp(NNUE::eval(acc, calculate_bucket(position.occ), WHITE), TB_LOSS_IN_MAX_PLY + 1, TB_WIN_IN_MAX_PLY - 1);

        if (processed_positions < 100)
            std::cout << static_cast<int>(position.score) << std::endl;

        ostream.write(reinterpret_cast<const char *>(&position), sizeof(BulletFormat));

        if (processed_positions % (1ULL << 20) == 0)
        {
            uint64_t curr_time = get_time();

            std::cout << "info nodes " << processed_positions << " time " << curr_time - start_time << " nps " << 1000 * sample_positions / (curr_time - sample_time) << std::endl;

            sample_time = curr_time;
            sample_positions = 0;
        }
    }

    uint64_t curr_time = get_time();

    std::cout << "info nodes " << processed_positions << " time " << curr_time - start_time << " nps " << processed_positions / (curr_time - start_time) << std::endl;
}