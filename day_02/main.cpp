#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <numeric>
#include <array>
#include <map>

#include "../common/utils.hpp"

std::vector<std::pair<std::string, int>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual output
    std::vector<std::pair<std::string, int>> output;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // Read input data
    std::string line;
    while (std::getline(input_file, line)) {
        size_t index = line.find(" ");
        if (index == std::string::npos) throw std::runtime_error("There is something wrong with the input file data.");
        // Populates the map
        // > key = std::string(&line[0], &line[index])
        // > value = std::stoi(&line[index+1]) -> This picks the line from index+1 until the end
        output.push_back({std::string(&line[0], &line[index]), std::stoi(&line[index+1])});
    }

    return output;
}



int main(int argc, char* argv[]) {
    // Common
    std::vector<std::pair<std::string, int>> moves = parse_inputs(argc, argv);

    // Part One algorithms
    std::map<std::string, std::array<int,2>> MOVE_INCREMENT_1 = {
        { "forward", {1, 0} },
        { "up", {0, -1} },
        { "down", {0, 1} }
    };

    std::array<int, 2> position_1 = {0, 0}; // (horizontal, depth)
    float part_1_elapsed_time = time_block( [&](){
        // Check each move in the inputs
        for (auto move : moves) {
            // Adds each position element to the increment
            for (int i = 0; i < 2; i++) {
                position_1[i] += MOVE_INCREMENT_1[move.first][i] * move.second;
            }
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   My final position is (%d, %d). Result = %d\n", position_1[0], position_1[1], position_1[0]*position_1[1]);
    


    // Part Two algorithms
    std::map<std::string, std::array<int,3>> MOVE_INCREMENT_2 = {
        { "forward", {1, 0, 0} },
        { "up", {0, 0, -1} },
        { "down", {0, 0, 1} }
    };

    std::array<int, 3> position_2 = {0, 0, 0}; // (horizontal, depth, aim)
    float part_2_elapsed_time = time_block( [&](){
        // Check each move in the inputs
        for (auto move : moves) {
            // Adds each position element to the increment
            for (int i = 0; i < 3; i++) {
                position_2[i] += MOVE_INCREMENT_2[move.first][i] * move.second;
            }

            // Additionally updates the depth in case of forward command
            if (move.first == "forward") {
                position_2[1] += position_2[2] * move.second;
            }
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   My final position is (%d, %d) with a total aim of %d. Result = %d\n", position_2[0], position_2[1], position_2[2], position_2[0]*position_2[1]);
    
    return 0;
}
