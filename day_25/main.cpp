#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <stdexcept>

#include "../common/utils.hpp"

// template<typename T>
using Matrix = std::vector<std::vector<uint8_t>>;

std::tuple<Matrix, Matrix> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    Matrix e_map, s_map;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while (std::getline(input_file, line)) {
        std::vector<uint8_t> e_line, s_line;
        for (const auto & c : line) {
            if (c == '>') {
                e_line.push_back(1);
                s_line.push_back(0);
            } else if (c == 'v') {
                e_line.push_back(0);
                s_line.push_back(1);
            } else {
                e_line.push_back(0);
                s_line.push_back(0);
            }
        }
        e_map.push_back(e_line);
        s_map.push_back(s_line);
    }

    return {e_map, s_map};
}



int main(int argc, char* argv[]) {
    // Common
    const auto [E_MAP, S_MAP] = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Copy of the maps
        Matrix e_map = E_MAP, s_map = S_MAP;

        std::vector<std::string> prints (e_map.size(), "");

        while (true) {
            // Control variables
            uint8_t moved = 0x00;

            // Maps for next moves
            Matrix next_e_map (e_map.size(), std::vector<uint8_t>(e_map[0].size(), 0));
            Matrix next_s_map (s_map.size(), std::vector<uint8_t>(s_map[0].size(), 0));

            // Move the East arrows
            for (int y = 0 ; y < e_map.size() ; y++) {
                for (int x = 0 ; x < e_map[y].size() ; x++) {
                    if (e_map[y][x]) {
                        if (!e_map[y][(x+1)%e_map[y].size()] && !s_map[y][(x+1)%s_map[y].size()]) {
                            next_e_map[y][(x+1)%e_map[y].size()] = 1;
                            moved |= 1;
                        } else {
                            next_e_map[y][x] = 1;
                        }
                    }
                }
            }
            e_map = next_e_map;

            // Move the South arrows
            for (int y = 0 ; y < s_map.size() ; y++) {
                for (int x = 0 ; x < s_map[y].size() ; x++) {
                    if (s_map[y][x]) {
                        if (!e_map[(y+1)%e_map.size()][x] && !s_map[(y+1)%e_map.size()][x]) {
                            next_s_map[(y+1)%s_map.size()][x] = 1;
                            moved |= 1;
                        } else {
                            next_s_map[y][x] = 1;
                        }
                    }
                }
            }
            s_map = next_s_map;

            result_1++;
            if (!moved) break;
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
