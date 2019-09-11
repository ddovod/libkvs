
#include "LinearHashStorage.hpp"
#include <memory>
#include <mutex>
#include <vector>
#include "Record.hpp"
#include "ValueType.hpp"
#include "buffer.h"
#include "linear_hashing_table.h"
#include "utility/MGLockGuard.hpp"
#include "utility/TimeUtils.hpp"

namespace kvs
{
    LinearHashStorage::LinearHashStorage(const LinearHashStorageParams& params)
    {
        auto bucketCacheSize =
            std::max(params.hashTableParams.bucketCacheSizeBytes, params.hashTableParams.maxRecordSizeBytes);
        m_bucketManager = std::make_unique<HashTable::FileBucketManager>(params.hashTableParams.bucketCapacity,
            params.hashTableParams.minBucketsCount,
            params.primaryFilepath.c_str(),
            params.overflowFilepath.c_str(),
            serialize,
            deserialize,
            params.hashTableParams.maxRecordSizeBytes,
#ifdef BOOST_BIG_ENDIAN
            Buffer::Endian::BIG,
#else
            Buffer::Endian::LITTLE,
#endif
            bucketCacheSize);
        m_values = std::make_unique<HashTable>(m_bucketManager.get());
    }

    Status LinearHashStorage::getValue(const Key& key, Value* value)
    {
        MGLockGuard lock{m_storageLock, LockType::kS};

        Record record;
        auto found = m_values->find(key.getKey(), record);
        if (!found) {
            return Status{Status::FailReason::kKeyNotFound};
        }

        if (record.getExpirationTimestampMs() < getTimestampMs()) {
            std::lock_guard<std::mutex> expLock{m_expiredKeysMutex};
            m_expiredKeys.insert(key.getKey());
            return Status{Status::FailReason::kKeyNotFound};
        }

        *value = Value(record.getValueType(), record.getRawValue());
        return {};
    }

    Status LinearHashStorage::putValue(const Key& key, const Value& value)
    {
        {
            MGLockGuard lock{m_storageLock, LockType::kX};

            m_values->insert(key.getKey(),
                Record{value.getType(), value.getRawValue(), getTimestampMs(), key.getExpirationTimestampMs()});
        }

        {
            std::lock_guard<std::mutex> expLock{m_expiredKeysMutex};
            for (const auto& k : m_expiredKeys) {
                Record rec;
                m_values->remove(k, rec);
            }
        }

        return {};
    }

    Status LinearHashStorage::deleteValue(const Key& key)
    {
        MGLockGuard lock{m_storageLock, LockType::kX};

        Record record;
        auto found = m_values->find(key.getKey(), record);
        if (!found) {
            return Status{Status::FailReason::kKeyNotFound};
        }
        m_values->remove(key.getKey(), record);
        return {};
    }

    void LinearHashStorage::serialize(const std::string& key, const Record& value, Buffer& buffer)
    {
        buffer.put_uint64(key.size());
        buffer.put_chars(key.data(), key.size());

        buffer.put_ubyte(static_cast<uint8_t>(value.getValueType()));
        buffer.put_int64(value.getCreationTimestampMs());
        buffer.put_int64(value.getExpirationTimestampMs());
        const auto& rawValue = value.getRawValue();
        buffer.put_uint64(rawValue.size());
        buffer.put_bytes(rawValue.data(), rawValue.size());
    }

    void LinearHashStorage::deserialize(Buffer& buffer, std::string& key, Record& value)
    {
        size_t keySize = buffer.get_uint64();
        key.resize(keySize);
        buffer.get_chars(key.data(), keySize);

        auto valueType = static_cast<ValueType>(buffer.get_ubyte());
        auto creationTime = buffer.get_int64();
        auto expirationTime = buffer.get_int64();
        auto valueSize = buffer.get_uint64();
        std::vector<uint8_t> rawValue(valueSize);
        buffer.get_bytes(rawValue.data(), valueSize);
        value = Record(valueType, rawValue, creationTime, expirationTime);
    }
}
