
#include "VirtualStorage.hpp"
#include <algorithm>
#include <cassert>
#include <memory>
#include "ICompositeStorage.hpp"
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"
#include "Status.hpp"
#include "StorageNode.hpp"
#include "linear_hashing/LinearHashStorageRegistry.hpp"
#include "utility/MGLockGuard.hpp"
#include "utility/StringUtils.hpp"

namespace kvs
{
    VirtualStorage::VirtualStorage()
        : m_storageRegistry(std::make_unique<LinearHashStorageRegistry>())
    {
    }

    MountResult VirtualStorage::mount(const MountOptions& mountOptions)
    {
        MGLockGuard lock{m_rootNode.nodeLock, LockType::kX};

        // Loading volume subtree
        StorageAcquisitionOptions acquireOptions;
        acquireOptions.volumeFilePath = mountOptions.volumeFilePath;
        acquireOptions.nodePath = mountOptions.nodePath;
        acquireOptions.hashTableParams = mountOptions.hashTableParams;
        auto acquireResult = m_storageRegistry->acquireStorage(acquireOptions);
        if (!acquireResult.isOk()) {
            return createMountResult(acquireResult);
        }
        auto storageNode = std::move(acquireResult.getRoot());

        // Re-mounting nodes to the previously mounted points if there're new nodes found
        auto remountResult = remountNodes(mountOptions);
        if (!remountResult.isOk()) {
            return remountResult;
        }

        // Creating virtual structure to the mount point
        Node* node = createNodesForPath(mountOptions.storageMountPath);

        // Mounting volume nodes to the virtual nodes
        auto mountId = m_mountIdCounter++;
        mount(*node, *storageNode, mountOptions.priority);
        m_mountPoints[mountId] = node;
        m_volumesToNodeMountPointsMap[mountOptions.volumeFilePath].push_back(node);
        MountPoint mountPoint;
        mountPoint.storageNode = std::move(storageNode);
        mountPoint.priority = mountOptions.priority;
        mountPoint.remountOptions.node = node;
        mountPoint.remountOptions.mountId = mountId;
        mountPoint.remountOptions.mountOptions = mountOptions;
        node->mountPoints[mountId] = std::move(mountPoint);
        return MountResult{MountResult::Status::kOk, mountId};
    }

    void VirtualStorage::mount(Node& node, StorageNode& storageNode, int priority)
    {
        // Mounting node at the end of the same priority group preserving priorities in descending order
        MountedStorage newStorage;
        newStorage.storage = storageNode.storage;
        newStorage.priority = priority;
        auto ub =
            std::upper_bound(node.storages.begin(), node.storages.end(), newStorage, [](const auto& l, const auto& r) {
                return l.priority > r.priority;
            });
        node.storages.insert(ub, newStorage);

        for (auto& child : storageNode.children) {
            auto found = node.children.find(child.first);
            if (found == node.children.end()) {
                found = node.children.insert({child.first, std::make_unique<Node>()}).first;
            }
            mount(*found->second, *child.second, priority);
        }
    }

    UnmountResult VirtualStorage::unmount(const UnmountOptions& unmountOptions)
    {
        MGLockGuard lock{m_rootNode.nodeLock, LockType::kX};

        auto foundNode = m_mountPoints.find(unmountOptions.mountId);
        if (foundNode == m_mountPoints.end()) {
            return UnmountResult(UnmountResult::Status::kInvalidMountId);
        }

        auto foundStorageNode = foundNode->second->mountPoints.find(unmountOptions.mountId);
        assert(foundStorageNode != foundNode->second->mountPoints.end());

        unmount(*foundNode->second, *foundStorageNode->second.storageNode);

        foundNode->second->mountPoints.erase(foundStorageNode);
        m_mountPoints.erase(foundNode);

        return UnmountResult(UnmountResult::Status::kOk);
    }

    void VirtualStorage::unmount(Node& node, StorageNode& storageNode)
    {
        auto foundStorage = std::find_if(node.storages.begin(),
            node.storages.end(),
            [&storageNode](const auto& mountedStorage) { return mountedStorage.storage == storageNode.storage; });
        assert(foundStorage != node.storages.end());

        StorageReleaseOptions releaseOptions;
        releaseOptions.storage = storageNode.storage;
        auto releaseResult = m_storageRegistry->releaseStorage(releaseOptions);
        assert(releaseResult.isOk());
        (void)releaseResult;

        node.storages.erase(foundStorage);
        for (auto& child : storageNode.children) {
            auto foundNode = node.children.find(child.first);
            assert(foundNode != node.children.end());
            unmount(*foundNode->second, *child.second);
        }
    }

    Status VirtualStorage::getValue(const Key& key, Value* value)
    {
        assert(value != nullptr);

        MGMultiLockGuard locks;
        if (auto node = resolveNode(key.getPath(), locks, LockType::kIS); node && !node->storages.empty()) {
            for (const auto& storage : node->storages) {
                auto status = storage.storage->getValue(key, value);
                if (status.isOk()) {
                    return status;
                }
            }
            return Status{Status::FailReason::kKeyNotFound};
        }
        return Status{Status::FailReason::kNodeNotFound};
    }

    Status VirtualStorage::putValue(const Key& key, const Value& value)
    {
        MGMultiLockGuard locks;
        if (auto node = resolveNode(key.getPath(), locks, LockType::kIX); node && !node->storages.empty()) {
            return node->storages.front().storage->putValue(key, value);
        }
        return Status{Status::FailReason::kNodeNotFound};
    }

    Status VirtualStorage::deleteValue(const Key& key)
    {
        MGMultiLockGuard locks;
        auto node = resolveNode(key.getPath(), locks, LockType::kIX);
        if (!node || node->storages.empty()) {
            return Status{Status::FailReason::kNodeNotFound};
        }

        for (const auto& storage : node->storages) {
            auto status = storage.storage->deleteValue(key);
            if (status.isOk()) {
                return {};
            }
        }

        return Status{Status::FailReason::kKeyNotFound};
    }

    Status VirtualStorage::getKeysRange(const Keys& keys, KeysRange* keysRange)
    {
        assert(keysRange != nullptr);

        MGMultiLockGuard locks;
        auto node = resolveNode(keys.getPath(), locks, LockType::kIS);
        if (!node || node->storages.empty()) {
            return Status{Status::FailReason::kNodeNotFound};
        }

        // Skipping first `getIndexFrom` keys
        std::unordered_set<std::string> excludeKeys;
        auto indicesToSkip = keys.getIndexFrom();
        size_t storageIndex = 0;
        size_t storageKeyIndex = 0;
        KeysRange storageKeys;
        while (indicesToSkip > 0 && storageIndex < node->storages.size()) {
            auto storage = node->storages[storageIndex].storage;
            size_t keysToRead = std::min(indicesToSkip, storage->getSize() - storageKeyIndex);
            storage->getKeysRange({"", storageKeyIndex, keysToRead}, &storageKeys);
            for (const auto& k : storageKeys.getKeys()) {
                excludeKeys.insert(k);
            }
            storageKeyIndex += keysToRead;
            indicesToSkip -= storageKeys.getKeys().size();
            if (storageKeyIndex >= storage->getSize()) {
                storageIndex++;
                storageKeyIndex = 0;
            }
        }

        // Collecting up to `getKeysCount` keys starting from the current position
        auto keysLeft = keys.getKeysCount();
        std::vector<std::string> resultKeys;
        resultKeys.reserve(keysLeft);
        while (keysLeft > 0 && storageIndex < node->storages.size()) {
            auto storage = node->storages[storageIndex].storage;
            size_t keysToRead = std::min(keysLeft, storage->getSize() - storageKeyIndex);
            storage->getKeysRange({"", storageKeyIndex, keysToRead}, &storageKeys);
            size_t addedToReault = 0;
            for (const auto& k : storageKeys.getKeys()) {
                if (excludeKeys.find(k) == excludeKeys.end()) {
                    resultKeys.push_back(k);
                    addedToReault++;
                    excludeKeys.insert(k);
                }
            }
            storageKeyIndex += keysToRead;
            keysLeft -= addedToReault;
            if (storageKeyIndex >= storage->getSize()) {
                storageIndex++;
                storageKeyIndex = 0;
            }
        }

        *keysRange = KeysRange{std::move(resultKeys)};
        return Status{Status::FailReason::kOk};
    }

    VirtualStorage::Node* VirtualStorage::resolveNode(const std::string& path, MGMultiLockGuard& locks, LockType type)
    {
        locks.addLock(m_rootNode.nodeLock, type);
        Node* node = &m_rootNode;
        for (const auto& comp : split(path, "/")) {
            auto found = node->children.find(comp);
            if (found == node->children.end()) {
                return {};
            }
            node = found->second.get();
            locks.addLock(node->nodeLock, type);
        }
        return node;
    }

    VirtualStorage::Node* VirtualStorage::createNodesForPath(const std::string& path)
    {
        Node* node = &m_rootNode;
        for (const auto& comp : split(path, "/")) {
            auto found = node->children.find(comp);
            if (found == node->children.end()) {
                auto newNode = std::make_unique<Node>();
                auto newNodePtr = newNode.get();
                node->children[comp] = std::move(newNode);
                node = newNodePtr;
            } else {
                node = found->second.get();
            }
        }
        assert(node != nullptr);

        return node;
    }

    MountResult VirtualStorage::remountNodes(const MountOptions& mountOptions)
    {
        auto found = m_volumesToNodeMountPointsMap.find(mountOptions.volumeFilePath);
        if (found != m_volumesToNodeMountPointsMap.end()) {
            std::vector<RemountOptions> remountEntries;
            auto nodesCopy = found->second;
            for (auto foundNode : nodesCopy) {
                for (auto& mountPointPair : foundNode->mountPoints) {
                    auto& mountedStorageNode = mountPointPair.second.storageNode;
                    if (mountedStorageNode->volumeFilePath != mountOptions.volumeFilePath) {
                        // Another volume
                        continue;
                    }
                    if (!startsWith(mountOptions.nodePath, mountedStorageNode->nodePath)) {
                        // Not a subtree
                        continue;
                    }
                    if (mountOptions.nodePath == mountedStorageNode->nodePath) {
                        // Exactly the same structure
                        continue;
                    }
                    remountEntries.push_back(mountPointPair.second.remountOptions);
                }
            }

            for (const auto& remountOptions : remountEntries) {
                auto& oldNode = remountOptions.node;
                auto& oldMountPoint = remountOptions.node->mountPoints[remountOptions.mountId];
                assert(oldMountPoint.storageNode != nullptr);
                unmount(*oldNode, *oldMountPoint.storageNode);

                StorageAcquisitionOptions acquireOptions;
                acquireOptions.volumeFilePath = remountOptions.mountOptions.volumeFilePath;
                acquireOptions.nodePath = remountOptions.mountOptions.nodePath;
                acquireOptions.hashTableParams = remountOptions.mountOptions.hashTableParams;
                auto acquireResult = m_storageRegistry->acquireStorage(acquireOptions);
                assert(acquireResult.isOk());
                if (!acquireResult.isOk()) {
                    return createMountResult(acquireResult);
                }
                oldMountPoint.storageNode = std::move(acquireResult.getRoot());
                mount(*oldNode, *oldMountPoint.storageNode, remountOptions.mountOptions.priority);
            }
        }

        return MountResult{MountResult::Status::kOk, 0};
    }

    MountResult VirtualStorage::createMountResult(const StorageAcquisitionResult& acquireResult)
    {
        MountResult::Status resultStatus = MountResult::Status::kOk;
        switch (acquireResult.getStatus()) {
            case StorageAcquisitionResult::Status::kVolumeLoadError:
                resultStatus = MountResult::Status::kVolumeLoadError;
                break;
            case StorageAcquisitionResult::Status::kStorageLoadError:
                resultStatus = MountResult::Status::kStorageLoadError;
                break;
            case StorageAcquisitionResult::Status::kOk: break;
        }
        return MountResult{resultStatus, 0};
    }
}
