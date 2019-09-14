
#pragma once

#include "IStorage.hpp"

namespace kvs
{
    /** An extended interface of the storage, used by the implementation. */
    class IStorageImpl : public IStorage
    {
    public:
        virtual size_t getSize() const = 0;
        virtual bool hasKey(const Key& key) const = 0;
    };
}
