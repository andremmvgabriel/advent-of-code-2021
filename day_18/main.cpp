#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

#include "../common/utils.hpp"



struct Node {
    int value = 0;
    int depth = 0;
};

class Snailfish
{
private:
    bool _reduced = false;
    std::vector<Node> _numbers;

private:
    void _parse_raw_numbers(const std::string& raw_numbers) {
        // Depth control variable
        int depth = 0;

        // Reads all the characters in the input
        for (const auto c : raw_numbers) {
            if (c == '[') { depth++; continue; } // Increase depth if open bracket
            if (c == ']') { depth--; continue; } // Decrease depth if open bracket
            if (c == ',') { continue; } // Ignore if comma

            // Adds the number into the list
            _numbers.push_back({c - '0',depth});
        }
    }

    int _pair_magnitude(const Node& node1, const Node& node2) const {
        return node1.value * 3 + node2.value * 2;
    }

    void _explode(int index) {
        // Adds the LEFT exploded value into its LEFT adjacent neighbour (if exists)
        if (index > 0) {  _numbers[index-1].value += _numbers[index].value; }

        // Adds the RIGHT exploded value into its RIGHT adjacent neighbour (if exists)
        if (index < _numbers.size() - 2) {  _numbers[index+2].value += _numbers[index+1].value; }

        // Updates the RIGHT exploded value
        // > This will represent the pair that exploded
        // > Needs to replace its values as 0 and reduce its depth by 1
        _numbers[index+1].value = 0;
        _numbers[index+1].depth--;

        // Removed the LEFT exploded number from the list
        _numbers.erase(_numbers.begin()+index, _numbers.begin()+index+1);
    }

    void _split(int index) {
        // Already inserts a new node into the snailfish
        _numbers.insert(_numbers.begin() + index, _numbers[index]);

        // Replaces the old node in the list with the new value and depth
        _numbers[index].value /= 2;
        _numbers[index].depth++;
        
        // Replaces the new node in the list with the new value and depth
        ++_numbers[index+1].value /= 2;
        _numbers[index+1].depth++;
    }

public:
    Snailfish() = delete;
    Snailfish(const std::string& raw_numbers) : _reduced(false) {
        _parse_raw_numbers(raw_numbers);
    }

    void reduce() {
        // Control variables
        int action = 0; // Control if we are searching for a explode criteria or split criteria
        bool reducing = true; // Control if no actions were performed, to stop the reduce processing

        // Actual reduce processing
        while (reducing) {
            // Explode
            if (action % 2 == 0) {
                for (int i = 0 ; i < _numbers.size() - 1 ; i++) {
                    if (_numbers[i].depth >= 5 && _numbers[i].depth == _numbers[i+1].depth) {
                        _explode(i);
                        i = 0; // Restart the whole explode searching
                    }
                }
            }
            // Split
            else {
                reducing = false;
                for (int i = 0 ; i < _numbers.size() ; i++) {
                    if (_numbers[i].value >= 10) {
                        _split( i);
                        reducing = true;
                        break; // One split has to restart the whole search
                    }
                }
            }

            // Next action search!
            action++;
        }

        // Update the reduced control variable
        _reduced = true;
    }

    void add(const Snailfish &other) {
        // Updates the reduced control variable
        _reduced = false;

        // Adds the other snailfish numbers into the numbers in this snailfish
        _numbers.insert(_numbers.end(), other._numbers.begin(), other._numbers.end());

        // Increases all the numbers depth's by 1
        std::transform(_numbers.begin(), _numbers.end(), _numbers.begin(), [&](Node n){ return Node{n.value, n.depth+1}; });
    }

    int magnitude() const {
        // Verification check
        if (!_reduced) { return -1; }

        // Copy of the numbers (we don't want to mutate the ones in the class)
        std::vector<Node> numbers = _numbers;

        // We know it is impossible to have numbers with depth > 4 since the snailfish has to be reduced
        for (int i = 4 ; i > 0 ; i--) {
            // Goes through all the numbers in the list
            for (int j = numbers.size() - 1 ; j > 0 ; j--) {
                // If a pair with the current depth searh is found
                if (numbers[j-1].depth == i && numbers[j].depth == i) {
                    // Calculates its magnitude, representing it as a single number with its depth reduced by 1
                    numbers[j].value = _pair_magnitude(numbers[j-1], numbers[j]);
                    numbers[j].depth--;

                    // Removes the other number that was used to calculate the magnitude of the pair
                    numbers.erase(numbers.begin() + j - 1, numbers.begin() + j);
                }
            }
        }

        // Here, only a single number will be present in the list
        return numbers[0].value;
    }
};

std::vector<Snailfish> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<Snailfish> snailfishes; 

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while (std::getline(input_file, line)) {
        snailfishes.emplace_back(Snailfish{line});
    }

    return snailfishes;
}



int main(int argc, char* argv[]) {
    // Common
    const auto SNAILFISHES = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        Snailfish final_snailfish = SNAILFISHES[0];

        for (int i = 1 ; i < SNAILFISHES.size() ; i++) {
            final_snailfish.add(SNAILFISHES[i]);
            final_snailfish.reduce();
        }

        result_1 = final_snailfish.magnitude();
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   All the snailfishes summed result in a final magnitude of %d\n", result_1);   



    // Part Two algorithms
    int result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        for (int i = 0 ; i < SNAILFISHES.size() ; i++) {
            for (int j = 0 ; j < SNAILFISHES.size() ; j++) {
                // We dont want to add the same snailfish (X + X)
                if (i == j) { continue; }

                // Adds the two snailfishes
                Snailfish snail = SNAILFISHES[i];
                snail.add(SNAILFISHES[j]);

                // Reduce & gets the magnitude
                snail.reduce();
                int magnitude = snail.magnitude();

                // Updates the max magnitude
                if (result_2 < magnitude) result_2 = magnitude;
            }
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The largest magnitude of any sum of two different snailfish numbers is %d\n", result_2);   

    return 0;
}
