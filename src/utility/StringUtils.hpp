
#pragma once

#include <string>
#include <vector>

namespace kvs
{
    bool startsWith(const std::string& value, const std::string& starting);
    bool endsWith(const std::string& value, const std::string& ending);
    std::vector<std::string> split(const std::string& str, const std::string& delim = " ");
    std::vector<std::string> split(const std::string& original, char separator);
    std::string generateString(size_t length = 16);
}
