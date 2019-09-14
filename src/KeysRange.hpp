
#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace kvs
{
    /** A wrapper for a range of keys. Used in key range queries. */
    class Keys
    {
    public:
        /**
         * Constructor.
         * \param path Path to the node.
         * \param indexFrom Offset of the first key.
         * \param keysCount Number of keys.
         */
        Keys(const std::string& path, size_t indexFrom, size_t keysCount)
            : m_path(path)
            , m_indexFrom(indexFrom)
            , m_keysCount(keysCount)
        {
        }

        /** Returns the path to the node. */
        const std::string& getPath() const { return m_path; }

        /** Returns the offset of the first key. */
        size_t getIndexFrom() const { return m_indexFrom; }

        /** Returns the number of keys. */
        size_t getKeysCount() const { return m_keysCount; }

    private:
        std::string m_path;
        size_t m_indexFrom = 0;
        size_t m_keysCount = 0;
    };

    /** A wrapper for a range of the keys. */
    class KeysRange
    {
    public:
        KeysRange() = default;
        explicit KeysRange(std::vector<std::string>&& keys)
            : m_keys(std::move(keys))
        {
        }

        /** Returns an unordered array of the keys. */
        const std::vector<std::string>& getKeys() const { return m_keys; }

    private:
        std::vector<std::string> m_keys;
    };
}
