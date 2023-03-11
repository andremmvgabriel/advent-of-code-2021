#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "../common/utils.hpp"



std::vector<int> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<int> temp;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    std::getline(input_file, line);

    return temp;
}



int main(int argc, char* argv[]) {
    // Common
    const auto data = parse_inputs(argc, argv);

    // Part One algorithms
    float part_1_elapsed_time = time_block( [&](){
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    // printf("   To guarantee victory agaisnt the giant squid, the best board results in a final score of %d.\n", result_1);   



    // Part Two algorithms
    float part_2_elapsed_time = time_block( [&](){
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    // printf("   The last board to win have a final score of %d.\n", result_2);  

    return 0;
}
