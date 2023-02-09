#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>

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

struct Instruction {
    char dir = 'y';
    int index = 0;
};

class TransparentPaper
{
private:
    std::map<Point, int> _points;

    int _width = 0;
    int _height = 0;

    uint64_t _n_folds = 0;

private:
    void _fold_vertically(int index) {
        // Verification check
        if (index < 0 || index > _width) return;

        std::vector<Point> to_delete;

        for (auto [point, _] : _points) {
            if (point.x > index) {
                _points[{2*index - point.x, point.y}] = 1;
                to_delete.push_back(point);
            }
        }

        for (const auto & lost : to_delete) {
            _points.erase(lost);
        }

        _width = index;
        _n_folds++;
    }

    void _fold_horizontally(int index) {
        // Verification check
        if (index < 0 || index > _height) return;

        std::vector<Point> to_delete;

        for (auto [point, _] : _points) {
            if (point.y > index) {
                _points[{point.x, 2*index - point.y}] = 1;
                to_delete.push_back(point);
            }
        }

        for (const auto & lost : to_delete) {
            _points.erase(lost);
        }

        _height = index;
        _n_folds++;
    }

public:
    TransparentPaper() = delete;
    TransparentPaper(const std::vector<std::string>& raw_data) {
        // Populates the paper
        for (const auto & raw_point : raw_data) {
            auto point = string_split(raw_point, ",");

            int point_x = std::stoi(point[0]);
            int point_y = std::stoi(point[1]);

            // Updates the ranges
            if (_width < point_x) _width = point_x;
            if (_height < point_y) _height = point_y;

            ++_points[{point_x, point_y}];
        }
    }

    void fold(const Instruction& instruction) {
        if (instruction.dir == 'x') _fold_vertically(instruction.index);
        else if (instruction.dir == 'y') _fold_horizontally(instruction.index);
    }

    void view() const {
        for (int y = 0; y < _height; y++) {
            printf("      ");
            for (int x = 0; x < _width; x++) {
                _points.find({x,y}) == _points.end() ? printf(" ") : printf("#");
            }
            printf("\n");
        }
    }

    uint64_t n_folds() const { return _n_folds; }
    uint64_t n_points() const { return _points.size(); }
};



std::tuple<TransparentPaper, std::vector<Instruction>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<std::string> raw_points;
    std::vector<Instruction> instructions;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;

    // Read all the points that will be drawn into the transparent paper
    while(std::getline(input_file, line) && !line.empty()) {
        raw_points.push_back(line);
    }

    // Reads the folding instructions
    while(std::getline(input_file, line)) {
        auto inst = string_split(line, "=");
        instructions.push_back({inst[0][inst[0].size()-1], std::stoi(inst[1])});
    }

    return {TransparentPaper(raw_points), instructions};
}



int main(int argc, char* argv[]) {
    // Common
    const auto [PAPER, INSTRUCTIONS] = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Copy of the initial state of the paper
        TransparentPaper paper = PAPER;

        // Do just the first fold
        for (auto instruction : INSTRUCTIONS) {
            paper.fold(instruction);
            break;
        }

        // Gets the result
        result_1 = paper.n_points();
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   After the first fold, there are a total of %d points.\n", result_1);   



    // Part Two algorithms
    TransparentPaper paper_2 = PAPER; // Copy of the initial state of the paper

    float part_2_elapsed_time = time_block( [&](){
        // Do all the folding
        for (auto instruction : INSTRUCTIONS) {
            paper_2.fold(instruction);
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   After %lu folds, we look at the paper and see:\n", paper_2.n_folds());
    paper_2.view();

    return 0;
}
