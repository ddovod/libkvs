
#pragma once

#include <string>
#include "utility/TimeUtils.hpp"

namespace kvs
{
    /**
     * A wrapper for a key.
     * Contains a full path to the node and a key of the record.
     */
    class Key
    {
    public:
        /**
         * Constructor.
         * \param key A key of the record.
         * \param path A path to the node.
         */
        Key(const std::string& key, const std::string& path)
            : m_key(key)
            , m_path(path)
        {
        }

        /**
         * Constructor.
         * \param key A key of the record.
         * \param path A path to the node.
         * \param timeToLiveMs TTL for this key, in milliseconds.
         */
        Key(const std::string& key, const std::string& path, int64_t timeToLiveMs)
            : m_key(key)
            , m_path(path)
        {
            const auto expMaxMs = std::numeric_limits<int64_t>::max();
            const auto nowMs = utility::getTimestampMs();
            if (expMaxMs - nowMs >= timeToLiveMs) {
                m_expirationTimestampMs = nowMs + timeToLiveMs;
            }
        }

        /** Returns the path to the node. */
        const std::string& getPath() const { return m_path; }

        /** Returns the key of the record. */
        const std::string& getKey() const { return m_key; }

        /** Returns the timestamp when key will be expired, in milliseconds. */
        int64_t getExpirationTimestampMs() const { return m_expirationTimestampMs; }

    private:
        const std::string& m_key;
        const std::string& m_path;
        int64_t m_expirationTimestampMs = std::numeric_limits<int64_t>::max();
    };
}
