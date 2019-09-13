
#include <iostream>
#include <random>
#include "utility.hpp"

extern std::mt19937 g_rand;

template <typename T>
bool parseParam(argh::parser& args, const std::string& paramName, T& value)
{
    if (auto param = args(paramName)) {
        param >> value;
        return true;
    }

    std::cout << "Missing '" << paramName << "'" << std::endl;
    return false;
}

template <typename T>
typename T::value_type getRandomElement(const T& container)
{
    auto i = g_rand() % container.size();
    auto it = container.begin();
    while (i > 0) {
        it++;
        i--;
    }
    return *it;
}
