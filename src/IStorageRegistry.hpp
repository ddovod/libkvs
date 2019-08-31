
#pragma once

#include <cassert>
#include <memory>
#include "IStorage.hpp"

namespace kvs
{
    struct StorageAcquisitionOptions
    {
        std::string volumeFilePath;
        std::string volumeMountPath;
    };

    class StorageAcquisitionResult
    {
    public:
        enum class Status
        {
            kOk,
            kFileNotFound,
            kNodeNotFound,
        };

        StorageAcquisitionResult(IStorage* storage, Status status)
            : m_storage(storage)
            , m_status(status)
        {
            if (m_storage == nullptr) {
                assert(m_status != Status::kOk);
            } else {
                assert(m_status == Status::kOk);
            }
        }

        bool isOk() const { return m_status == Status::kOk; }
        Status getStatus() const { return m_status; }
        IStorage* getStorage() const { return m_storage; }

    private:
        IStorage* m_storage = nullptr;
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
