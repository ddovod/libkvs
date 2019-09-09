
#pragma once

#include <string>
#include "utility/TimeUtils.hpp"

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

        Key(const std::string& key, const std::string& path, int64_t timeToLiveMs)
            : m_key(key)
            , m_path(path)
        {
            const auto expMaxMs = std::numeric_limits<int64_t>::max();
            const auto nowMs = getTimestampMs();
            if (expMaxMs - nowMs <= timeToLiveMs) {
                m_expirationTimestampMs = nowMs + timeToLiveMs;
            }
        }

        const std::string& getPath() const { return m_path; }
        const std::string& getKey() const { return m_key; }
        int64_t getExpirationTimestampMs() const { return m_expirationTimestampMs; }

    private:
        const std::string& m_key;
        const std::string& m_path;
        int64_t m_expirationTimestampMs = std::numeric_limits<int64_t>::max();
    };
}
