
#include "LinearHashStorage.hpp"
#include <memory>
#include "Record.hpp"
#include "linear_hashing_table.h"
#include "utility/TimeUtils.hpp"

namespace kvs
{
    LinearHashStorage::LinearHashStorage(LinearHashStorage::StorageType storageType)
    {
        switch (storageType) {
            case StorageType::kDisk: {
                // m_bucketManager = std::make_unique<ContainerT::FileBucketManager>(32);
                // break;
            }
            case StorageType::kMemory: {
                m_bucketManager = std::make_unique<ContainerT::TransientBucketManager>(32);
                break;
            }
        }
        m_values = std::make_unique<LinearHashingTable<std::string, Record>>(m_bucketManager.get());
    }

    Status LinearHashStorage::getValue(const Key& key, Value* value)
    {
        Record record;
        auto found = m_values->find(key.getKey(), record);
        if (!found) {
            return Status{Status::FailReason::kKeyNotFound};
        }

        if (record.getExpirationTimestampMs() < getTimestampMs()) {
            m_values->remove(key.getKey(), record);
            return Status{Status::FailReason::kKeyNotFound};
        }

        *value = Value(record.getValueType(), record.getRawValue());
        return {};
    }

    Status LinearHashStorage::putValue(const Key& key, const Value& value)
    {
        m_values->insert(key.getKey(),
            Record{value.getType(), value.getRawValue(), getTimestampMs(), std::numeric_limits<int64_t>::max()});
        return {};
    }

    Status LinearHashStorage::deleteValue(const Key& key)
    {
        Record record;
        auto found = m_values->find(key.getKey(), record);
        if (!found) {
            return Status{Status::FailReason::kKeyNotFound};
        }
        m_values->remove(key.getKey(), record);
        return {};
    }

    void LinearHashStorage::flush() { m_bucketManager->flush(); }
}
