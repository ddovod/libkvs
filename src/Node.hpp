
#pragma once

#include <string>

namespace kvs
{
    struct Node
    {
        std::string primaryFilepath;
        std::string overflowFilepath;
    };
}
