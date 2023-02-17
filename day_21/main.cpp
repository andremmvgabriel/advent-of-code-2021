#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <array>
#include <map>
#include <unordered_map>

#include "../common/utils.hpp"

#include <functional>

std::tuple<int, int> parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::string line;

    // Player 1
    std::getline(input_file, line);
    std::vector<std::string> player_1_parts = string_split(line, ": ");

    // Player 2
    std::getline(input_file, line);
    std::vector<std::string> player_2_parts = string_split(line, ": ");

    return {std::stoi(player_1_parts[1]), std::stoi(player_2_parts[1])};
}

struct State
{
    std::array<int, 2> scores {0, 0};
    std::array<int, 2> places {0, 0};

    bool operator==(State const &s) const {
        return scores[0] == s.scores[0] && scores[1] == s.scores[1] && places[0] == s.places[0] && places[1] == s.places[1];
    }

    struct Hasher
    {
        std::size_t operator()(const State& s) const
        {
            return std::hash<int>()( (s.places[0] << 24) | (s.places[1] << 16) | (s.scores[0] << 8) | (s.scores[1] << 0) );
        }
    };
};



int main(int argc, char* argv[]) {
    // Common
    const auto [PLAYER_1, PLAYER_2] = parse_inputs(argc, argv);

    // Part One algorithms
    int WIN_SCORE_1 = 1000;

    int dice_rolls = 0;
    std::array<int, 2> player_1 { 0, PLAYER_1 - 1 };
    std::array<int, 2> player_2 { 0, PLAYER_2 - 1 };

    float part_1_elapsed_time = time_block( [&](){
        // Variable to symbolize which player's turn is
        int player_turn = 0;

        // The game will be played as long as no player reaches 1000
        while (player_1[0] < WIN_SCORE_1 && player_2[0] < WIN_SCORE_1) {
            // Dice rolled 3 times
            dice_rolls += 3;

            // Player 1: Update position & Increase score
            if (player_turn % 2 == 0) {
                player_1[1] = (player_1[1] + (dice_rolls * 3 - 3)) % 10;
                player_1[0] += player_1[1] + 1;
            }
            // Player 2: Update position & Increase score
            else {
                player_2[1] = (player_2[1] + (dice_rolls * 3 - 3)) % 10;
                player_2[0] += player_2[1] + 1;
            }
            
            // Next player
            player_turn++;
        }
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("    The game has finished! The final game score is: %d\n", dice_rolls * (player_1[0] >= 1000 ? player_2[0] : player_1[0]));
    printf("    > Player 1 score: %d %s\n", player_1[0], player_1[0] >= 1000 ? "(WINNER)" : " ");
    printf("    > Player 2 score: %d %s\n", player_2[0], player_2[0] >= 1000 ? "(WINNER)" : " ");
    printf("    > Total dice rolls: %d\n", dice_rolls);



    // Part Two algorithms
    int WIN_SCORE_2 = 21;

    std::array<uint64_t, 2> wins {0, 0};

    float part_2_elapsed_time = time_block( [&](){
        // Variable that maps the possible rolls outputs after 3 rolls
        std::unordered_map<int, int> possible_rolls;
        for (int r1 = 1; r1 <= 3; r1++) {
            for (int r2 = 1; r2 <= 3; r2++) {
                for (int r3 = 1; r3 <= 3; r3++) {
                    possible_rolls[r1+r2+r3]++;
                }
            }
        }

        // Initial player's scores and places
        std::array<int, 2> initial_scores {0, 0};
        std::array<int, 2> initial_places {PLAYER_1-1, PLAYER_2-1};

        // Map that will contain all the universes possibilities and their amounts
        std::unordered_map<State, uint64_t, State::Hasher> multiverses; multiverses[{initial_scores, initial_places}] = 1;

        // Variable to symbolize which player's turn is
        int player_turn = 0;     

        // The game will be played as long as there are universes without winners
        while (!multiverses.empty()) {
            // Variable that will contain the outcome universes after a player's turn
            std::unordered_map<State, uint64_t, State::Hasher> new_multiverses;

            // Goes through all the current universes
            for (auto [state, amount] : multiverses) {
                // Iterates through all the rolls possibilities
                for (const auto & [move, f] : possible_rolls) {
                    // Caches the current state scores and places
                    std::array<int, 2> scores = state.scores;
                    std::array<int, 2> places = state.places;

                    // Increases the player's place and score
                    places[player_turn%2] = (places[player_turn%2] + move) % 10;
                    scores[player_turn%2] += places[player_turn%2] + 1;

                    // Checks if the player that played won or not
                    // > Increases the player's wins if score was achieved
                    // > Increases new universes amount if score was not achieved
                    if (scores[player_turn%2] >= WIN_SCORE_2) {
                        wins[player_turn%2] += amount * f;
                    } else {
                        new_multiverses[{scores, places}] += amount * f;
                    }
                }
            }

            // Replaces the multiverses with the new generated ones
            multiverses = new_multiverses;

            // Next player's turn
            player_turn++;
        }
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("    All the universes got a winner! Player %d had the most wins with %lu wins.\n", wins[0] > wins[1] ? 1 : 2, wins[0] > wins[1] ? wins[0] : wins[1]);
    printf("    > Player 1 wins: %lu %s\n", wins[0], wins[0] > wins[1] ? "(WINNER)" : " ");
    printf("    > Player 2 wins: %lu %s\n", wins[1], wins[1] > wins[0] ? "(WINNER)" : " ");

    return 0;
}
