
#include "SimpleInMemoryStorage.hpp"
#include <limits>
#include "Record.hpp"
#include "Status.hpp"
#include "Value.hpp"
#include "utility/TimeUtils.hpp"

namespace kvs
{
    Status SimpleInMemoryStorage::getValue(const Key& key, Value* value)
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

    Status SimpleInMemoryStorage::putValue(const Key& key, const Value& value)
    {
        m_map[key.getKey()] =
            Record{value.getType(), value.getRawValue(), getTimestampMs(), std::numeric_limits<int64_t>::max()};
        return {};
    }

    Status SimpleInMemoryStorage::deleteValue(const Key& key)
    {
        m_map.erase(key.getKey());
        return {};
    }
}
