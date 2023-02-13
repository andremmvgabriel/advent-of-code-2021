#include <fstream>
#include <numeric>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

#include "../common/utils.hpp"



class Packet
{
protected:
    uint8_t _version;
    uint8_t _id;

public:
    Packet() = delete;
    Packet(uint8_t version, uint8_t id) : _version(version), _id(id) {}

    virtual ~Packet() {}

    virtual uint64_t get_result() { return 0x00; }

    uint8_t get_version() { return _version; }
    virtual uint64_t sum_packets_versions() { return _version; }
};

class LiteralPacket : public Packet
{
private:
    std::vector<uint8_t> _literals;

public:
    LiteralPacket() = delete;
    LiteralPacket(uint8_t version, uint8_t id, const std::vector<uint8_t>& literals) : Packet(version, id), _literals(literals) {}

    virtual uint64_t get_result() override {
        uint64_t literal = 0x00;

        // Every literal only occupies 4 bits max
        // > Shifts 4 bits to the left
        // > Adds the new 4 bits from the next literal 
        for (const auto & sub : _literals) { literal = literal << 4 | sub; }

        return literal;
    }
};

class OperatorPacket : public Packet
{
private:
    std::vector<Packet*> _sub_packets;

private:
    uint64_t _operate_sum() {
        // Sums all the values from each one of its sub packets
        uint64_t result = 0;
        for (auto packet : _sub_packets) { result += packet->get_result(); }
        return result;
    }

    uint64_t _operate_prod() {
        // Multiplies all the values from each one of its sub packets
        uint64_t result = 1;
        for (auto packet : _sub_packets) result *= packet->get_result();
        return result;
    }

    uint64_t _operate_min() {
        // Gets the minimum value from each of its sub packets
        uint64_t result = -1;
        for (auto packet : _sub_packets) {
            uint64_t value = packet->get_result();
            if (value < result) result = value;
        }
        return result;
    }

    uint64_t _operate_max() {
        // Gets the maximum value from each of its sub packets
        uint64_t result = 0;
        for (auto packet : _sub_packets) {
            uint64_t value = packet->get_result();
            if (value > result) result = value;
        }
        return result;
    }

    uint64_t _operate_greater() {
        // Checks if the first sub packet has a greater value value
        return _sub_packets.at(0)->get_result() > _sub_packets.at(1)->get_result();
    }

    uint64_t _operate_smaller() {
        // Checks if the first sub packet has a smaller value value
        return _sub_packets.at(0)->get_result() < _sub_packets.at(1)->get_result();
    }

    uint64_t _operate_equal() {
        // Checks if the both sub packets has the same value
        return _sub_packets.at(0)->get_result() == _sub_packets.at(1)->get_result();
    }

public:
    OperatorPacket() = delete;
    OperatorPacket(uint8_t version, uint8_t id, const std::vector<Packet*>& packets) : Packet(version, id), _sub_packets(packets) {}

    ~OperatorPacket() {
        for (int i = _sub_packets.size() - 1; i >= 0 ; i--) {
            if (_sub_packets[i]) delete _sub_packets[i];
        }
    }

    uint64_t get_result() override {
        uint64_t result = 0x00;

        switch (_id) {
            case 0x00: result = _operate_sum(); break;
            case 0x01: result = _operate_prod(); break;
            case 0x02: result = _operate_min(); break;
            case 0x03: result = _operate_max(); break;
            case 0x05: result = _operate_greater(); break;
            case 0x06: result = _operate_smaller(); break;
            case 0x07: result = _operate_equal(); break;
            default: break;
        }

        return result;
    }

    virtual uint64_t sum_packets_versions() override {
        uint64_t value = get_version();

        for (auto packet : _sub_packets) {
            value += packet->sum_packets_versions();
        }

        return value;
    }
};

class PacketsDecoder
{
private:
    const std::unordered_map<char, uint8_t> _HEX_LUT { {'0', 0x00}, {'1', 0x01}, {'2', 0x02}, {'3', 0x03}, {'4', 0x04}, {'5', 0x05}, {'6', 0x06}, {'7', 0x07}, {'8', 0x08}, {'9', 0x09}, {'A', 0x0A}, {'B', 0x0B}, {'C', 0x0C}, {'D', 0x0D}, {'E', 0x0E}, {'F', 0x0F} };

private:
    std::vector<uint8_t> _bytes;

public:
    const std::string message;

    Packet* _packet = nullptr;

private:
    void _message_to_bytes() {
        for (int i = 0; i < message.size(); i+=2) {
            uint8_t value = (_HEX_LUT.at(message[i]) << 4) | (_HEX_LUT.at(message[i+1]));
            _bytes.push_back(value);
        }
    }

    uint8_t _get_bit(const uint64_t& bit) const {
        // Gets a specific bit from the whole bit stream
        // > _bytes[bit / 8] : This gets the byte on which the bit is contained
        // > (bit % 8) : This gets the bit position on which the bit is within the byte
        // > (7-(bit % 8)) : Reverts the bit position
        // > _bytes[bit / 8] >> (7-(bit % 8)) & 1 : This gets only its value. 0 or 1.
        return _bytes[bit / 8] >> (7-(bit % 8)) & 1;
    }

    template<typename T>
    T _read_bits(uint64_t& bit, T length) {
        // Do not let more shift than the number of actual available bits
        if (sizeof(T) * 8 < length) return 0;

        T value = 0x00;

        // Shifting
        for (T i = 0; i < length; i++) {
            value = (value << 1) | _get_bit(bit);
            bit++;
        }

        return value;
    }

    Packet* _read_literal_packet(uint64_t& bit, uint8_t version, uint8_t id) {
        // List of literals within the package
        std::vector<uint8_t> literals;

        while (true) {
            // Reads the control bit
            uint8_t control_bit = _read_bits(bit, 1);

            // Reads and caches the literal
            literals.push_back(_read_bits(bit, 4));

            // Last literal was read
            if (control_bit == 0x00) break;
        }

        return new LiteralPacket(version, id, literals);
    }

    Packet* _read_operator_packet(uint64_t& bit, uint8_t version, uint8_t id) {
        // List of packages that the operator packet contains
        std::vector<Packet*> packets;

        // Reads the control bit
        uint8_t control_bit = _read_bits(bit, 1);
        
        // We found an operator packet that contains info about the full size of the packet
        if (control_bit == 0x00) {
            // Reads the size of the packet
            uint16_t packet_full_size = _read_bits(bit, 15);

            // Caches the current bit index
            uint64_t start_bit = bit;
            
            // Searches for packets in all the bits defined by the packet size
            while (bit - start_bit < packet_full_size) {
                packets.push_back(_find_package(bit));
            }
        }
        // We found an operator packet that contains info about the number of sub packet contained within
        else {
            // Reads the number of sub packets contained within the packet
            uint16_t number_sub_packets = _read_bits(bit, 11);

            // Reads all the packets
            for (uint16_t j = 0; j < number_sub_packets; j++) {
                packets.push_back(_find_package(bit));
            }
        }

        return new OperatorPacket(version, id, packets);
    }

    Packet* _find_package(uint64_t& bit) {
        // Read the packet Version & ID
        uint8_t version = _read_bits(bit, 3);
        uint8_t id = _read_bits(bit, 3);

        // We found a Literal Packet!
        if (id == 0x04) {
            return _read_literal_packet(bit, version, id);
        }
        // We found an Operator Packet!
        else {
            return _read_operator_packet(bit, version, id);
        }
    }

public:
    PacketsDecoder() = delete;
    PacketsDecoder(const std::string& message) : message(message) {
        _message_to_bytes();
        decode();
    }

    ~PacketsDecoder() {
        if (_packet) { delete _packet; }
    }

    void decode() {
        uint64_t counter = 0x00;
        _packet = _find_package(counter);
    }

    uint64_t sum_packets_versions() { return _packet->sum_packets_versions(); }

    uint64_t get_result() { return _packet->get_result(); }
};

std::string parse_inputs(int argc, char* argv[]) {
    // Makes sure a input file is specified
    if (argc <= 1) throw std::invalid_argument("You have to specify an input text file.");

    // Actual outputs
    std::string line;

    // Open input file
    std::ifstream input_file(std::string(argv[1]), std::ios::in);

    std::getline(input_file, line);

    return line;
}



int main(int argc, char* argv[]) {
    // Common
    const auto RAW_MESSAGE = parse_inputs(argc, argv);

    // Part One algorithms
    uint64_t result_1 = 0;

    float part_1_elapsed_time = time_block( [&](){
        PacketsDecoder decoder(RAW_MESSAGE);
        result_1 = decoder.sum_packets_versions();
    });
    
    // Part One visualization
    printf("\n> Part One < (%f seconds)\n", part_1_elapsed_time);
    printf("   The sum of all packet versions is %lu.\n", result_1);



    // Part Two algorithms
    uint64_t result_2 = 0;

    float part_2_elapsed_time = time_block( [&](){
        PacketsDecoder decoder(RAW_MESSAGE);
        result_2 = decoder.get_result();
    });

    // Part Two visualization
    printf("\n> Part Two < (%f seconds)\n", part_2_elapsed_time);
    printf("   After decoding the message we get %lu.\n", result_2);

    return 0;
}
