
#include "VirtualStorage.hpp"
#include <memory>
#include "IStorage.hpp"
#include "IVirtualStorage.hpp"
#include "SimpleInMemoryStorage.hpp"
#include "Status.hpp"
#include "utility/StringUtils.hpp"

#include <iostream>

namespace kvs
{
    MountResult VirtualStorage::mount(const MountOptions& mountOptions)
    {
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
        node->storages.push_back(std::make_shared<SimpleInMemoryStorage>());

        return {};
    }

    Status VirtualStorage::getValue(const Key& key, Value* value)
    {
        if (const auto& storages = resolveNodes(key.getPath()); !storages.empty()) {
            for (const auto& storage : resolveNodes(key.getPath())) {
                auto status = storage->getValue(key, value);
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
        if (const auto& storages = resolveNodes(key.getPath()); !storages.empty()) {
            return storages.front()->putValue(key, value);
        }
        return Status{Status::FailReason::kNodeNotFound};
    }

    Status VirtualStorage::deleteValue(const Key& key)
    {
        const auto& storages = resolveNodes(key.getPath());
        if (storages.empty()) {
            return Status{Status::FailReason::kNodeNotFound};
        }

        for (const auto& storage : storages) {
            auto status = storage->deleteValue(key);
            if (status.isOk()) {
                return {};
            }
        }

        return Status{Status::FailReason::kKeyNotFound};
    }

    std::vector<std::shared_ptr<IStorage>> VirtualStorage::resolveNodes(const std::string& path)
    {
        Node* node = &m_rootNode;
        for (const auto& comp : split(path, "/")) {
            auto found = node->children.find(comp);
            if (found == node->children.end()) {
                return {};
            }
            node = found->second.get();
        }
        return node->storages;
    }
}
