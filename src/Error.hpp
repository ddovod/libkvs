
#pragma once

#include <exception>
#include "ValueType.hpp"

namespace kvs
{
    class ValueTypeMismatch : public std::exception
    {
    public:
        ValueTypeMismatch(ValueType expectedType, ValueType actualType)
            : m_expectedType(expectedType)
            , m_actualType(actualType)
        {
        }

        const char* what() const noexcept override { return "ValueType mismatch"; }

        ValueType getExpectedType() const { return m_expectedType; }
        ValueType getActualType() const { return m_actualType; }

    private:
        ValueType m_expectedType = ValueType::kUndefined;
        ValueType m_actualType = ValueType::kUndefined;
    };
}
