
#include "VirtualStorage.hpp"
#include "IStorage.hpp"
#include "IVirtualStorage.hpp"
#include "utility/StringUtils.hpp"

namespace kvs
{
    MountResult VirtualStorage::mount(const MountOptions& mountOptions)
    {
        MountResult result;
        (void)mountOptions;

        return result;
    }

    Status VirtualStorage::getValue(const Key& key, Value* value)
    {
        auto nodeStorage = resolveNode(key.getPath());
        if (!nodeStorage) {
            return Status{Status::FailReason::kNodeNotFound};
        }

        return nodeStorage->getValue(key, value);
    }

    Status VirtualStorage::putValue(const Key& key, const Value& value)
    {
        auto nodeStorage = resolveNode(key.getPath());
        if (!nodeStorage) {
            return Status{Status::FailReason::kNodeNotFound};
        }

        return nodeStorage->putValue(key, value);
    }

    Status VirtualStorage::deleteValue(const Key& key)
    {
        auto nodeStorage = resolveNode(key.getPath());
        if (!nodeStorage) {
            return Status{Status::FailReason::kNodeNotFound};
        }

        return nodeStorage->deleteValue(key);
    }

    std::shared_ptr<IStorage> VirtualStorage::resolveNode(const std::string& path)
    {
        Node* node = &m_rootNode;
        for (const auto& comp : split(path, "/")) {
            auto found = node->children.find(comp);
            if (found == node->children.end()) {
                return {};
            }
            node = found->second.get();
        }
        return node->storage;
    }
}
