#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <stdexcept>

#include "../common/utils.hpp"

// template<typename T>
using Matrix = std::vector<std::vector<char>>;

Matrix parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    Matrix map;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while (std::getline(input_file, line)) {
        std::vector<char> line_nums;
        for (const auto & c : line) {
            line_nums.push_back(c);
        }
        map.push_back(line_nums);
    }

    return map;
}



int main(int argc, char* argv[]) {
    // Common
    const auto MAP = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Copy of the map
        Matrix map_1 = MAP;

        std::vector<std::string> prints (map_1.size(), "");

        while (true) {
            // Control variables
            uint8_t moved = 0x00;

            for (int y = 0 ; y < map_1.size() ; y++) {
                for (int x = 0 ; x < map_1[y].size() ; x++) {
                    prints[y].push_back(map_1[y][x]);
                }
                prints[y].push_back('\t');
            }

            // New Map
            Matrix next_state (map_1.size(), std::vector<char>(map_1[0].size(), '.'));

            // Move the East arrows
            for (int y = 0 ; y < map_1.size() ; y++) {
                for (int x = 0 ; x < map_1[y].size() ; x++) {
                    if (map_1[y][x] == '>') {
                        if (map_1[y][(x+1)%map_1[y].size()] == '.') {
                            next_state[y][(x+1)%map_1[y].size()] = '>';
                            moved |= 1;
                        } else {
                            next_state[y][x] = '>';
                        }
                    }
                }
            }

            // Move the South arrows
            for (int y = 0 ; y < map_1.size() ; y++) {
                for (int x = 0 ; x < map_1[y].size() ; x++) {
                    if (map_1[y][x] == 'v') {
                        if (map_1[(y+1)%map_1.size()][x] == '.') {
                            next_state[(y+1)%map_1.size()][x] = 'v';
                            moved |= 1;
                        } else {
                            next_state[y][x] = 'v';
                        }
                    }
                }
            }

            map_1 = next_state;

            result_1++;
            // printf(">%d\n", result_1);
            if (result_1 >= 7) break;
            if (!moved) break;
        }

        for (int i = 0 ; i < prints.size() ; i++) {
            printf("%s\n", prints[i].c_str());
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   No more moves can be done after %d moves.\n", result_1);   



    // Part Two algorithms
    float part_2_elapsed_time = time_block( [&](){
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    // printf("   The last board to win have a final score of %d.\n", result_2);  

    return 0;
}
