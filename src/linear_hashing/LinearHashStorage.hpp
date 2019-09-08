
#pragma once

#include <memory>
#include <unordered_map>
#include "HashTable.hpp"
#include "HashTableParams.hpp"
#include "IStorage.hpp"
#include "Record.hpp"
#include "Value.hpp"

namespace kvs
{
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
