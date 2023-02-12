#include <array>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <numeric>

#include "../common/utils.hpp"



#define INFINITE std::numeric_limits<int>::max()

template<typename T>
using Matrix = std::vector<std::vector<T>>;

struct Vertex {
    int x = 0, y = 0;

    bool operator<(Vertex const &v) const {
        return x < v.x || (x == v.x && y < v.y);
    }
};

std::vector<std::vector<uint8_t>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    Matrix<uint8_t> graph;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;
    while (std::getline(input_file, line)) {
        std::vector<uint8_t> line_nums;
        for (const auto & c : line) {
            line_nums.push_back(c - '0');
        }
        graph.push_back(line_nums);
    }

    return graph;
}

class DijkstraAlgorithm
{
private:
    struct Candidate {
        int weight;
        Vertex vertex;

        bool operator<(Candidate const &c) const {
            return weight > c.weight;
        }
    };
    
private:
    const Matrix<uint8_t> _grid;

public:
    DijkstraAlgorithm() = delete;
    DijkstraAlgorithm(const Matrix<uint8_t>& grid) : _grid(grid) {}

    Matrix<int> shortest_paths_from(const Vertex& start) const {
        // Control variable to store the best distance betweet start and all vertices
        Matrix<int> distances(_grid.size(), std::vector<int>(_grid[0].size(), INFINITE));

        // Control variable to track visited vertices
        Matrix<uint8_t> visited(_grid.size(), std::vector<uint8_t>(_grid[0].size(), 0));

        // Start vertex does not have weight and it is already visited
        distances[start.x][start.y] = 0;
        visited[start.x][start.y] = 1;

        // Increments for each vertex neighbour
        std::vector<Vertex> increments { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

        // List of best candidates to be analyzed (the candidate with lest weight goes first)
        std::priority_queue<Candidate> candidates; candidates.push({0, {start.x, start.y}});

        while (!candidates.empty()) {
            // Gets the best candidate
            const auto [w, v1] = candidates.top(); candidates.pop();

            // Checks all its neighbours
            for (const auto & i : increments) {
                // Ignore vertices outside the grid
                if (v1.x + i.x < 0) { continue; }
                if (v1.y + i.y < 0) { continue; }
                if (v1.x + i.x >= _grid[0].size()) { continue; }
                if (v1.y + i.y >= _grid.size()) { continue; }

                // Ignore the vertice if already visited
                if (visited[v1.x + i.x][v1.y + i.y]) { continue; }

                // Creates the neighbour vertex object
                Vertex v2 {v1.x + i.x, v1.y + i.y};

                // Checks if the neighbour distance can be reduced through the best candidate
                if (distances[v2.x][v2.y] > distances[v1.x][v1.y] + _grid[v2.x][v2.y]) {
                    distances[v2.x][v2.y] = distances[v1.x][v1.y] + _grid[v2.x][v2.y];
                    candidates.push({distances[v2.x][v2.y], v2});
                }
            }

            // Updates the current best candidate vertex to visited
            visited[v1.x][v1.y] = 1;
        }

        return distances;
    }
};

Matrix<uint8_t> increase_graph (const Matrix<uint8_t>& graph, int size_increase) {
    Matrix<uint8_t> new_graph = Matrix<uint8_t>(
        graph.size()*size_increase, std::vector<uint8_t>(graph[0].size()*size_increase, 0)
    );

    for (int y = 0 ; y < graph.size() ; y++) {
        for (int x = 0 ; x < graph[y].size() ; x++) {
            for (int i = 0 ; i < size_increase ; i++) {
                for (int j = 0 ; j < size_increase ; j++) {
                    int new_value = graph[y][x] + i + j;
                    if (new_value > 9) {
                        new_value = (new_value % 9);
                    }
                    new_graph[y+j*graph.size()][x+i*graph[0].size()] = new_value;
                }
            }
        }
    }

    return new_graph;
};



int main(int argc, char* argv[]) {
    // Common
    const auto GRAPH = parse_inputs(argc, argv);

    // Part One algorithms
    const Vertex START_POS_1 {0,0};
    const Vertex FINISH_POS_1 {static_cast<int>(GRAPH[0].size() - 1), static_cast<int>(GRAPH.size() - 1)};

    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        DijkstraAlgorithm dijkstra(GRAPH);
        Matrix<int> moves = dijkstra.shortest_paths_from(START_POS_1);
        
        // Gets the best path weight from start to finish position
        result_1 = moves[FINISH_POS_1.x][FINISH_POS_1.y];
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   The lowest risk path has total of %d risk.\n", result_1);



    // Part Two algorithms
    const int SIZE_INCREASE = 5;
    const Vertex START_POS_2 {0,0};
    const Vertex FINISH_POS_2 {static_cast<int>(GRAPH[0].size()*SIZE_INCREASE - 1), static_cast<int>(GRAPH.size()*SIZE_INCREASE - 1)};

    int result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        std::vector<std::vector<uint8_t>> new_graph = increase_graph(GRAPH, SIZE_INCREASE);

        DijkstraAlgorithm dijkstra(new_graph);
        Matrix<int> moves = dijkstra.shortest_paths_from(START_POS_2);

        // Gets the best path weight from start to finish position
        result_2 = moves[FINISH_POS_2.x][FINISH_POS_2.y];
    });

    

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The lowest risk path has total of %d risk.\n", result_2);   

    return 0;
}
