#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <array>
#include <tuple>

#include "../common/utils.hpp"

class Cube
{
public:
    std::array<int, 2> x_range, y_range, z_range;

public:
    Cube() = delete;
    Cube(const std::string &raw_data) {
        std::vector<std::string> xyz = string_split(raw_data, ",");

        // X parsing
        std::vector<std::string> x_parts = string_split(std::string(xyz[0].begin() + 2, xyz[0].end()), "..");
        x_range[0] = std::stoi(x_parts[0]);
        x_range[1] = std::stoi(x_parts[1]);

        // Y parsing
        std::vector<std::string> y_parts = string_split(std::string(xyz[1].begin() + 2, xyz[1].end()), "..");
        y_range[0] = std::stoi(y_parts[0]);
        y_range[1] = std::stoi(y_parts[1]);

        // Z parsing
        std::vector<std::string> z_parts = string_split(std::string(xyz[2].begin() + 2, xyz[2].end()), "..");
        z_range[0] = std::stoi(z_parts[0]);
        z_range[1] = std::stoi(z_parts[1]);
    }
};

class CuboidsGrid
{
private:
    std::vector<std::vector<std::vector<uint8_t>>> _grid;

    int _x_min = 0, _y_min = 0, _z_min = 0;

public:
    CuboidsGrid() = delete;
    CuboidsGrid(std::array<int, 2> x_limits, std::array<int, 2> y_limits, std::array<int, 2> z_limits) : _x_min(x_limits[0]), _y_min(y_limits[0]), _z_min(z_limits[0]), _grid(
        z_limits[1] - z_limits[0] + 1, std::vector<std::vector<uint8_t>>(
            y_limits[1] - y_limits[0] + 1, std::vector<uint8_t>(
                x_limits[1] - x_limits[0] + 1, 0
            )
        )
    ) {}

    void place_cube(const Cube& cube, uint8_t action) {
        for (int x = cube.x_range[0]; x <= cube.x_range[1]; x++) {
            for (int y = cube.y_range[0]; y <= cube.y_range[1]; y++) {
                for (int z = cube.z_range[0]; z <= cube.z_range[1]; z++) {
                    _grid[z-_z_min][y-_y_min][x-_x_min] = action;
                }
            }
        }
    }


    int count_cuboids() {
        int counter = 0;

        for (const auto & matrix : _grid) {
            for (const auto & row : matrix) {
                for (const auto & value : row) {
                    if (value) counter++;
                }
            }
        }

        return counter;
    }
};

std::tuple<std::vector<uint8_t>, std::vector<Cube>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<uint8_t> actions;
    std::vector<Cube> cubes;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while (std::getline(input_file, line)) {
        std::vector<std::string> parts = string_split(line, " ");
        actions.push_back( parts[0] == "on" ? 1 : 0 );
        cubes.push_back( {parts[1]} );
    }

    return {actions, cubes};
}



int main(int argc, char* argv[]) {
    // Common
    const auto [ACTIONS, CUBES] = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        std::array<int, 2> x_range {0,0}, y_range {0,0}, z_range {0,0};

        for (const auto & cube : CUBES) {
            if (cube.x_range[0] < x_range[0]) x_range[0] = cube.x_range[0];
            if (cube.x_range[1] > x_range[1]) x_range[1] = cube.x_range[1];
            if (cube.y_range[0] < y_range[0]) y_range[0] = cube.y_range[0];
            if (cube.y_range[1] > y_range[1]) y_range[1] = cube.y_range[1];
            if (cube.z_range[0] < z_range[0]) z_range[0] = cube.z_range[0];
            if (cube.z_range[1] > z_range[1]) z_range[1] = cube.z_range[1];
        }

        printf("X: %d %d\n", x_range[0], x_range[1]);
        printf("y: %d %d\n", y_range[0], y_range[1]);
        printf("z: %d %d\n", z_range[0], z_range[1]);
        uint64_t total = (x_range[1]-x_range[0]+1) * (y_range[1]-y_range[0]+1) * (z_range[1]-z_range[0]+1);
        printf("TOTAL: %lu\n", total);

        // CuboidsGrid grid(x_range, y_range, z_range);

        for (int i = 0 ; i < ACTIONS.size() ; i++) {
            // grid.place_cube(CUBES[i], ACTIONS[i]);
        }

        // result_1 = grid.count_cuboids();
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   In the end, there are %d cuboids.\n", result_1);   



    // Part Two algorithms
    float part_2_elapsed_time = time_block( [&](){
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    // printf("   The last board to win have a final score of %d.\n", result_2);  

    return 0;
}
