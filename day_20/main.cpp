#include <cstdint>
#include <fstream>
#include <limits>
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>

#include "../common/utils.hpp"


class Image
{
private:
    using Row = std::vector<uint8_t>;
    using Matrix = std::vector<Row>;

private:
    Matrix _data;
    uint8_t infinite_value = 0;

    void _parse_raw_image(const std::vector<std::string> &raw_image) {
        for (const auto & line : raw_image) {
            Row row;
            for (const auto & c : line) {
                row.push_back(c == '#' ? 1 : 0);
            }
            _data.push_back(row);
        }
    }

    uint16_t _analyse_kernel_at(int h, int w) {
        // Kernel window is always 3 by 3
        uint16_t place = 0;

        // Filling the kernel value (line by line)
        // wh -> window height
        // ww -> window width
        for (int wh = -2; wh < 1; wh++) {
            for (int ww = -2; ww < 1; ww++) {
                // Always shift left the place by one
                place <<= 1;

                // If the current kernel position is out of the image bounds
                if (h + wh < 0 || h + wh >= height()) { place |= infinite_value; continue; }
                if (w + ww < 0 || w + ww >= width() ) { place |= infinite_value; continue; }

                // Adds the image pixel value to the kernel value
                place |= _data[h+wh][w+ww];
            }
        }

        return place;
    }

public:
    Image() = delete;
    Image(const std::vector<std::string> &raw_image) {
        _parse_raw_image(raw_image);
    }

    void enhance(const std::string& algorithm) {
        // Creates the image matrix that will be generated
        // > Should have the original image size with 1 pixel margin
        Matrix _enhanced_data(height() + 2, Row(width() + 2, 0));

        for (int h = 0 ; h < height() + 2 ; h++) {
            for (int w = 0 ; w < width() + 2 ; w++) {
                uint16_t place = _analyse_kernel_at(h, w);
                _enhanced_data[h][w] = algorithm[place] == '#' ? 1 : 0;
            }
        }

        // Replaces the original image with the enhanced one
        _data = _enhanced_data;

        // Handles the pixels value in the infinite
        if (infinite_value) { if (algorithm[511] == '.') infinite_value = 0; }
        else                { if (algorithm[ 0 ] == '#') infinite_value = 1; }
    }

    int count_on_pixels() {
        // If the pixels in the infinite are ON, the returned value should also be INF
        if (infinite_value) { return std::numeric_limits<int>::max(); }

        // Counts the ON pixels
        int counter = 0;
        for (const auto & row : _data) {
            for (const auto & val : row) {
                counter += val;
            }
        }
        return counter;
    }

    int count_off_pixels() {
        return height()*width() - count_on_pixels();
    }

    uint64_t height() const { return _data.size(); }
    uint64_t width() const { return height() ? _data[0].size() : 0; }
};


std::tuple<std::string, std::vector<std::string>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::string enhancer;
    std::vector<std::string> image;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // Reads the enhancement algorithm
    std::getline(input_file, enhancer);

    // Ignore the empty line
    std::string line;
    std::getline(input_file, line);

    while (std::getline(input_file, line)) {
        image.push_back(line);
    }

    return {enhancer, image};
}



int main(int argc, char* argv[]) {
    // Common
    const auto & [ENHANCER, IMAGE] = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;
    const int enhancement_times_1 = 2;

    float part_1_elapsed_time = time_block( [&](){
        Image image(IMAGE);

        for (int i = 0 ; i < enhancement_times_1 ; i++) {
            image.enhance(ENHANCER);
        }

        result_1 = image.count_on_pixels();
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   After enhancing the image %d times, the image contains %d pixels ON.\n", enhancement_times_1, result_1);   



    // Part Two algorithms
    int result_2 = 0;
    const int enhancement_times_2 = 50;

    float part_2_elapsed_time = time_block( [&](){
        Image image(IMAGE);

        for (int i = 0 ; i < enhancement_times_2 ; i++) {
            image.enhance(ENHANCER);
        }

        result_2 = image.count_on_pixels();
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   After enhancing the image %d times, the image contains %d pixels ON.\n", enhancement_times_2, result_2);

    return 0;
}
