
#pragma once

#include <cstdint>
#include <vector>
#include "ValueType.hpp"

namespace kvs
{
    /** A low level value record. */
    class Record
    {
    public:
        Record() = default;
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

        /** Returns the type of the value. */
        ValueType getValueType() const { return m_valueType; }

        /** Returns the byte array which stores the value. */
        const std::vector<uint8_t>& getRawValue() const { return m_rawValue; }

        /** Returns the timestamp of the value creation, in milliseconds. */
        int64_t getCreationTimestampMs() const { return m_creationTimestampMs; }

        /** Returns the timestamp of the value expiration, in milliseconds. */
        int64_t getExpirationTimestampMs() const { return m_expirationTimestampMs; }

    private:
        ValueType m_valueType = ValueType::kUndefined;
        std::vector<uint8_t> m_rawValue;
        int64_t m_creationTimestampMs = 0;
        int64_t m_expirationTimestampMs = 0;
    };
}
