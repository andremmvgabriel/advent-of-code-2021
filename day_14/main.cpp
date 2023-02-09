#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <unordered_map>

#include "../common/utils.hpp"



class Polymer
{
private:
    const std::string _template;
    std::unordered_map<std::string, char> _rules;
    std::unordered_map<std::string, uint64_t> _pairs;

public:
    Polymer() = delete;
    Polymer(const std::string& temp, const std::unordered_map<std::string, char> &rules) : _template(temp), _rules(rules) {
        for (int i = 0 ; i < temp.size() - 1; i++) {
            std::string pair = std::string(temp.begin() + i, temp.begin() + i + 2);
            ++_pairs[pair];
        }
    }

    void grow() {
        // New pairs will be stored here
        std::unordered_map<std::string, uint64_t> new_pairs;

        for (const auto & [pair, amount] : _pairs) {
            // Creates the new two pairs
            std::string new_pair_1 = pair; new_pair_1[1] = _rules[pair];
            std::string new_pair_2 = pair; new_pair_2[0] = _rules[pair];

            // Increases the pairs in the list, and its counters
            new_pairs[new_pair_1] += amount;
            new_pairs[new_pair_2] += amount;
        }

        // Updates the pairs of the polymer, after grow
        _pairs = new_pairs;
    }

    std::unordered_map<char, uint64_t> get_elements_counters() {
        // All characters occurences will be stored here
        std::unordered_map<char, uint64_t> counters;

        for (const auto & [pair, amount] : _pairs) {
            // Only counts the occurence of the first element of each pair
            counters[pair[0]] += amount;
        }

        // Last element of the initial polymer will always be the same
        // > Previous counting only counts first elements
        // > This needs to be done to not lose last element existence
        ++counters[_template.back()];

        return counters;
    }
};

std::tuple<std::string, std::unordered_map<std::string, char>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::string polymer;
    std::unordered_map<std::string, char> rules;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // First line is always the polymer
    std::getline(input_file, polymer);

    // Remove the empty line
    std::string line;
    std::getline(input_file, line);

    // Read all the rules
    while (std::getline(input_file, line)) {
        std::vector<std::string> parts = string_split(line, " -> ");
        rules[parts[0]] = parts[1][0];
    }

    return {polymer, rules};
}



int main(int argc, char* argv[]) {
    // Common
    const auto [POLYMER, RULES] = parse_inputs(argc, argv);

    // Part One algorithms
    const int NUMBER_STEPS_1 = 10;
    uint64_t result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Creates the polymer
        Polymer polymer(POLYMER, RULES);

        // Grows the polymer as many steps as needed
        for (int step = 0; step < NUMBER_STEPS_1; step++) { polymer.grow(); }

        // Variables to store max and min values
        uint64_t max_value = 0, min_value = -1; // Unsigned -> This is max value of uint 64 bits.

        // Updates the max and min values accordingly
        for (auto [c, counter] : polymer.get_elements_counters()) {
            if (counter > max_value) max_value = counter;
            if (counter < min_value) min_value = counter;
        }

        // Makes the calculation
        result_1 = max_value - min_value;
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   After %d steps, the difference between the most common and less common elements is %lu.\n", NUMBER_STEPS_1, result_1);   



    // Part Two algorithms
    const int NUMBER_STEPS_2 = 40;
    uint64_t result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        // Creates the polymer
        Polymer polymer(POLYMER, RULES);

        // Grows the polymer as many steps as needed
        for (int step = 0; step < NUMBER_STEPS_2; step++) { polymer.grow(); }

        // Variables to store max and min values
        uint64_t max_value = 0, min_value = -1; // Unsigned -> This is max value of uint 64 bits.

        // Updates the max and min values accordingly
        for (auto [c, counter] : polymer.get_elements_counters()) {
            if (counter > max_value) max_value = counter;
            if (counter < min_value) min_value = counter;
        }

        // Makes the calculation
        result_2 = max_value - min_value;
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   After %d steps, the difference between the most common and less common elements is %lu.\n", NUMBER_STEPS_2, result_2);   

    return 0;
}
