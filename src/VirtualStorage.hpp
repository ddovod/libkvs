
#pragma once

#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include "ICompositeStorage.hpp"
#include "IStorageImpl.hpp"
#include "IStorageRegistry.hpp"
#include "KeysRange.hpp"
#include "StorageNode.hpp"
#include "utility/MGLockGuard.hpp"

namespace kvs
{
    class VirtualStorage : public ICompositeStorage
    {
    public:
        VirtualStorage();

        MountResult mount(const MountOptions& mountOptions) override;
        UnmountResult unmount(const UnmountOptions& unmountOptions) override;

        Status getValue(const Key& key, Value* value) override;
        Status putValue(const Key& key, const Value& value) override;
        Status deleteValue(const Key& key) override;
        Status getKeysRange(const Keys& keys, KeysRange* keysRange) override;

    private:
        struct Node;
        struct RemountOptions
        {
            Node* node = nullptr;
            uint64_t mountId;
            MountOptions mountOptions;
        };

        struct MountedStorage
        {
            IStorageImpl* storage = nullptr;
            int priority = 0;
        };

        struct MountPoint
        {
            std::unique_ptr<StorageNode> storageNode;
            int priority = 0;
            RemountOptions remountOptions;
        };

        struct Node
        {
            MGMutex nodeLock;
            std::unordered_map<uint64_t, MountPoint> mountPoints;
            std::vector<MountedStorage> storages;
            std::unordered_map<std::string, std::unique_ptr<Node>> children;
        };

        std::unique_ptr<IStorageRegistry> m_storageRegistry;
        Node m_rootNode;
        uint64_t m_mountIdCounter = 1;
        std::unordered_map<uint64_t, Node*> m_mountPoints;
        std::unordered_map<std::string, std::vector<Node*>> m_volumesToNodeMountPointsMap;

        Node* resolveNode(const std::string& path, MGMultiLockGuard& locks, LockType type);
        void mount(Node& node, StorageNode& storageNode, int priority);
        void unmount(Node& node, StorageNode& storageNode);
        Node* createNodesForPath(const std::string& path);
        MountResult remountNodes(const MountOptions& mountOptions);
        MountResult createMountResult(const StorageAcquisitionResult& acquireResult);
    };
}
