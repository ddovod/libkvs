
#pragma once

#include <unordered_map>
#include "IStorage.hpp"
#include "IVirtualStorage.hpp"

namespace kvs
{
    class VirtualStorage : public IVirtualStorage
    {
    public:
        MountResult mount(const MountOptions& mountOptions) override;

        Status getValue(const Key& key, Value* value) override;
        Status putValue(const Key& key, const Value& value) override;
        Status deleteValue(const Key& key) override;

    private:
        struct Node
        {
            std::shared_ptr<IStorage> storage;
            std::unordered_map<std::string, std::unique_ptr<Node>> children;
        };
        Node m_rootNode;

        std::shared_ptr<IStorage> resolveNode(const std::string& path);
    };
}
