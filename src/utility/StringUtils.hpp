
#pragma once

#include <string>
#include <vector>

namespace kvs::utility
{
    /** Checks if `value` string has a `starting` prefix. */
    bool startsWith(const std::string& value, const std::string& starting);

    /** Splits `original` string by `separator` char delimiter. */
    std::vector<std::string> split(const std::string& original, char separator);

    /** Generates a random alpha-numeric string of the given length. */
    std::string generateString(size_t length = 16);
}
