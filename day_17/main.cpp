#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <array>
#include <tuple>

#include "../common/utils.hpp"



std::tuple<std::array<int, 2>, std::array<int, 2>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::array<int, 2> x_range {};
    std::array<int, 2> y_range {};

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    std::getline(input_file, line);

    std::vector<std::string> parts = string_split(line, " ");

    // Parse the X RANGE
    std::string x_limits = std::string(parts[2].begin() + 2, parts[2].end() - 1); // -1 removes the ,
    std::vector<std::string> x_limits_parts = string_split(x_limits, "..");

    x_range[0] = std::stoi(x_limits_parts[0]);
    x_range[1] = std::stoi(x_limits_parts[1]);

    // Parse the Y RANGE
    std::string y_limits = std::string(parts[3].begin() + 2, parts[3].end());
    std::vector<std::string> y_limits_parts = string_split(y_limits, "..");

    y_range[0] = std::stoi(y_limits_parts[0]);
    y_range[1] = std::stoi(y_limits_parts[1]);

    return {x_range, y_range};
}



int main(int argc, char* argv[]) {
    // Common
    const auto [X_RANGE, Y_RANGE] = parse_inputs(argc, argv);
    const std::array<int, 2> INITIAL_POS {0, 0};

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Part 1 Theory / Important notes:
        // > This part is completely independent of the x movement component
        // > A throw that makes the object reach the farthest y position in the zone will achieve the best possible height
        //   > In this farthest position...
        //      > the velocity will always be negative
        //      > the velocity vector calculated is referenced from the previous point (it will be the vector that made the movement possible to the point)
        //      > this velocity vector...
        //          > is -||initial_pos - position|| if y final is below y initial
        //          > is -||initial_pos - position|| + 1 if y final is above y initial
        // (This solution is dynamic for any possible zone)

        // Distance between the y initial position and y zone limits
        // > dy: absolute distance
        // > mdy: module of the absolute distance
        int dy0 = INITIAL_POS[1] - Y_RANGE[0], mdy0 = std::abs(dy0);
        int dy1 = INITIAL_POS[1] - Y_RANGE[1], mdy1 = std::abs(dy1);

        // Calculates the velocity in that farthest position
        // > y initial > y zone: -||initial_pos - position|| + 1
        // > y initial < y zone: -||initial_pos - position||
        int velocity = mdy0 > mdy1 ? dy0 > 0 ? -mdy0 : -mdy0+1 : dy1 > 0 ? -mdy1 : -mdy1+1;

        // Variable that will store the max height achieved
        // > It starts as the lowest possible position and we back trace from there
        int max_height = mdy0 > mdy1 ? Y_RANGE[0] : Y_RANGE[1];

        // Back tracing until velocity is 0 (top of the parable)
        while(velocity < 0) {
            max_height -= velocity;
            velocity++;
        }
        
        result_1 = max_height;
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   The highest height the thrown object achieved was %d.\n", result_1);   



    // Part Two algorithms
    int result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        // Part 2 Theory / Important notes:
        // > This part is just pure brute force
        // > The solution will iterate between:
        //   > X: -||Farthest X value|| to ||Farthest X value||
        //   > Y: -||Farthest Y value|| to ||Farthest Y value||
        //   > Other than these ranges, the object will never fall in the zone.
        // > For each X,Y pair iterated, this pair will symbolize the initial velocity
        //   > Then, it will simulate the position and velocity variations until the object hits the zone, or passes it. 
        // (This solution is dynamic for any possible zone)

        // Find farthest X and Y values
        int far_x = std::abs(X_RANGE[0]) > std::abs(X_RANGE[1]) ? std::abs(X_RANGE[0]) : std::abs(X_RANGE[1]); 
        int far_y = std::abs(Y_RANGE[0]) > std::abs(Y_RANGE[1]) ? std::abs(Y_RANGE[0]) : std::abs(Y_RANGE[1]); 

        for (int x = -far_x ; x <= far_x; x++) {
            for (int y = -far_y ; y <= far_y; y++) {
                // Setups the initial velocity and position of the X,Y pair
                std::array<int, 2> velocity {x, y};
                std::array<int, 2> position = INITIAL_POS;

                // Simulation
                // > This will simulate as long as the object does not pass the zone limits (in both axis)
                // > OR! If the object reaches the zone at any point of the simulation
                while (position[0] < X_RANGE[1] && position[1] > Y_RANGE[0]) {
                    // Updates the position
                    position = { position[0] + velocity[0], position[1] + velocity[1] };

                    // Updates the velocity
                    velocity = {
                        velocity[0] == 0 ? 0 : velocity[0] > 0 ? (velocity[0] - 1) : (velocity[0] + 1),
                        velocity[1] - 1
                    };

                    // Checks if the current movement change made the object reachs the zone
                    if (position[0] >= X_RANGE[0] && position[0] <= X_RANGE[1] && position[1] >= Y_RANGE[0] && position[1] <= Y_RANGE[1]) {
                        result_2++;
                        break;
                    }
                }
            }
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   There are a total of %d different initial velocities that can be used that will make the object fall in the zone.\n", result_2);  

    return 0;
}
