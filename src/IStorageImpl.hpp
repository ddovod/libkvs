
#pragma once

#include "IStorage.hpp"

namespace kvs
{
    class IStorageImpl : public IStorage
    {
    public:
        virtual size_t getSize() const = 0;
    };
}
