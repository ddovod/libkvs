
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "IStorageImpl.hpp"

namespace kvs
{
    /** A subtree of nodes of the storage. */
    struct StorageNode
    {
        std::string volumeFilePath;      /** A path to the containing volume. */
        std::string nodePath;            /** A full path to this node in the volume. */
        IStorageImpl* storage = nullptr; /** A pointer to the storage associated with this node. */
        std::unordered_map<std::string, std::unique_ptr<StorageNode>> children; /** Children nodes. */
    };
}
