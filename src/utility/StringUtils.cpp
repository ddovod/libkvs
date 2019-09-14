
#include "StringUtils.hpp"
#include <algorithm>
#include "utility/RandomUtils.hpp"

namespace
{
    const char chars[] =  // NOLINT
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    size_t stringLength = sizeof(chars) - 1;

    char getRandomChar() { return chars[kvs::utility::random(stringLength)]; }  // NOLINT
}

namespace kvs::utility
{
    std::string generateString(size_t length)
    {
        std::string res;
        res.reserve(length);
        for (size_t i = 0; i < length; i++) {
            res += ::getRandomChar();
        }
        return res;
    }

    bool startsWith(const std::string& value, const std::string& starting)
    {
        if (starting.size() > value.size()) {
            return false;
        }
        return std::equal(starting.begin(), starting.end(), value.begin());
    }

    std::vector<std::string> split(const std::string& original, char separator)
    {
        std::vector<std::string> results;
        std::string::const_iterator start = original.begin();
        std::string::const_iterator end = original.end();
        std::string::const_iterator next = std::find(start, end, separator);
        while (next != end) {
            results.emplace_back(start, next);
            start = next + 1;
            next = std::find(start, end, separator);
        }
        results.emplace_back(start, next);
        return results;
    }
}
