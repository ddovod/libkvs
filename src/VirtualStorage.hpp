
#pragma once

#include <unordered_map>
#include "ICompositeStorage.hpp"
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"

namespace kvs
{
    class VirtualStorage : public ICompositeStorage, public IStorage
    {
    public:
        VirtualStorage();

        MountResult mount(const MountOptions& mountOptions) override;

        Status getValue(const Key& key, Value* value) override;
        Status putValue(const Key& key, const Value& value) override;
        Status deleteValue(const Key& key) override;

    private:
        struct MountedStorage
        {
            IStorage* storage = nullptr;
            int priority = 0;
        };

        struct Node
        {
            std::vector<MountedStorage> storages;
            std::unordered_map<std::string, std::unique_ptr<Node>> children;
        };

        std::unique_ptr<IStorageRegistry> m_storageRegistry;
        Node m_rootNode;

        Node* resolveNode(const std::string& path);
    };
}
