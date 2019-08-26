
#pragma once

#include <cstring>
#include <string>
#include <vector>

namespace kvs
{
    class Key
    {
    public:
        Key(const std::string& key, const std::string& path)
            : m_key(key)
            , m_path(path)
        {
        }

        const std::string& getPath() const { return m_path; }
        const std::string& getKey() const { return m_key; }

    private:
        const std::string& m_key;
        const std::string& m_path;
    };

    class Value
    {
    public:
        Value() = default;
        Value(int32_t value) { setValue(value); }
        Value(uint32_t value) { setValue(value); }
        Value(int64_t value) { setValue(value); }
        Value(uint64_t value) { setValue(value); }
        Value(float value) { setValue(value); }
        Value(double value) { setValue(value); }
        Value(const std::string& value) { setValue(value); }

    private:
        std::vector<uint8_t> m_value;

        template <typename T>
        void setValue(T value)
        {
            m_value.resize(sizeof(value));
            memcpy(m_value.data(), &value, sizeof(value));
        }

        void setValue(const std::string& value)
        {
            m_value.resize(value.size());
            memcpy(m_value.data(), value.data(), value.size());
        }
    };

    class Status
    {
    public:
        enum class FailReason
        {
            kOk,
            kNodeNotFound,
            kKeyNotFound,
        };

        Status() = default;
        explicit Status(FailReason failReason)
            : m_failReason(failReason)
        {
        }

        bool isOk() const { return m_failReason == FailReason::kOk; }
        FailReason getFailReason() const { return m_failReason; }

    private:
        FailReason m_failReason = FailReason::kOk;
    };

    class IStorage
    {
    public:
        virtual ~IStorage() = default;

        virtual Status getValue(const Key& key, Value* value) = 0;
        virtual Status putValue(const Key& key, const Value& value) = 0;
        virtual Status deleteValue(const Key& key) = 0;
    };
}
