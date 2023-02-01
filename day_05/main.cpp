#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <map>

#include "../common/utils.hpp"


struct Point {
    int x = 0, y = 0;

    // Operator that allows the structure to be inserted in a map/set as key
    bool operator<(Point const &p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
};

class LineSegment
{
private:
    std::vector<Point> _points;

public:
    LineSegment() = delete;
    LineSegment(const std::string &raw_point1, const std::string &raw_point2) {
        // Separates each point by its x and y values
        std::vector<std::string> point1_xy = string_split(raw_point1, ",");
        std::vector<std::string> point2_xy = string_split(raw_point2, ",");

        // Creates the initial and final points
        Point p1 = { std::stoi(point1_xy[0]), std::stoi(point1_xy[1]) };
        Point p2 = { std::stoi(point2_xy[0]), std::stoi(point2_xy[1]) };

        // Calculates the distance between the points in each axis
        int delta_x = p2.x - p1.x; int mod_delta_x = std::abs(delta_x);
        int delta_y = p2.y - p1.y; int mod_delta_y = std::abs(delta_y);

        // Calculates how many points the line will have
        int n_points = std::max(mod_delta_x, mod_delta_y);

        // Creates all the line points
        for (int i = 0; i < n_points + 1; i++) {
            _points.push_back({
                p1.x + i * delta_x/n_points,
                p1.y + i * delta_y/n_points
            });
        }
    }

    std::vector<Point> get_points() const { return _points; }

    bool is_point() { return _points.size() == 1; }
    bool is_vertical() const { return _points[0].x == _points[1].x; }
    bool is_horizontal() const { return _points[0].y == _points[1].y; }
};


std::vector<LineSegment> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<LineSegment> line_segments;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // Read input data
    std::string line;
    while (std::getline(input_file, line)) {
        std::vector<std::string> points = string_split(line, " -> ");
        line_segments.push_back(LineSegment(points[0], points[1]));
    }

    return line_segments;
}



int main(int argc, char* argv[]) {
    // Common
    const auto LINES = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        // Map that will store all the points of each line and count them
        std::map<Point, int> points_map;

        for (const auto & line : LINES) {
            // We dont care about lines other than vertical and horizontal
            if (!line.is_vertical() && !line.is_horizontal()) continue;

            // Adds each point to the map
            // > It is safe to do this due to map operator [] specification
            //   > If no key is present, it creates it with a default value invoke
            for (auto point : line.get_points()) { ++points_map[point]; }
        }

        for (const auto & [point, count] : points_map) {
            if (count > 1) result_1++;
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   There are %d where at least two lines overlap.\n", result_1);



    // Part Two algorithms
    int result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        // Map that will store all the points of each line and count them
        std::map<Point, int> points_map;

        for (const auto & line : LINES) {
            // Adds each point to the map
            // > It is safe to do this due to map operator [] specification
            //   > If no key is present, it creates it with a default value invoke
            for (auto point : line.get_points()) { ++points_map[point]; }
        }

        for (const auto & [point, count] : points_map) {
            if (count > 1) result_2++;
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   There are %d where at least two lines overlap.\n", result_2);

    return 0;
}
