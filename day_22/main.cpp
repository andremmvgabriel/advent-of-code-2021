#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <array>
#include <tuple>
#include <unordered_map>

#include "../common/utils.hpp"

struct Cuboid
{
    int x = 0, y = 0, z = 0;

    bool operator==(Cuboid const &c) const {
        return x == c.x && y == c.y && z == c.z;
    }

    struct Hasher
    {
        std::size_t operator()(const Cuboid& c) const
        {
            size_t h1 = std::hash<double>()(c.x);
            size_t h2 = std::hash<double>()(c.y);
            size_t h3 = std::hash<double>()(c.z);
            return (h1 ^ (h2 << 1)) ^ h3;
        }
    };
};

class Cube
{
public:
    std::array<int, 2> x_range, y_range, z_range;

public:
    Cube() = delete;
    Cube(const std::string &raw_data) {
        std::vector<std::string> xyz = string_split(raw_data, ",");

        // X parsing
        std::vector<std::string> x_parts = string_split(xyz[0].substr(2), "..");
        x_range[0] = std::stoi(x_parts[0]);
        x_range[1] = std::stoi(x_parts[1]);

        // Y parsing
        std::vector<std::string> y_parts = string_split(xyz[1].substr(2), "..");
        y_range[0] = std::stoi(y_parts[0]);
        y_range[1] = std::stoi(y_parts[1]);

        // Z parsing
        std::vector<std::string> z_parts = string_split(xyz[2].substr(2), "..");
        z_range[0] = std::stoi(z_parts[0]);
        z_range[1] = std::stoi(z_parts[1]);
    }
};

class CuboidsGrid
{
private:
    std::unordered_map<Cuboid, uint8_t, Cuboid::Hasher> _cuboids;

    int _x_min = 0, _y_min = 0, _z_min = 0;

public:

    void place_cube(const Cube& cube, uint8_t action) {
        if (cube.x_range[1] < -50 || cube.x_range[0] > 50) return;
        if (cube.y_range[1] < -50 || cube.y_range[0] > 50) return;
        if (cube.z_range[1] < -50 || cube.z_range[0] > 50) return;

        std::array<int, 2> x_range { cube.x_range[0] < -50 ? -50 : cube.x_range[0], cube.x_range[1] > 50 ? 50 : cube.x_range[1]};
        std::array<int, 2> y_range { cube.y_range[0] < -50 ? -50 : cube.y_range[0], cube.y_range[1] > 50 ? 50 : cube.y_range[1]};
        std::array<int, 2> z_range { cube.z_range[0] < -50 ? -50 : cube.z_range[0], cube.z_range[1] > 50 ? 50 : cube.z_range[1]};

        for (int x = x_range[0]; x <= x_range[1]; x++) {
            for (int y = y_range[0]; y <= y_range[1]; y++) {
                for (int z = z_range[0]; z <= z_range[1]; z++) {
                    _cuboids[{x,y,z}] = action;
                }
            }
        }
    }


    int count_cuboids() {
        int counter = 0;

        for (const auto & cuboid : _cuboids) {
            if (cuboid.second) counter++;
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
        CuboidsGrid grid;

        for (int i = 0 ; i < ACTIONS.size() ; i++) {
            grid.place_cube(CUBES[i], ACTIONS[i]);
        }

        result_1 = grid.count_cuboids();
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
