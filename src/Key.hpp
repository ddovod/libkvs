
#pragma once

#include <string>

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
}
