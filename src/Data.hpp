
#pragma once

#include <cstdint>
#include <vector>

namespace kvs
{
    /** A simple byte array wrapper. */
    struct Data
    {
        std::vector<uint8_t> buffer;
    };
}
