
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "IStorage.hpp"

namespace kvs
{
    struct StorageNode
    {
        IStorage* storage = nullptr;
        std::unordered_map<std::string, std::unique_ptr<StorageNode>> children;
    };
}
