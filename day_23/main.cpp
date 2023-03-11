#include <cstdint>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <cmath>
#include <queue>
#include <unordered_map>

#include "../common/utils.hpp"



std::tuple<std::vector<uint8_t>, std::vector<std::vector<uint8_t>>> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::vector<uint8_t> hallway;
    std::vector<std::vector<uint8_t>> side_rooms;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    // First line never has content
    std::string line;
    std::getline(input_file, line);

    // Second line should include the hallway
    std::getline(input_file, line);
    // hallway.resize(line.size() - 2, 0xFF);
    hallway.resize(line.size() - 2, (line.size() - 4) / 2);
    side_rooms.resize((hallway.size() - 2) / 2);

    // Filling the side rooms
    while (std::getline(input_file, line)) {
        for (int i = 0 ; i < side_rooms.size() ; i++) {
            if (line[i*2 + 3] == '#') continue;
            side_rooms[i].push_back( line[i*2 + 3] == '.' ? side_rooms.size() : line[i*2 + 3] - 'A' );
        }
    }

    return {hallway, side_rooms};
}


class State
{
public:
    uint32_t score = 0;
    std::vector<uint8_t> hallway;
    std::vector<std::vector<uint8_t>> side_rooms;

public:
    State() {}
    State(std::vector<uint8_t> hallway, std::vector<std::vector<uint8_t>> side_rooms) : hallway(hallway), side_rooms(side_rooms) {}

    bool operator<(State const &s) const {
        return score > s.score;
    }

    bool operator==(State const &s) const {
        bool equal = score == s.score;

        for (int i = 0 ; i < s.hallway.size() ; i++) {
            equal &= hallway[i] == s.hallway[i]; 
        }

        for (int i = 0 ; i < s.side_rooms.size() ; i++) {
            for (int j = 0 ; j < s.side_rooms[i].size() ; j++) {
                equal &= side_rooms[i][j] == s.side_rooms[i][j]; 
            }
        }

        return equal;
    }

    struct Hasher
    {
        std::size_t operator()(const State& s) const
        {
            std::size_t seed = s.hallway.size() + s.side_rooms.size() + 1;
            for(auto& i : s.hallway) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            for (auto &sd : s.side_rooms) {
                int i = 0;
                for(auto& v : sd) i = i << 8 | v;
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            seed ^= s.score + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
};

class Organizer
{
private:
    // Queue to place the generated states + Unordered map
    // > Priority -> Organizes the states based on the < operator. It will order them in ascending order based on the total score.
    // > Unordered map -> Will track the state uniqueness. We dont want to process new states seen previously.
    std::priority_queue<State> _states;
    std::unordered_map<State, int, State::Hasher> _states_uniqueness;

private:
    void _clean() {
        _states_uniqueness.clear();
        while (!_states.empty()) _states.pop();
    }

    bool _is_state_complete(const State& state) {
        for (int s = 0; s < state.side_rooms.size(); s++) {
            for (auto val : state.side_rooms[s]) {
                if (val != s) return false;
            }
        }

        return true;
    }

    bool _can_move_in_hallway(const State& state, int from, int to) {
        int initial = (from < to) ? (from + 1) : to;
        int end     = (from < to) ? to : (from - 1);

        for (int i = initial; i <= end ; i++) {
            if (state.hallway[i] != state.side_rooms.size()) return false;
        }
       
       return true;
    }

    bool _can_move_into_sideroom(const State& state, uint8_t room, int& position_to_move) {
        // We check the room from the farthest to closest position to the hallway
        for (int i = state.side_rooms[room].size() - 1 ; i >= 0 ; i--) {
            // Empty position!
            if (state.side_rooms[room][i] == state.side_rooms.size()) {
                position_to_move = i;
                return true;
            }
            // Cannot move into if there are any amphipods that are not in the correct room
            else if (state.side_rooms[room][i] != room) break;
        }
        
        return false;
    }

    void _move_hallway_amphipodes(const State& state) {
        // Checks all the hallway positions, from left to right
        for (int h = 0; h < state.hallway.size(); h++) {
            // Useful vars
            uint8_t val = state.hallway[h];
            int target_pos = 2 + 2 * val;

            // Skip the current hallway position it does not contain an amphipod
            if (val == state.side_rooms.size()) continue;

            // Skips the current hallway position if the amphipod does not have a clear path to the entrance of its target side room
            if (!_can_move_in_hallway(state, h, target_pos)) continue;

            // Skips the current hallway position if the amphipod cannot enter in its target side room
            int move_to;
            if (!_can_move_into_sideroom(state, val, move_to)) continue;

            // Calculates the total distance the amphipod will move
            int distance = std::abs( h - target_pos) + 1 + move_to;

            // Creates a new state of the diagram
            // > Copy from the current state
            // > Changes the hallway position (inital) to an "empty" value
            // > Changes the side room position (final) to the amphipod value
            // > Updates the total score in the state due to the amphipod movement
            State new_state = state;
            new_state.hallway[h] = new_state.side_rooms.size();
            new_state.side_rooms[val][move_to] = val;
            new_state.score += distance * std::pow(10, val);

            // Adds the new state into the queue if it was never seen
            if(_states_uniqueness[new_state]++ <= 1) _states.push(new_state);
        }
    }

    bool _is_side_room_complete(const State& state, uint8_t room, int& move_from) {
        // Makes sure the given move_from is set as -1
        move_from = -1;

        // Checks all the side room elements, from farthest to closest to the hallway
        for (int i = state.side_rooms[room].size() - 1; i >= 0; i--) {
            if (state.side_rooms[room][i] == room && move_from != -1) move_from = i;
            else if (state.side_rooms[room][i] == room) continue;
            else if (state.side_rooms[room][i] == state.side_rooms.size()) break;
            else move_from = i;
        }

        return move_from == -1;
    }

    void _leave_side_room_to_left(const State& state, int room, int pos) {
        int initial = 2 + 2 * room - 1;

        for (int i = initial; i >= 0 ; i--) {
            // Skip forbidden places to move into
            if (i % 2 == 0 && i != 0) continue;

            // Skip if a place if already occupied
            // > No need to keep searching since all other positions will be blocked
            if (state.hallway[i] != state.side_rooms.size()) break;

            // Calculates the total distance the amphipod will move
            int distance = (2 + 2 * room) - i + 1 + pos;

            // Creates a new state of the diagram
            // > Copy from the current state
            // > Changes the hallway position (inital) to an "empty" value
            // > Changes the side room position (final) to the amphipod value
            // > Updates the total score in the state due to the amphipod movement
            State new_state = state;
            new_state.hallway[i] = new_state.side_rooms[room][pos];
            new_state.side_rooms[room][pos] = new_state.side_rooms.size();
            new_state.score += distance * std::pow(10, new_state.hallway[i]);

            // Adds the new state into the queue if it was never seen
            if(_states_uniqueness[new_state]++ <= 1) _states.push(new_state);
        }
    }

    void _leave_side_room_to_right(const State& state, int room, int pos) {
        int initial = 2 + 2 * room + 1;

        for (int i = initial; i < state.hallway.size() ; i++) {
            // Skip forbidden places to move into
            if (i % 2 == 0 && i != state.hallway.size() - 1) continue;

            // Skip if a place if already occupied
            // > No need to keep searching since all other positions will be blocked
            if (state.hallway[i] != state.side_rooms.size()) break;

            // Calculates the total distance the amphipod will move
            int distance = i - (2 + 2 * room) + 1 + pos;

            // Creates a new state of the diagram
            // > Copy from the current state
            // > Changes the hallway position (inital) to an "empty" value
            // > Changes the side room position (final) to the amphipod value
            // > Updates the total score in the state due to the amphipod movement
            State new_state = state;
            new_state.hallway[i] = new_state.side_rooms[room][pos];
            new_state.side_rooms[room][pos] = new_state.side_rooms.size();
            new_state.score += distance * std::pow(10, new_state.hallway[i]);

            // Adds the new state into the queue if it was never seen
            if(_states_uniqueness[new_state]++ <= 1) _states.push(new_state);
        }
    }

    void _move_side_room_amphipodes(const State& state) {
        // Checks all the side rooms, from left to right
        for (int s = 0; s < state.side_rooms.size(); s++) {
            // Skips the current side room if it already contains its correct amphipods
            int to_move;
            if (_is_side_room_complete(state, s, to_move)) continue;

            // Checks if the incorrect amphipod can move to the left
            _leave_side_room_to_left(state, s, to_move);

            // Checks if the incorrect amphipod can move to the right
            _leave_side_room_to_right(state, s, to_move);
        }
    }

public:
    int find_optimal(const std::vector<uint8_t> hallway, const std::vector<std::vector<uint8_t>>& side_rooms) {
        // Outputs
        int best_score = std::numeric_limits<int>::max();

        // Creates the initial state
        State initial_state {hallway, side_rooms};
        _states.push(initial_state);

        // Actual organization
        // > It will run as long as there are states to process
        // > It will run as long as there are states that have a lower score than the current best one
        while(!_states.empty()) {
            // Gets the next state from the queue
            State state = _states.top(); _states.pop();

            // Stops if no better score is possible
            if (best_score < state.score) break;

            // Updates the best score if the state is complete
            if (_is_state_complete(state)) {
                best_score = state.score;
                continue;
            }

            // Moves the amphipodes from the hallway
            _move_hallway_amphipodes(state);

            // Moves the incorrect amphipodes from the side rooms
            _move_side_room_amphipodes(state);
        }

        // Cleans the whole simulation
        _clean();

        return best_score;
    }
};



int main(int argc, char* argv[]) {
    // Common
    const auto [HALLWAY, SIDE_ROOMS] = parse_inputs(argc, argv);

    // Part One algorithms
    int result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        Organizer org;
        result_1 = org.find_optimal(HALLWAY, SIDE_ROOMS);
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   The lowest energy needed to organize the amphipods is %d.\n", result_1);   



    // Part Two algorithms
    int result_2 = 0;

    std::vector<std::vector<uint8_t>> side_rooms_ext {
        {3, 3}, // D D
        {2, 1}, // C B
        {1, 0}, // B A
        {0, 2}  // A C
    };

    float part_2_elapsed_time = time_block( [&](){
        auto side_rooms = SIDE_ROOMS;

        for (int i = 0 ; i < side_rooms.size() ; i++) {
            side_rooms[i].insert(side_rooms[i].begin()+1, side_rooms_ext[i].begin(), side_rooms_ext[i].end());
        }

        Organizer org;
        result_2 = org.find_optimal(HALLWAY, side_rooms);
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   The lowest energy needed to organize the amphipods is %d.\n", result_2);  

    return 0;
}
