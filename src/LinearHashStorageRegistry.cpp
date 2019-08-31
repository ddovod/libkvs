
#include "LinearHashStorageRegistry.hpp"
#include <cassert>
#include <memory>
#include "IStorageRegistry.hpp"
#include "LinearHashStorage.hpp"

namespace kvs
{
    StorageAcquisitionResult LinearHashStorageRegistry::acquireStorage(const StorageAcquisitionOptions& options)
    {
        auto key = options.volumeFilePath + ":" + options.volumeMountPath;
        auto found = m_storageMap.find(key);
        if (found == m_storageMap.end()) {
            StorageEntry newEntry;
            newEntry.storage = std::make_unique<LinearHashStorage>();
            newEntry.counter = 1;
            auto it = m_storageMap.insert({key, std::move(newEntry)});
            found = it.first;
            m_inverseStorageMap[found->second.storage.get()] = key;
        } else {
            assert(found->second.counter > 0);
            found->second.counter++;
        }

        return StorageAcquisitionResult{found->second.storage.get(), StorageAcquisitionResult::Status::kOk};
    }

    StorageReleaseResult LinearHashStorageRegistry::releaseStorage(const StorageReleaseOptions& options)
    {
        auto keyFound = m_inverseStorageMap.find(options.storage);
        if (keyFound == m_inverseStorageMap.end()) {
            return StorageReleaseResult{StorageReleaseResult::Status::kStorageNotFound};
        }

        auto found = m_storageMap.find(keyFound->second);
        assert(found != m_storageMap.end());
        found->second.counter--;
        assert(found->second.counter >= 0);
        if (found->second.counter == 0) {
            m_storageMap.erase(found);
            m_inverseStorageMap.erase(keyFound);
        }

        return StorageReleaseResult{StorageReleaseResult::Status::kOk};
    }
}
