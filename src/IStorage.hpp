
#pragma once

#include <cstring>
#include <string>
#include <vector>
#include "Key.hpp"
#include "Status.hpp"
#include "Value.hpp"

namespace kvs
{
    class IStorage
    {
    public:
        virtual ~IStorage() = default;

        virtual Status getValue(const Key& key, Value* value) = 0;
        virtual Status putValue(const Key& key, const Value& value) = 0;
        virtual Status deleteValue(const Key& key) = 0;
    };
}
