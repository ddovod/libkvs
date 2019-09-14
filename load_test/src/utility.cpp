
#include "utility.hpp"
#include <random>
#include <utility/RandomUtils.hpp>
#include <utility/StringUtils.hpp>

std::string generatePath()
{
    int pathLength = 1 + kvs::utility::random(4);
    std::string result;
    for (int i = 0; i < pathLength; i++) {
        if (!result.empty()) {
            result.append("/");
        }
        result.append(kvs::utility::generateString(1));
    }
    return result;
}
