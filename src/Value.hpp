
#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>
#include "ValueType.hpp"
#include "detail/ValueTraits.hpp"

namespace kvs
{
    class Value
    {
    public:
        Value() = default;
        Value(int32_t value) { detail::ValueTraits<int32_t>::setValue(value, m_value, m_type); }
        Value(uint32_t value) { detail::ValueTraits<uint32_t>::setValue(value, m_value, m_type); }
        Value(int64_t value) { detail::ValueTraits<int64_t>::setValue(value, m_value, m_type); }
        Value(uint64_t value) { detail::ValueTraits<uint64_t>::setValue(value, m_value, m_type); }
        Value(float value) { detail::ValueTraits<float>::setValue(value, m_value, m_type); }
        Value(double value) { detail::ValueTraits<double>::setValue(value, m_value, m_type); }
        Value(const std::string& value) { detail::ValueTraits<std::string>::setValue(value, m_value, m_type); }
        Value(const Data& value) { detail::ValueTraits<Data>::setValue(value, m_value, m_type); }
        Value(ValueType valueType, const std::vector<uint8_t>& rawValue)
            : m_value(rawValue)
            , m_type(valueType)
        {
        }

        ValueType getType() const { return m_type; }

        template <typename T>
        T getValue() const
        {
            return detail::ValueTraits<std::decay_t<T>>::getValue(m_value, m_type);
        }

        const std::vector<uint8_t>& getRawValue() const { return m_value; }

    private:
        std::vector<uint8_t> m_value;
        ValueType m_type = ValueType::kUndefined;
    };
}
