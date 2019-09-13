
#include "utility.hpp"
#include <random>
#include <utility/StringUtils.hpp>

std::mt19937 g_rand;

std::string generatePath()
{
    int pathLength = 1 + g_rand() % 4;
    std::string result;
    for (int i = 0; i < pathLength; i++) {
        if (!result.empty()) {
            result.append("/");
        }
        result.append(kvs::generateString(1));
    }
    return result;
}
