
#include "LinearHashStorage.hpp"
#include <memory>
#include <mutex>
#include <vector>
#include "Record.hpp"
#include "ValueType.hpp"
#include "buffer.h"
#include "linear_hashing_table.h"
#include "utility/MGLockGuard.hpp"
#include "utility/MGMutex.hpp"
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
        MGLockGuard lock{m_storageLock, LockType::kX};
        clearExpiredKeys();

        if (m_values->contains(key.getKey())) {
            Record record;
            m_values->remove(key.getKey(), record);
        }

        m_values->insert(key.getKey(),
            Record{value.getType(), value.getRawValue(), getTimestampMs(), key.getExpirationTimestampMs()});

        return {};
    }

    Status LinearHashStorage::deleteValue(const Key& key)
    {
        MGLockGuard lock{m_storageLock, LockType::kX};
        clearExpiredKeys();

        if (!m_values->contains(key.getKey())) {
            return Status{Status::FailReason::kKeyNotFound};
        }
        Record record;
        m_values->remove(key.getKey(), record);
        return {};
    }

    Status LinearHashStorage::getKeysRange(const Keys& keys, KeysRange* keysRange)
    {
        MGLockGuard lock{m_storageLock, LockType::kS};

        auto result = m_values->getKeysRange(keys.getIndexFrom(), keys.getKeysCount());
        *keysRange = KeysRange{std::move(result)};
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

    size_t LinearHashStorage::getSize() const
    {
        MGLockGuard lock{m_storageLock, LockType::kS};
        return m_values->size();
    }

    bool LinearHashStorage::hasKey(const Key& key) const
    {
        MGLockGuard lock{m_storageLock, LockType::kS};
        return m_values->contains(key.getKey());
    }

    void LinearHashStorage::clearExpiredKeys()
    {
        std::lock_guard<std::mutex> expLock{m_expiredKeysMutex};
        std::unordered_set<std::string> keysDeleted;
        for (const auto& k : m_expiredKeys) {
            Record rec;
            m_values->remove(k, rec);
            keysDeleted.insert(k);
            if (keysDeleted.size() >= 5) {
                break;
            }
        }

        for (const auto& k : keysDeleted) {
            m_expiredKeys.erase(k);
        }
    }
}
