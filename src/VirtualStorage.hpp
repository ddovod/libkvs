
#pragma once

#include <atomic>
#include <unordered_map>
#include <bits/stdint-uintn.h>
#include "ICompositeStorage.hpp"
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"
#include "StorageNode.hpp"
#include "utility/MGLockGuard.hpp"

namespace kvs
{
    class VirtualStorage : public ICompositeStorage, public IStorage
    {
    public:
        VirtualStorage();

        MountResult mount(const MountOptions& mountOptions) override;
        UnmountResult unmount(const UnmountOptions& unmountOptions) override;

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
            MGMutex nodeLock;
            std::unordered_map<uint64_t, StorageNode> mountPoints;
            std::vector<MountedStorage> storages;
            std::unordered_map<std::string, std::unique_ptr<Node>> children;
        };

        std::unique_ptr<IStorageRegistry> m_storageRegistry;
        Node m_rootNode;
        uint64_t m_mountIdCounter = 1;
        std::unordered_map<uint64_t, Node*> m_mountPoints;

        Node* resolveNode(const std::string& path, MGMultiLockGuard& locks, LockType type);
        void mount(Node& node, StorageNode& storageNode, int priority);
        void unmount(Node& node, StorageNode& storageNode);
    };
}
