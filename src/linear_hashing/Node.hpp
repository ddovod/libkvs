
#pragma once

#include <string>

namespace kvs
{
    /** A metadata of the volume node. */
    struct Node
    {
        std::string primaryFilepath;
        std::string overflowFilepath;
    };
}
