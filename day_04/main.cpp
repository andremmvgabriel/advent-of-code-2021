#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <numeric>
#include <array>
#include <tuple>
#include <cmath>
#include <unordered_map>

#include "../common/utils.hpp"

#define BOARD_SIZE 5



class Board
{
private:
    struct Position { int x = 0, y = 0; };

private:
    std::unordered_map<int, Position> _num2pos_map;
    std::unordered_map<std::string, std::array<std::vector<int>, BOARD_SIZE>> _register;

    bool _has_bingo;

private:
    bool _check_if_has_bingo(const Position &pos) {
        if (_register["row"][pos.y].size() == BOARD_SIZE) { _has_bingo = true; }
        if (_register["col"][pos.x].size() == BOARD_SIZE) { _has_bingo = true; }
        return _has_bingo;
    }

public:
    Board() = delete;
    Board(std::vector<std::string> board_data) : _has_bingo(false), _register({ {"row",{}}, {"col",{}} }) {
        // Unwraps the raw board data
        for (int i = 0; i < BOARD_SIZE; i++) {
            const std::string &line = board_data[i];
            for (int j = 0; j < BOARD_SIZE; j++) { 
                // Crops the number from the line
                std::string number(&line[3*j], &line[3*j+2]);

                // Inserts the number into the map
                _num2pos_map[std::stoi(number)] = {j,i};
            }
        }
    }

    bool register_number(const int& number) {
        if (_num2pos_map.find(number) == _num2pos_map.end()) { return false; }

        Position pos = _num2pos_map[number];

        // Registers the number in the register
        _register["row"][pos.y].push_back(number);
        _register["col"][pos.x].push_back(number);

        // Removes the number from the map
        _num2pos_map.erase(number);

        // Checks if the board has Bingo!
        return _check_if_has_bingo(pos);
    }

    bool has_bingo() { return _has_bingo; }

    std::vector<int> get_remaining_values() {
        std::vector<int> output;

        for (const auto & [value, pos] : _num2pos_map ) {
            output.push_back(value);
        }

        return output;
    }
};



std::tuple<std::vector<int>, std::vector<Board>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<int> numbers;
    std::vector<Board> boards;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // First line is always the random numbers
    std::string line;
    std::getline(input_file, line);

    // Reads the random numbers
    std::vector<std::string> str_numbers = string_split(line, ",");
    std::transform(str_numbers.begin(), str_numbers.end(), std::back_inserter(numbers), [](const std::string &value){return std::stoi(value);});

    // The other lines have the boards
    while (true) {
        // First line is to be ignore - is empty
        if (!std::getline(input_file, line)) break;

        std::vector<std::string> board_data;

        for (int i = 0 ; i < BOARD_SIZE ; i++) {
            std::getline(input_file, line);
            board_data.push_back(line);
        }

        boards.push_back(board_data);
    }

    return {numbers, boards};
}



int main(int argc, char* argv[]) {
    // Common
    const auto [NUMBERS, BOARDS] = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 1;

    float part_1_elapsed_time = time_block( [&](){
        // Makes a copy of the boards
        std::vector<Board> boards = BOARDS;

        // Control variable to check if someone made bingo!
        bool bingo = false;

        // Withdraws the numbers
        for (const auto & number : NUMBERS) {
            for (auto & board : boards) {
                // Registers the withdrawn number into the board
                bingo = board.register_number(number);

                // Checks if the board has bingo!
                if (bingo) {
                    // Gets the values from the board that were not withdrawn
                    std::vector<int> remaining_values = board.get_remaining_values();
                    
                    // Updates the final result value with the sum of all the remaining values
                    result_1 *= std::accumulate(remaining_values.begin(), remaining_values.end(), 0);

                    break;
                }
            }

            // Updates the final result value by multiplying it with the last withdrawn number
            if (bingo) { result_1 *= number; break; }
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   To guarantee victory agaisnt the giant squid, the best board results in a final score of %d.\n", result_1);   



    // Part Two algorithms
    int result_2 = 1;

    float part_2_elapsed_time = time_block( [&](){
        // Makes a copy of the boards
        std::vector<Board> boards = BOARDS;

        // Control variable to know how many boards are already completed
        int completed_boards = 0;

        // Withdraws the numbers
        for (const auto & number : NUMBERS) {
            for (auto & board : boards) {
                // Ignores the board if it already has bingo
                if ( board.has_bingo() ) { continue; }

                // Checks if the board has bingo!
                if (board.register_number(number)) {
                    // Updates the amount of boards that were already completed
                    completed_boards++;

                    // Only cares to calculate the result if this was the last board to win
                    if (completed_boards == boards.size()) {
                        // Gets the values from the board that were not withdrawn
                        std::vector<int> remaining_values = board.get_remaining_values();
                        
                        // Updates the final result value with the sum of all the remaining values
                        result_2 *= std::accumulate(remaining_values.begin(), remaining_values.end(), 0);
                    }
                }
            }

            // Updates the final result value by multiplying it with the last withdrawn number
            if (completed_boards == boards.size()) { result_2 *= number; break; }
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The last board to win have a final score of %d.\n", result_2);  

    return 0;
}
