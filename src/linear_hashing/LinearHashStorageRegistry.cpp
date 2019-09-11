
#include "LinearHashStorageRegistry.hpp"
#include <cassert>
#include <cstdio>
#include <memory>
#include "IStorageRegistry.hpp"
#include "LinearHashStorage.hpp"
#include "LinearHashVolume.hpp"

namespace kvs
{
    StorageAcquisitionResult LinearHashStorageRegistry::acquireStorage(const StorageAcquisitionOptions& options)
    {
        auto found = m_volumes.find(options.volumeFilePath);
        if (found == m_volumes.end()) {
            auto volume = std::make_unique<LinearHashVolume>(options.volumeFilePath);
            if (!volume->isOk()) {
                return StorageAcquisitionResult{{}, StorageAcquisitionResult::Status::kVolumeLoadError};
            }
            found = m_volumes.emplace(options.volumeFilePath, std::move(volume)).first;
        }

        LoadStorageOptions loadOptions;
        loadOptions.path = options.nodePath;
        loadOptions.hashTableParams = options.hashTableParams;
        auto loadResult = found->second->loadStorage(loadOptions);
        if (!loadResult.isOk()) {
            return StorageAcquisitionResult{{}, StorageAcquisitionResult::Status::kStorageLoadError};
        }

        acquireStorage(*loadResult.getRoot(), options.volumeFilePath, options.nodePath);

        return StorageAcquisitionResult{std::move(loadResult.getRoot()), StorageAcquisitionResult::Status::kOk};
    }

    StorageReleaseResult LinearHashStorageRegistry::releaseStorage(const StorageReleaseOptions& options)
    {
        auto foundKey = m_storageKeys.find(options.storage);
        if (foundKey == m_storageKeys.end()) {
            return StorageReleaseResult{StorageReleaseResult::Status::kStorageNotFound};
        }

        auto counterFound = m_storageCounters.find(foundKey->second);
        assert(counterFound != m_storageCounters.end());

        counterFound->second.second--;
        assert(counterFound->second.second >= 0);

        if (counterFound->second.second == 0) {
            m_storageKeys.erase(foundKey);
            m_storageCounters.erase(counterFound);
        }

        return StorageReleaseResult{StorageReleaseResult::Status::kOk};
    }

    void LinearHashStorageRegistry::acquireStorage(StorageNode& storageNode,
        const std::string& volumeFilepath,
        const std::string& nodePath)
    {
        const auto key = volumeFilepath + ":" + nodePath;
        auto found = m_storageCounters.find(key);
        if (found == m_storageCounters.end()) {
            found = m_storageCounters.emplace(key, std::make_pair(storageNode.storage, 0)).first;
            m_storageKeys[storageNode.storage] = key;
        }
        found->second.second++;

        for (auto& child : storageNode.children) {
            acquireStorage(*child.second, volumeFilepath, nodePath + "/" + child.first);
        }
    }
}
