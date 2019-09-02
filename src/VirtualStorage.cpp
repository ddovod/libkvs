
#include "VirtualStorage.hpp"
#include <algorithm>
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
        acquireOptions.volumeMountPath = mountOptions.volumeMountPath;
        auto result = m_storageRegistry->acquireStorage(acquireOptions);
        if (!result.isOk()) {
            MountResult::Status resultStatus;
            switch (result.getStatus()) {
                case StorageAcquisitionResult::Status::kFileNotFound:
                    resultStatus = MountResult::Status::kFileNotFound;
                    break;
                case StorageAcquisitionResult::Status::kNodeNotFound:
                    resultStatus = MountResult::Status::kNodeNotFound;
                    break;
                default: resultStatus = MountResult::Status::kUnknownError; break;
            }
            return MountResult{resultStatus};
        }

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

        MountedStorage newStorage;
        newStorage.storage = result.getStorage();
        newStorage.priority = mountOptions.priority;
        auto ub = std::upper_bound(
            node->storages.begin(), node->storages.end(), newStorage, [](const auto& l, const auto& r) {
                return l.priority > r.priority;
            });
        node->storages.insert(ub, newStorage);

        return MountResult{MountResult::Status::kOk};
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
