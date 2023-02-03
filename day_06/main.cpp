#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <cstdint>

#include "../common/utils.hpp"

#define MAX_GLOWFISH_TIMER 7



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

int64_t simulate_glowfish_growth(const int & sim_time, const std::vector<int> & fishes) {
    // Creates a map with all the glowfishes timer possibilities, and the amount on each timer
    std::map<int64_t, int64_t> glowfish_timers;
    for (const auto & fish : fishes) { ++glowfish_timers[fish]; }

    // Actual simulation
    for (int day = 1; day <= sim_time; day++) {
        // Temporary variable to 
        int64_t timer_0_cache = glowfish_timers[0];

        // Decreases all the timers
        for (int i = 0; i < MAX_GLOWFISH_TIMER + 1; i++) { glowfish_timers[i] = glowfish_timers[i+1]; }

        // Handles the 0 timer glowfishes
        // > The already existent go to position MAX-1
        // > The newborns got to position Max+1 with same amount as the ones that created them
        glowfish_timers[MAX_GLOWFISH_TIMER-1] += timer_0_cache;
        glowfish_timers[MAX_GLOWFISH_TIMER+1] = timer_0_cache;
    }

    // Counts all the existent glowfishes in the current day
    int64_t fishes_amout = 0;
    for (const auto & [timer, amount] : glowfish_timers) { fishes_amout += amount; }

    return fishes_amout;
}



int main(int argc, char* argv[]) {
    // Common
    const auto numbers = parse_inputs(argc, argv);

    // Part One algorithms
    int SIMULATION_TIME_1 = 80;
    int64_t result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        result_1 = simulate_glowfish_growth(SIMULATION_TIME_1, numbers);
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   After %d days there are a total of %lu glowfishes.\n", SIMULATION_TIME_1, result_1);



    // Part Two algorithms
    int SIMULATION_TIME_2 = 256;
    int64_t result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        result_2 = simulate_glowfish_growth(SIMULATION_TIME_2, numbers);
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   After %d days there are a total of %lu glowfishes.\n", SIMULATION_TIME_2, result_2);

    return 0;
}
