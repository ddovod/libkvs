
#pragma once

#include <string>
#include <vector>

namespace kvs
{
    inline bool startsWith(const std::string& value, const std::string& starting)
    {
        if (starting.size() > value.size()) {
            return false;
        }
        return std::equal(starting.begin(), starting.end(), value.begin());
    }

    inline bool endsWith(const std::string& value, const std::string& ending)
    {
        if (ending.size() > value.size()) {
            return false;
        }
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }

    inline std::vector<std::string> split(const std::string& str, const std::string& delim = " ")
    {
        std::vector<std::string> res;
        std::size_t current, previous = 0;
        current = str.find(delim);
        while (current != std::string::npos) {
            res.push_back(str.substr(previous, current - previous));
            previous = current + delim.length();
            current = str.find(delim, previous);
        }
        res.push_back(str.substr(previous, current - previous));
        return res;
    }

    std::string generateString(size_t length = 16);
}
