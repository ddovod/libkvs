
#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace kvs
{
    class Keys
    {
    public:
        Keys(const std::string& path, size_t indexFrom, size_t keysCount)
            : m_path(path)
            , m_indexFrom(indexFrom)
            , m_keysCount(keysCount)
        {
        }

        const std::string& getPath() const { return m_path; }
        size_t getIndexFrom() const { return m_indexFrom; }
        size_t getKeysCount() const { return m_keysCount; }

    private:
        std::string m_path;
        size_t m_indexFrom = 0;
        size_t m_keysCount = 0;
    };

    class KeysRange
    {
    public:
        KeysRange() = default;
        explicit KeysRange(std::vector<std::string>&& keys)
            : m_keys(std::move(keys))
        {
        }

        const std::vector<std::string>& getKeys() const { return m_keys; }

    private:
        std::vector<std::string> m_keys;
    };
}
