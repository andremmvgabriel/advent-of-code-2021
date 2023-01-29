#include <chrono>
#include <vector>
#include <string>

//
// Times the execution of a given function
//
template <typename Func>
float time_block(Func func)
{
    auto begin = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0;
}

//
// Splits a string into a vector of strings accordingly to a given delimiter
//
std::vector<std::string> string_split(std::string string, std::string delimiter) {
    std::vector<std::string> output;

    size_t index = string.find(delimiter);
    while (index != -1) {
        output.push_back(std::string(&string[0], &string[index]));
        string = std::string(&string[index+delimiter.size()]);
        index = string.find(delimiter);
    }

    output.push_back(string);

    return output;
}

//
// Enumerator
//
// template<typename Iterable>
// class enumerate
// {
// private:
//     Iterable& data;
//     std::size_t _size;
//     decltype(std::begin(_iter)) _begin;
//     const decltype(std::end(_iter)) _end;
// public:
//     enumerate() = delete;
//     enumerate(Iterable iter) {}
//     enumerate(Iterable iter) {}
// };

//
// Zipper
//