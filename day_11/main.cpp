#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <cstdint>
#include <algorithm>

#include "../common/utils.hpp"



struct Point {
    int x = 0, y = 0;

    bool operator<(Point const &p) const {
        return x < p.x || (x == p.x && y < p.y);
    }

    bool operator==(Point const &p) const {
        return (x==p.x) && (y==p.y);
    }
};

class Cavern
{
private:
    std::map<Point, int> _octopuses;
    int _x_range = 0;
    int _y_range = 0;

    uint64_t _total_steps = 0;
    uint64_t _total_flashes = 0;

private:
    std::vector<Point> _get_neighbours(const Point& point) {
        return {
            {point.x - 1, point.y    }, // Left
            {point.x - 1, point.y - 1}, // Top left
            {point.x    , point.y - 1}, // Top
            {point.x + 1, point.y - 1}, // Top right
            {point.x + 1, point.y    }, // Right
            {point.x + 1, point.y + 1}, // Bottom right
            {point.x    , point.y + 1}, // Bottom
            {point.x - 1, point.y + 1}  // Bottom left
        };
    }

    std::vector<Point> _flash(const Point& point, std::vector<Point>& flashed_octopuses) {
        // Resets its value
        _octopuses[point] = 0;
        flashed_octopuses.push_back(point);

        // Gives a list of all its valid neighbours that can be affected the flash
        std::vector<Point> neighbours;
        for (const auto & neighbour : _get_neighbours(point)) {
            // Not a valid point if out of grid's range
            if (neighbour.x < 0 || neighbour.x >= _x_range) { continue; }
            if (neighbour.y < 0 || neighbour.y >= _y_range) { continue; }
            
            // Adds the neighbour to the output list
            neighbours.push_back(neighbour);
        }

        return neighbours;
    }

    uint64_t _next_step() {
        // This list will control what points were already flashed
        std::vector<Point> flashed_octopuses;

        // This will contain all the neighbours of all the points that got flashed in the current step
        std::vector<Point> neighbours_to_analyse;

        // First, flash the octopuses that "naturally" flash by incrementing the step
        for (auto & [pos, val] : _octopuses) {
            if (++val > 9) {
                std::vector<Point> neighbours = _flash(pos, flashed_octopuses);
                neighbours_to_analyse.insert(neighbours_to_analyse.end(), neighbours.begin(), neighbours.end());
            }
        }

        // Second, let's analysed the neighbours of all octopuses that flashed
        while (neighbours_to_analyse.size()) {
            // Current point being analysed
            const Point& point = neighbours_to_analyse.back(); neighbours_to_analyse.pop_back();

            // Do not proceed if this point was already flashed in this step
            if (std::find_if(flashed_octopuses.begin(), flashed_octopuses.end(), [point](Point const& other){ return point == other; }) != flashed_octopuses.end()) { continue; }

            // Check if point will flash or just increase its value
            if (++_octopuses[point] > 9) {
                std::vector<Point> neighbours = _flash(point, flashed_octopuses);
                neighbours_to_analyse.insert(neighbours_to_analyse.end(), neighbours.begin(), neighbours.end());
            }
        }

        return flashed_octopuses.size();
    }

public:
    Cavern() = delete;
    Cavern(const std::vector<std::string>& raw_data) : _x_range(raw_data[0].size()), _y_range(raw_data.size()) {
        // Populates the map
        for (int y = 0 ; y < _y_range; y++) {
            for (int x = 0 ; x < _x_range; x++) {
                _octopuses[{x,y}] = raw_data[y][x] - '0';
            }
        }
    }

    void simulate(uint64_t simulation_steps) {
        // Variables that will:
        // > store all the steps performed in the simulation
        // > store all the flashes performed in the simulation
        uint64_t steps = 0;
        uint64_t flashes = 0;

        for (uint64_t i = 0 ; i < simulation_steps; i++) {
            // Gets the amount of flashes of the current step
            uint64_t step_flashes = _next_step();

            // Increases the control variables
            steps++;
            flashes += step_flashes;

            // Stop the simulation if all of the octopuses flash simultaneously
            if (step_flashes == _octopuses.size()) break;
        }

        // Updates the total amount steps and flashes
        _total_steps += steps;
        _total_flashes += flashes;
    }

    int x_range() { return _x_range; }
    int y_range() { return _y_range; }
    int total_steps() { return _total_steps; }
    int total_flashes() { return _total_flashes; }
};

Cavern parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<std::string> octopuses;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while (std::getline(input_file, line)) {
        octopuses.push_back(line);
    }

    return Cavern{octopuses};
}



int main(int argc, char* argv[]) {
    // Common
    const auto & cavern = parse_inputs(argc, argv);

    // Part One algorithms
    const uint64_t number_steps_1 = 100;
    uint64_t result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Copy of the cavern
        Cavern cavern_1 = cavern;

        // Simulation
        cavern_1.simulate(number_steps_1);
        result_1 = cavern_1.total_flashes();
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   After %lu steps there were a total of %lu flashes.\n", number_steps_1, result_1);   



    // Part Two algorithms
    const uint64_t number_steps_2 = -1; // Unsigned -> This is max value of uint 64 bits.
    uint64_t result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        // Copy of the cavern
        Cavern cavern_2 = cavern;

        // Simulation
        cavern_2.simulate(number_steps_2);
        result_2 = cavern_2.total_steps();
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   All the octopuses flash simultaneously after step %lu.\n", result_2);  

    return 0;
}
