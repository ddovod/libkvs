
#pragma once

#include <memory>
#include <unordered_set>
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"
#include "LinearHashStorage.hpp"

namespace kvs
{
    class LinearHashStorageRegistry : public IStorageRegistry
    {
    public:
        StorageAcquisitionResult acquireStorage(const StorageAcquisitionOptions& options) override;
        StorageReleaseResult releaseStorage(const StorageReleaseOptions& options) override;

    private:
        struct StorageEntry
        {
            std::unique_ptr<IStorage> storage;
            int counter = 0;
        };

        std::unordered_map<std::string, StorageEntry> m_storageMap;
        std::unordered_map<IStorage*, std::string> m_inverseStorageMap;
    };
}
