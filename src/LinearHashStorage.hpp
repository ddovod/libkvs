
#pragma once

#include <linear_hashing_table.h>
#include <unordered_map>
#include "IStorage.hpp"
#include "Record.hpp"
#include "Value.hpp"

namespace kvs
{
    class LinearHashStorage : public IStorage
    {
    public:
        enum class StorageType
        {
            kDisk,
            kMemory,
        };

        LinearHashStorage(StorageType storageType = StorageType::kDisk);

        Status getValue(const Key& key, Value* value) override;
        Status putValue(const Key& key, const Value& value) override;
        Status deleteValue(const Key& key) override;

        void flush();

    private:
        using ContainerT = LinearHashingTable<std::string, Record>;

        std::unique_ptr<ContainerT::BucketManager> m_bucketManager;
        std::unique_ptr<ContainerT> m_values;
    };
}
