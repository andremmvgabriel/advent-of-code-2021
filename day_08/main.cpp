#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

#include "../common/utils.hpp"


class Entry
{
private:
    std::vector<std::string> _signal_patterns;
    std::vector<std::string> _output_digits;

    std::unordered_map<std::string, std::string> _pattern2num;
    std::unordered_map<std::string, std::string> _num2pattern;

public:
    Entry() = delete;
    Entry(const std::string& raw_entry) {
        std::vector<std::string> parts = string_split(raw_entry, " | ");
        
        // Split the signal patterns from the output digits
        _signal_patterns = string_split(parts[0], " ");
        _output_digits = string_split(parts[1], " ");

        // Sorts every signal pattern
        for (auto & signal : _signal_patterns) {
            std::sort(signal.begin(), signal.end());
        }

        // Sorts every output digit
        for (auto & digit : _output_digits) {
            std::sort(digit.begin(), digit.end());
        }

        decipher();
        }

    void decipher() {
        // QoL function to check if all segments of a signal are contained in other
        auto contains_all = [](const std::string& seg1, const std::string& seg2){
            bool all_found = true;
            for (auto c : seg1) {
                if (seg2.find(c) == seg2.npos) {
                    all_found = false;
                    break;
                }
            }
            return all_found;
        };

        // Lets start by mapping the signals by its size
        std::unordered_map<int, std::vector<std::string>> segments_by_size;
        for (auto signal : _signal_patterns) {
            segments_by_size[signal.size()].push_back(signal);
        }

        // We already know 1, 4, 7, and 8
        _pattern2num[segments_by_size[2][0]] = "1"; _num2pattern["1"] = segments_by_size[2][0];
        _pattern2num[segments_by_size[3][0]] = "7"; _num2pattern["7"] = segments_by_size[3][0];
        _pattern2num[segments_by_size[4][0]] = "4"; _num2pattern["4"] = segments_by_size[4][0];
        _pattern2num[segments_by_size[7][0]] = "8"; _num2pattern["8"] = segments_by_size[7][0];

        // Lets now find 6
        for (auto seg = segments_by_size[6].begin(); seg != segments_by_size[6].end(); seg++) {
            if (!contains_all(segments_by_size[2][0], *seg)) {
                // Adds the new discovery into the pattern dictionaries
                _pattern2num[*seg] = "6";
                _num2pattern["6"] = *seg;

                // Removes the element (needed for next steps)
                segments_by_size[6].erase(seg);

                break;
            }
        }
        
        // Lets now find 3
        for (auto seg = segments_by_size[5].begin(); seg != segments_by_size[5].end(); seg++) {
            if (contains_all(segments_by_size[2][0], *seg)) {
                // Adds the new discovery into the pattern dictionaries
                _pattern2num[*seg] = "3";
                _num2pattern["3"] = *seg;

                // Removes the element (needed for next steps)
                segments_by_size[5].erase(seg);

                break;
            }
        }

        // Lets now find 2 and 5
        // > For this step, we will have start by finding the C segment character
        // > Then, this C segment will be used to distinguish between the 2 and 5 values
        std::string c_segment;
        for (auto c1 : _num2pattern["8"]) {
            if (_num2pattern["6"].find(c1) == _num2pattern["6"].npos) {
                c_segment = c1;
                break;
            }
        }
        
        if (segments_by_size[5][0].find(c_segment) == segments_by_size[5][0].npos) {
            // Adds the new discovery into the pattern dictionaries
            _pattern2num[segments_by_size[5][0]] = "5"; _num2pattern["5"] = segments_by_size[5][0];
            _pattern2num[segments_by_size[5][1]] = "2"; _num2pattern["2"] = segments_by_size[5][1];
        } else {
            // Adds the new discovery into the pattern dictionaries
            _pattern2num[segments_by_size[5][0]] = "2"; _num2pattern["2"] = segments_by_size[5][0];
            _pattern2num[segments_by_size[5][1]] = "5"; _num2pattern["5"] = segments_by_size[5][1];
        }

        // Lets now find 0 and 9
        if (contains_all(_num2pattern["5"], segments_by_size[6][0])) {
            // Adds the new discovery into the pattern dictionaries
            _pattern2num[segments_by_size[6][0]] = "9"; _num2pattern["9"] = segments_by_size[6][0];
            _pattern2num[segments_by_size[6][1]] = "0"; _num2pattern["0"] = segments_by_size[6][1];
        } else {
            // Adds the new discovery into the pattern dictionaries
            _pattern2num[segments_by_size[6][0]] = "0"; _num2pattern["0"] = segments_by_size[6][0];
            _pattern2num[segments_by_size[6][1]] = "9"; _num2pattern["9"] = segments_by_size[6][1];
        }
    }

    int count_output_instances_of(int num) const {
        int total = 0;
        for (const auto & digit : _output_digits) {
            if (_pattern2num.at(digit) == std::to_string(num)) { total++; }
        }
        return total;
    }

    int count_output_instances_of(const std::vector<int>& nums) const {
        int total = 0;
        for (const auto & num : nums) {
            total += count_output_instances_of(num);
        }
        return total;
    }

    int get_output() const {
        std::string str_number;

        for (const auto & str : _output_digits) {
            str_number += _pattern2num.at(str);
        }

        return std::stoi(str_number);
    }
};


std::vector<Entry> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<Entry> output;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while(std::getline(input_file, line)) {
        output.push_back(Entry(line));
    }

    return output;
}



int main(int argc, char* argv[]) {
    // Common
    const auto entries = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        for (const auto & entry : entries) {
            result_1 += entry.count_output_instances_of({1, 4, 7, 8});
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   There are a total of %d intances of the digits 1, 4, 7, or 8.\n", result_1);



    // Part Two algorithms
    int result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        for (auto entry : entries) {\
            // The current implementation has already this step done it its constructor.
            // > However, this part was the one that required the decipher of the signals
            // > We will re-do the decipher step for the sake of timing the part
            entry.decipher();
            result_2 += entry.get_output();
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The last board to win have a final score of %d.\n", result_2);  

    return 0;
}
