#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <set>
#include <limits>

#include "../common/utils.hpp"



std::vector<int> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual output
    std::vector<int> values;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // There is only a single line with all the values
    std::string line;
    std::getline(input_file, line);

    // Separates all the values
    std::vector<std::string> str_values = string_split(line, ",");

    // Converts all the values from string to integer
    std::transform(str_values.begin(), str_values.end(), std::back_inserter(values), [](const std::string &val){ return std::stoi(val); });

    return values;
}

std::tuple<int, int> find_best_consumption(std::map<int, int>& distribution, std::unordered_map<int, int>& consumption_model) {
    // Outputs
    int best_target = 0;
    int best_consumption = std::numeric_limits<int>::max();

    // Gets the minimum and maximum positions in the list of numbers
    auto [min_position, _1] = *std::min_element(distribution.begin(), distribution.end());
    auto [max_position, _2] = *std::max_element(distribution.begin(), distribution.end());

    // Goes through all the possible positions
    for (int i = min_position ; i <= max_position; i++) {
        int total = 0;

        // Sums all the fuel amounts
        for (const auto & [value, amount] : distribution) {
            int distance = std::abs(i - value);
            total += consumption_model[distance] * amount;
        }

        // Updates the best fuel and target
        // > If not a better result was found we can break the search (we found the best!)
        // > This is possible because the distribution is ordered
        if (best_consumption > total) {
            best_consumption = total;
            best_target = i;
        } else break;

    }

    return {best_consumption, best_target};
}



int main(int argc, char* argv[]) {
    // Common
    const auto values = parse_inputs(argc, argv);

    // Creates a distribution model of the values
    // > We want this distribution to be ordered -> map
    std::map<int, int> distribution;
    for (const auto & value : values) { ++distribution[value]; }

    // Gets the minimum and maximum elements in the list of numbers
    int min_number = *std::min_element(values.begin(), values.end());
    int max_number = *std::max_element(values.begin(), values.end());

    // Part One algorithms
    int result_fuel_1;
    int result_target_1;

    float part_1_elapsed_time = time_block( [&](){
        // Creates the total consumption from a point to all other points
        std::unordered_map<int, int> fuel_consumption;
        for (int i = min_number; i <= max_number - min_number; i++) { fuel_consumption[i] = i; }

        // Actual simulation
        auto [best_consumption, best_target] = find_best_consumption(distribution, fuel_consumption);

        // Updates the results
        result_fuel_1 = best_consumption;
        result_target_1 = best_target;
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   The best horizontal position that the crabs can align is %d.\n", result_target_1);
    printf("   The total consumption is %d of fuel.\n", result_fuel_1);



    // Part Two algorithms
    int result_fuel_2;
    int result_target_2;

    float part_2_elapsed_time = time_block( [&](){
        // Creates the total consumption from a point to all other points
        std::unordered_map<int, int> fuel_consumption {{0, 0}};

        for (int i = 1; i <= max_number - min_number; i++) { fuel_consumption[i] = fuel_consumption[i-1] + i; }

        // Actual simulation
        auto [best_consumption, best_target] = find_best_consumption(distribution, fuel_consumption);

        // Updates the results
        result_fuel_2 = best_consumption;
        result_target_2 = best_target;
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The best horizontal position that the crabs can align is %d.\n", result_target_2);
    printf("   The total consumption is %d of fuel.\n", result_fuel_2);

    return 0;
}
