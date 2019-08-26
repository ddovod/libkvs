
#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <bits/stdint-intn.h>
#include "Error.hpp"
#include "ValueType.hpp"

namespace kvs::detail
{
    template <typename T>
    struct ValueTraits;

    template <>
    struct ValueTraits<int32_t>
    {
        static void setValue(int32_t value, std::vector<uint8_t>& data, ValueType& type)
        {
            data.resize(sizeof(int32_t));
            memcpy(data.data(), &value, sizeof(int32_t));
            type = ValueType::kInt32;
        }

        static int32_t getValue(const std::vector<uint8_t>& data, ValueType type)
        {
            if (type != ValueType::kInt32) {
                throw ValueTypeMismatch(ValueType::kInt32, type);
            }

            int32_t result;
            memcpy(&result, data.data(), sizeof(int32_t));
            return result;
        }
    };

    template <>
    struct ValueTraits<std::string>
    {
        static void setValue(const std::string& value, std::vector<uint8_t>& data, ValueType& type)
        {
            data.resize(value.size());
            memcpy(data.data(), value.data(), value.size());
            type = ValueType::kString;
        }

        static std::string getValue(const std::vector<uint8_t>& data, ValueType type)
        {
            if (type != ValueType::kString) {
                throw ValueTypeMismatch(ValueType::kString, type);
            }

            std::string result;
            result.assign(data.begin(), data.end());
            return result;
        }
    };
}
