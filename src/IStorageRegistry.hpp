
#pragma once

#include <cassert>
#include <memory>
#include "HashTableParams.hpp"
#include "IStorageImpl.hpp"
#include "StorageNode.hpp"

namespace kvs
{
    struct StorageAcquisitionOptions
    {
        std::string volumeFilePath;
        std::string nodePath;
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

        StorageAcquisitionResult(std::unique_ptr<StorageNode>&& root, Status status)
            : m_root(std::move(root))
            , m_status(status)
        {
            if (m_root->storage == nullptr) {
                assert(m_status != Status::kOk);
            } else {
                assert(m_status == Status::kOk);
            }
        }

        bool isOk() const { return m_status == Status::kOk; }
        Status getStatus() const { return m_status; }
        std::unique_ptr<StorageNode>& getRoot() { return m_root; }

    private:
        std::unique_ptr<StorageNode> m_root;
        Status m_status = Status::kOk;
    };

    struct StorageReleaseOptions
    {
        IStorageImpl* storage = nullptr;
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
