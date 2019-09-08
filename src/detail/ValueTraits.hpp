
#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <bits/stdint-intn.h>
#include "Data.hpp"
#include "Error.hpp"
#include "ValueType.hpp"

namespace kvs::detail
{
    template <typename T, ValueType valueType>
    struct PrimitiveValueTraits
    {
        static void setValue(T value, std::vector<uint8_t>& data, ValueType& type)
        {
            data.resize(sizeof(T));
            memcpy(data.data(), &value, sizeof(T));
            type = valueType;
        }

        static T getValue(const std::vector<uint8_t>& data, ValueType type)
        {
            if (type != valueType) {
                throw ValueTypeMismatch(valueType, type);
            }

            T result;
            memcpy(&result, data.data(), sizeof(T));
            return result;
        }
    };

    template <typename T>
    struct ValueTraits;

    template <>
    struct ValueTraits<int32_t> : public PrimitiveValueTraits<int32_t, ValueType::kInt32>
    {
    };

    template <>
    struct ValueTraits<uint32_t> : public PrimitiveValueTraits<uint32_t, ValueType::kUInt32>
    {
    };

    template <>
    struct ValueTraits<int64_t> : public PrimitiveValueTraits<int64_t, ValueType::kInt64>
    {
    };

    template <>
    struct ValueTraits<uint64_t> : public PrimitiveValueTraits<uint64_t, ValueType::kUInt64>
    {
    };

    template <>
    struct ValueTraits<float> : public PrimitiveValueTraits<float, ValueType::kFloat>
    {
    };

    template <>
    struct ValueTraits<double> : public PrimitiveValueTraits<double, ValueType::kDouble>
    {
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

    template <>
    struct ValueTraits<Data>
    {
        static void setValue(const Data& value, std::vector<uint8_t>& data, ValueType& type)
        {
            data.resize(value.buffer.size());
            memcpy(data.data(), value.buffer.data(), value.buffer.size());
            type = ValueType::kData;
        }

        static Data getValue(const std::vector<uint8_t>& data, ValueType type)
        {
            if (type != ValueType::kData) {
                throw ValueTypeMismatch(ValueType::kData, type);
            }

            Data result;
            result.buffer.assign(data.begin(), data.end());
            return result;
        }
    };
}
