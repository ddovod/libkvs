
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "IStorageImpl.hpp"

namespace kvs
{
    struct StorageNode
    {
        IStorageImpl* storage = nullptr;
        std::unordered_map<std::string, std::unique_ptr<StorageNode>> children;
    };
}
