
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
        Value(const std::string& value) { detail::ValueTraits<std::string>::setValue(value, m_value, m_type); }

        ValueType getType() const { return m_type; }

        template <typename T>
        T getValue() const
        {
            return detail::ValueTraits<std::decay_t<T>>::getValue(m_value, m_type);
        }

    private:
        std::vector<uint8_t> m_value;
        ValueType m_type = ValueType::kUndefined;
    };
}
