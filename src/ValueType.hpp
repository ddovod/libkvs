
#pragma once

#include <cstdint>

namespace kvs
{
    enum class ValueType : uint8_t
    {
        kUndefined = 0,
        kInt32,
        kUInt32,
        kInt64,
        kUInt64,
        kFloat,
        kDouble,
        kString,
        kData,
    };
}
