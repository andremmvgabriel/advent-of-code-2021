#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <cstdint>

#include "../common/utils.hpp"



std::vector<std::string> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<std::string> lines;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while(std::getline(input_file, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::unordered_map<char, char> CHUNK_PAIRS = { {'(', ')'}, {'[', ']'}, {'{', '}'}, {'<', '>'} };

bool is_open_char(const char & c) {
    for (const auto & [open, close] : CHUNK_PAIRS) {
        if (open == c) return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    // Common
    const auto navigation_subsystem = parse_inputs(argc, argv);

    // Part One algorithms
    std::unordered_map<char, int> corruption_points = { {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137} };
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        for (const auto & line : navigation_subsystem) {
            // Vector that will store the sequence of expected characters to close the chunk
            // > First element has the lowest priority
            // > Last element has the highest priority
            std::vector<char> expected;

            // Goes through every character in the line
            for (const char & c : line) {
                // Adds a new close character that is expected to close the new opened chunk
                if (is_open_char(c)) { expected.push_back(CHUNK_PAIRS[c]); continue; }

                // Chunk closed! Remove expected character from the list
                if (c == expected.back()) { expected.pop_back(); continue; }

                // If code reach here, we found a corrupted character
                // > Adds its corruption value into the final score
                result_1 += corruption_points[c];
                break;
            }
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   The total syntax error score for all the illegal characters in each corrupted line of the navigation system is %d.\n", result_1);



    // Part Two algorithms
    std::unordered_map<char, int> incomplete_score = { {')', 1}, {']', 2}, {'}', 3}, {'>', 4} };
    int result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        std::vector<uint64_t> scores;

        for (const auto & line : navigation_subsystem) {
            // Control variable to know if the line is corrupted
            bool is_corrupted = false;

            // Vector that will store the sequence of expected characters to close the chunk
            // > First element has the lowest priority
            // > Last element has the highest priority
            std::vector<char> expected;

            // Goes through every character in the line
            for (const char & c : line) {
                // Adds a new close character that is expected to close the new opened chunk
                if (is_open_char(c)) { expected.push_back(CHUNK_PAIRS[c]); continue; }

                // Chunk closed! Remove expected character from the list
                if (c == expected.back()) { expected.pop_back(); continue; }

                // Updates the corruption flag
                is_corrupted = true;
                break;
            }

            // Do not proceed if the current line is corrupted
            if (is_corrupted) { continue; }

            // Do not proceed if the current line is complete
            if (!expected.size()) { continue; }

            // Lets calculate the line incompletion score
            uint64_t score = 0;
            while (!expected.empty()) {
                score = score * 5 + incomplete_score[expected.back()];
                expected.pop_back();
            }
            
            // Adds the score into the list of scores
            scores.push_back(score);
        }

        // Ascending sort of the scores
        std::sort(scores.begin(), scores.end(), [](const uint64_t &a, const uint64_t& b){ return a <= b; });

        // Gets the middle score (this assumes that the scores size is always odd)
        result_2 = scores[ (scores.size()-1) / 2 ];
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The middle score from the list of all incomplete lines is %d.\n", result_2);  

    return 0;
}
