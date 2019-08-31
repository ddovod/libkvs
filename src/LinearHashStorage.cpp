
#include "LinearHashStorage.hpp"
#include "utility/TimeUtils.hpp"

namespace kvs
{
    Status LinearHashStorage::getValue(const Key& key, Value* value)
    {
        auto found = m_map.find(key.getKey());
        if (found == m_map.end()) {
            return Status{Status::FailReason::kKeyNotFound};
        }

        if (found->second.getExpirationTimestampMs() < getTimestampMs()) {
            m_map.erase(found);
            return Status{Status::FailReason::kKeyNotFound};
        }

        *value = Value{found->second.getValueType(), found->second.getRawValue()};
        return {};
    }

    Status LinearHashStorage::putValue(const Key& key, const Value& value)
    {
        m_map[key.getKey()] =
            Record{value.getType(), value.getRawValue(), getTimestampMs(), std::numeric_limits<int64_t>::max()};
        return {};
    }

    Status LinearHashStorage::deleteValue(const Key& key)
    {
        auto found = m_map.find(key.getKey());
        if (found == m_map.end()) {
            return Status{Status::FailReason::kKeyNotFound};
        }
        m_map.erase(found);
        return {};
    }
}
