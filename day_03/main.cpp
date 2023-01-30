#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <numeric>
#include <array>
#include <map>

#include "../common/utils.hpp"

std::vector<std::string> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual output
    std::vector<std::string> output;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // Read input data
    std::string line;
    while (std::getline(input_file, line)) {
        output.push_back(line);
    }

    return output;
}



int main(int argc, char* argv[]) {
    // Common
    std::vector<std::string> numbers = parse_inputs(argc, argv);

    // Part One algorithms
    uint64_t gamma = 0;
    uint64_t epsilon = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Mask has the purpose of removing unwanted flipped bits
        // > Calculating epsilon will flip all 64 bits, but not all bits may be relevant.
        // > If each number has a length of x bits, where x < 64, all the remaining bits from x to 64 are not valid. This will replace them with 0's.
        uint64_t mask = 0;

        for (int i = 0; i < numbers[0].size(); i++) {
            // Finds the amount of 0's and 1's
            uint64_t zeros = 0, ones = 0;
            for (auto number : numbers) {
                number[i] == '0' ? zeros++ : ones++;
            }

            // Updates the gamma
            gamma = (gamma << 1) | (ones >= zeros);

            // Updates the mask relevant bits
            mask = (mask << 1) | 1;
        }

        // Calculates the epsilon
        epsilon = (~gamma & mask);
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   1. The report shows that gamma and epsilon values are %lu and %lu, respectively.\n", gamma, epsilon);
    printf("   2. This concludes that the power consumption of the submarine is %lu.\n", epsilon * gamma);
    


    // Part Two algorithms
    uint64_t oxygen_generator = 0;
    uint64_t co2_scrubber = 0;

    float part_2_elapsed_time = time_block( [&](){
        // Oxygen generator rating search
        std::vector<std::string> oxygen_numbers_list = numbers;
        for (int i = 0; i < numbers[0].size(); i++) {
            // Finds the amount of 0's and 1's
            uint64_t zeros = 0, ones = 0;
            std::vector<std::string> zero_numbers = {}, one_numbers = {};
            for (auto number : oxygen_numbers_list) {
                if (number[i] == '0') {
                    zeros++;
                    zero_numbers.push_back(number);
                } else {
                    ones++;
                    one_numbers.push_back(number);
                }
            }

            // Updates the oxygen numbers list for the next search
            oxygen_numbers_list = (ones >= zeros) ? one_numbers : zero_numbers;

            // Exit check
            if (oxygen_numbers_list.size() <= 1) {
                for (auto number : oxygen_numbers_list[0]) {
                    oxygen_generator = (oxygen_generator << 1) | (number == '0' ? 0 : 1);
                }
                break;
            }
        }

        // CO2 Scrubber rating search
        std::vector<std::string> co2_numbers_list = numbers;
        for (int i = 0; i < numbers[0].size(); i++) {
            // Finds the amount of 0's and 1's
            uint64_t zeros = 0, ones = 0;
            std::vector<std::string> zero_numbers = {}, one_numbers = {};
            for (auto number : co2_numbers_list) {
                if (number[i] == '0') {
                    zeros++;
                    zero_numbers.push_back(number);
                } else {
                    ones++;
                    one_numbers.push_back(number);
                }
            }

            // Updates the co2 numbers list for the next search
            co2_numbers_list = (ones < zeros) ? one_numbers : zero_numbers;

            // Exit check
            if (co2_numbers_list.size() <= 1) {
                for (auto number : co2_numbers_list[0]) {
                    co2_scrubber = (co2_scrubber << 1) | (number == '0' ? 0 : 1);
                }
                break;
            }
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   1. The report shows that oxygen generator and co2 scrubber ratings are %lu and %lu, respectively.\n", oxygen_generator, co2_scrubber);
    printf("   2. This concludes that the life support rating of the submarine is %lu.\n", oxygen_generator * co2_scrubber);

    return 0;
}
