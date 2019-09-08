
#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"
#include "LinearHashStorage.hpp"
#include "LinearHashVolume.hpp"
#include "StorageNode.hpp"

namespace kvs
{
    class LinearHashStorageRegistry : public IStorageRegistry
    {
    public:
        StorageAcquisitionResult acquireStorage(const StorageAcquisitionOptions& options) override;
        StorageReleaseResult releaseStorage(const StorageReleaseOptions& options) override;

    private:
        std::unordered_map<std::string, std::unique_ptr<IVolume>> m_volumes;
        std::unordered_map<std::string, std::pair<IStorage*, int>> m_storageCounters;
        std::unordered_map<IStorage*, std::string> m_storageKeys;

        void acquireStorage(StorageNode& storageNode, const std::string& volumeFilepath, const std::string& volumePath);
    };
}
