
#include "utility.hpp"
#include <utility/StringUtils.hpp>

std::string generatePath()
{
    int pathLength = 1 + rand() % 4;
    std::string result;
    for (int i = 0; i < pathLength; i++) {
        if (!result.empty()) {
            result.append("/");
        }
        result.append(kvs::generateString(1));
    }
    return result;
}
