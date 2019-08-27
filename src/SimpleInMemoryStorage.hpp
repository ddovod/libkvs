
#pragma once

#include <unordered_map>
#include "IStorage.hpp"
#include "Record.hpp"
#include "Value.hpp"

namespace kvs
{
    class SimpleInMemoryStorage : public IStorage
    {
    public:
        Status getValue(const Key& key, Value* value) override;
        Status putValue(const Key& key, const Value& value) override;
        Status deleteValue(const Key& key) override;

    private:
        std::unordered_map<std::string, Record> m_map;
    };
}
