#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <queue>
#include <algorithm>

#include "../common/utils.hpp"

struct Point {
    int x = 0;
    int y = 0;
    int height = 0;

    bool operator<(Point const &p) const {
        return x < p.x || (x == p.x && y < p.y);
    }

    bool operator==(Point const &p) const {
        return (x==p.x) && (y==p.y);
    }
};

class Heightmap
{
private:
    std::vector<std::vector<Point>> _data;

public:
    Heightmap() = delete;
    Heightmap(const std::vector<std::vector<int>>& data) {
        // Wraps the data from a matrix of ints to a matrix of Points
        for (int y = 0; y < data.size(); y++) {
            std::vector<Point> line;
            for (int x = 0; x < data[y].size(); x++) {
                line.push_back( {x, y, data[y][x]} );
            }
            _data.push_back(line);
        }
    }

    int y_range() const { return _data.size(); }
    int x_range() const { return _data[0].size(); }

    std::vector<Point> find_low_points() const {
        // List of low points that will be returned
        std::vector<Point> low_points;

        // Goes through all the points in the map
        for (int y = 0; y < y_range(); y++) {
            for (int x = 0; x < x_range(); x++) {
                // Current point being analysed
                // > It is considered a low point if its height is inferior than all its neighbours
                const Point& point = _data[y][x];

                // Top and Left checks
                if (y > 0 && point.height >= _data[y-1][x].height) continue;
                if (x > 0 && point.height >= _data[y][x-1].height) continue;

                // Bottom and Right checks
                if (y < y_range()-1 && point.height >= _data[y+1][x].height) continue;
                if (x < x_range()-1 && point.height >= _data[y][x+1].height) continue;

                // It's a low point!
                low_points.push_back( point );
            }
        }

        return low_points;
    }
    
    std::vector<std::vector<Point>> find_basins() const {
        // There are as many basins as low points
        std::vector<Point> low_points = find_low_points();

        // List of basins that will be returned
        std::vector<std::vector<Point>> basins; basins.reserve(low_points.size());

        for (const auto & low_point : low_points) {

            // Points list for the current basin
            std::vector<Point> basin;

            // Find all the basin points (starting in the low point)
            std::queue<Point> next_to_analyse; next_to_analyse.push(low_point);
            while (!next_to_analyse.empty()) {
                // Current point being analysed
                Point point = next_to_analyse.front(); next_to_analyse.pop();

                // Do not proceed if the current point is the maximum height
                if (point.height == 9) { continue; }

                // Do not proceed if this point is already part of the basin
                if (std::find_if(basin.begin(), basin.end(), [point](Point const& other){ return point == other; }) != basin.end()) { continue; }

                // It is a point of the basin!
                basin.push_back(point);

                // Top
                if (point.y > 0 && point.height <= _data[point.y-1][point.x].height) {
                    next_to_analyse.push({point.x, point.y-1, _data[point.y-1][point.x].height});
                }

                // Bottom
                if (point.y < y_range() - 1 && point.height <= _data[point.y+1][point.x].height) {
                    next_to_analyse.push({point.x, point.y+1, _data[point.y+1][point.x].height});
                }

                // Right
                if (point.x > 0 && point.height <= _data[point.y][point.x-1].height) {
                    next_to_analyse.push({point.x-1, point.y, _data[point.y][point.x-1].height});
                }

                // Left
                if (point.x < x_range() - 1 && point.height <= _data[point.y][point.x+1].height) {
                    next_to_analyse.push({point.x+1, point.y, _data[point.y][point.x+1].height});
                }
            }

            // Adds the found basin into the list of basins
            basins.push_back(basin);
        }

        return basins;
    }
};



Heightmap parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<std::vector<int>> output;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while (std::getline(input_file, line)) {
        std::vector<int> line_numbers;
        for (auto num : line ) {
            line_numbers.push_back(num - '0');
        }
        output.push_back(line_numbers);
    }

    return Heightmap{output};
}



int main(int argc, char* argv[]) {
    // Common
    const auto heightmap = parse_inputs(argc, argv);

    std::vector<Point> lowest_points;

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Gets the low points from the heightmap
        std::vector<Point> low_points = heightmap.find_low_points();

        // Performs the calculations
        for (const auto & point : low_points) {
            result_1 += point.height + 1;
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   The sum of all low points is %d.\n", result_1);   



    // Part Two algorithms
    int result_2 = 1;

    float part_2_elapsed_time = time_block( [&](){
        // Gets all the basins
        std::vector<std::vector<Point>> basins = heightmap.find_basins();
        
        // Sorts the basins by its size (descending order)
        std::sort(basins.begin(), basins.end(),
            [](const std::vector<Point> &a, const std::vector<Point> &b){ return a.size() > b.size(); }
        );

        // Guarantees that at least 3 basins exist
        if (basins.size() < 3) { throw std::runtime_error("Something went wrong... The algorithm could not find at least 3 basins."); }

        // Performs the calculations
        for (int i = 0; i < 3; i++) { result_2 *= basins[i].size(); }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The size multiplication of the three largest basins is %d.\n", result_2);  

    return 0;
}
