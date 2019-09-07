
#pragma once

#include <cassert>
#include <memory>
#include "IStorage.hpp"
#include "LinearHashStorage.hpp"
#include "StorageNode.hpp"

namespace kvs
{
    struct StorageAcquisitionOptions
    {
        std::string volumeFilePath;
        std::string volumePath;
        HashTableParams hashTableParams;
    };

    class StorageAcquisitionResult
    {
    public:
        enum class Status
        {
            kOk,
            kVolumeLoadError,
            kStorageLoadError,
        };

        StorageAcquisitionResult(StorageNode root, Status status)
            : m_root(std::move(root))
            , m_status(status)
        {
            if (m_root.storage == nullptr) {
                assert(m_status != Status::kOk);
            } else {
                assert(m_status == Status::kOk);
            }
        }

        bool isOk() const { return m_status == Status::kOk; }
        Status getStatus() const { return m_status; }
        StorageNode& getRoot() { return m_root; }

    private:
        StorageNode m_root;
        Status m_status = Status::kOk;
    };

    struct StorageReleaseOptions
    {
        IStorage* storage = nullptr;
    };

    class StorageReleaseResult
    {
    public:
        enum class Status
        {
            kOk,
            kStorageNotFound,
        };

        explicit StorageReleaseResult(Status status)
            : m_status(status)
        {
        }

        bool isOk() const { return m_status == Status::kOk; }
        Status getStatus() const { return m_status; }

    private:
        Status m_status = Status::kOk;
    };

    class IStorageRegistry
    {
    public:
        virtual ~IStorageRegistry() = default;

        virtual StorageAcquisitionResult acquireStorage(const StorageAcquisitionOptions& options) = 0;
        virtual StorageReleaseResult releaseStorage(const StorageReleaseOptions& options) = 0;
    };
}
