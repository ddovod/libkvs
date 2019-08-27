
#pragma once

#include <cstdint>
#include <vector>
#include "ValueType.hpp"

namespace kvs
{
    class Record
    {
    public:
        Record(ValueType valueType,
            const std::vector<uint8_t>& rawValue,
            int64_t creationTimestampMs,
            int64_t expirationTimestampMs)
            : m_valueType(valueType)
            , m_rawValue(rawValue)
            , m_creationTimestampMs(creationTimestampMs)
            , m_expirationTimestampMs(expirationTimestampMs)
        {
        }

        ValueType getValueType() const { return m_valueType; }
        const std::vector<uint8_t>& getRawValue() const { return m_rawValue; }
        int64_t getCreationTimestampMs() const { return m_creationTimestampMs; }
        int64_t getExpirationTimestampMs() const { return m_expirationTimestampMs; }

    private:
        ValueType m_valueType = ValueType::kUndefined;
        std::vector<uint8_t> m_rawValue;
        int64_t m_creationTimestampMs = 0;
        int64_t m_expirationTimestampMs = 0;
    };
}
