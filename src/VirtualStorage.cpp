
#include "VirtualStorage.hpp"
#include <algorithm>
#include <cassert>
#include <memory>
#include "ICompositeStorage.hpp"
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"
#include "LinearHashStorageRegistry.hpp"
#include "SimpleInMemoryStorage.hpp"
#include "Status.hpp"
#include "utility/StringUtils.hpp"

#include <iostream>

namespace kvs
{
    VirtualStorage::VirtualStorage()
        : m_storageRegistry(std::make_unique<LinearHashStorageRegistry>())
    {
    }

    MountResult VirtualStorage::mount(const MountOptions& mountOptions)
    {
        StorageAcquisitionOptions acquireOptions;
        acquireOptions.volumeFilePath = mountOptions.volumeFilePath;
        acquireOptions.volumePath = mountOptions.volumePath;
        acquireOptions.hashTableParams = mountOptions.hashTableParams;
        auto result = m_storageRegistry->acquireStorage(acquireOptions);
        if (!result.isOk()) {
            MountResult::Status resultStatus = MountResult::Status::kOk;
            switch (result.getStatus()) {
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

        auto storageNode = std::move(result.getRoot());

        Node* node = &m_rootNode;
        for (const auto& comp : split(mountOptions.storageMountPath, "/")) {
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

        auto mountId = m_mountIdCounter++;
        mount(*node, storageNode, mountOptions.priority);
        node->mountPoints[mountId] = std::move(storageNode);
        m_mountPoints[mountId] = node;

        return MountResult{MountResult::Status::kOk, mountId};
    }

    void VirtualStorage::mount(Node& node, StorageNode& storageNode, int priority)
    {
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
            mount(*found->second.get(), *child.second.get(), priority);
        }
    }

    UnmountResult VirtualStorage::unmount(const UnmountOptions& unmountOptions)
    {
        auto foundNode = m_mountPoints.find(unmountOptions.mountId);
        if (foundNode == m_mountPoints.end()) {
            return UnmountResult(UnmountResult::Status::kInvalidMountId);
        }

        auto foundStorageNode = foundNode->second->mountPoints.find(unmountOptions.mountId);
        assert(foundStorageNode != foundNode->second->mountPoints.end());

        unmount(*foundNode->second, foundStorageNode->second);

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

        node.storages.erase(foundStorage);
        for (auto& child : storageNode.children) {
            auto foundNode = node.children.find(child.first);
            assert(foundNode != node.children.end());
            unmount(*foundNode->second.get(), *child.second.get());
        }
    }

    Status VirtualStorage::getValue(const Key& key, Value* value)
    {
        if (auto node = resolveNode(key.getPath()); node && !node->storages.empty()) {
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
        if (auto node = resolveNode(key.getPath()); node && !node->storages.empty()) {
            return node->storages.front().storage->putValue(key, value);
        }
        return Status{Status::FailReason::kNodeNotFound};
    }

    Status VirtualStorage::deleteValue(const Key& key)
    {
        auto node = resolveNode(key.getPath());
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

    VirtualStorage::Node* VirtualStorage::resolveNode(const std::string& path)
    {
        Node* node = &m_rootNode;
        for (const auto& comp : split(path, "/")) {
            auto found = node->children.find(comp);
            if (found == node->children.end()) {
                return {};
            }
            node = found->second.get();
        }
        return node;
    }
}
