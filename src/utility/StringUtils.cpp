
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

namespace kvs
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

    bool endsWith(const std::string& value, const std::string& ending)
    {
        if (ending.size() > value.size()) {
            return false;
        }
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }

    std::vector<std::string> split(const std::string& str, const std::string& delim)
    {
        std::vector<std::string> res;
        std::size_t current = 0;
        std::size_t previous = 0;
        current = str.find(delim);
        while (current != std::string::npos) {
            res.push_back(str.substr(previous, current - previous));
            previous = current + delim.length();
            current = str.find(delim, previous);
        }
        res.push_back(str.substr(previous, current - previous));
        return res;
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
