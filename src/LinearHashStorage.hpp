
#pragma once

#include <memory>
#include <unordered_map>
#include "HashTable.hpp"
#include "IStorage.hpp"
#include "Record.hpp"
#include "Value.hpp"
#include "buffer.h"

namespace kvs
{
    struct HashTableParams
    {
        size_t bucketCapacity = 32;
        size_t maxRecordSizeBytes = 1024 * 1024;         // 1Mb
        size_t bucketCacheSizeBytes = 1024 * 1024 * 64;  // 64Mb
        size_t minBucketsCount = 32;
        double minLoadFactor = 0.75;
        double maxLoadFactor = 0.95;
    };

    struct LinearHashStorageParams
    {
        std::string primaryFilepath;
        std::string overflowFilepath;
        HashTableParams hashTableParams;
    };

    class LinearHashStorage : public IStorage
    {
    public:
        LinearHashStorage(const LinearHashStorageParams& params);

        Status getValue(const Key& key, Value* value) override;
        Status putValue(const Key& key, const Value& value) override;
        Status deleteValue(const Key& key) override;

    private:
        std::unique_ptr<HashTable::BucketManager> m_bucketManager;
        std::unique_ptr<HashTable> m_values;

        static void serialize(const std::string& key, const Record& value, Buffer& buffer);
        static void deserialize(Buffer& buffer, std::string& key, Record& value);
    };
}
