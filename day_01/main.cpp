#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <numeric>

#include "../common/utils.hpp"


std::vector<int> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual output
    std::vector<int> output;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // Read input data
    std::string line;
    while (std::getline(input_file, line)) {
        output.push_back(std::stoi(line));
    }

    return output;
}



int main(int argc, char* argv[]) {
    std::vector<int> values = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;
    float part_1_elapsed_time = time_block( [&](){
        for (int i = 0; i < values.size() - 1; i++) {
            result_1 += values[i] < values[i+1];
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   There are %d measurements that are larger than the previous measurement.\n", result_1);
    


    // Part Two algorithms
    int WINDOW_SIZE = 3;

    int result_2 = 0;
    float part_2_elapsed_time = time_block( [&](){
        for (int i = 0; i < values.size() - WINDOW_SIZE; i++) {
            result_2 += std::accumulate(&values[i], &values[i+WINDOW_SIZE], 0) < std::accumulate(&values[i+1], &values[i+1+WINDOW_SIZE], 0);
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   There are %d block measurements that are larger than the previous block of measurement.\n", result_2);
    
    return 0;
}
