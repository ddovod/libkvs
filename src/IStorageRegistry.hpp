
#pragma once

#include <cassert>
#include <memory>
#include "HashTableParams.hpp"
#include "IStorageImpl.hpp"
#include "StorageNode.hpp"

namespace kvs
{
    /** Options for acquire operation. */
    struct StorageAcquisitionOptions
    {
        std::string volumeFilePath;      /** File path of the volume. */
        std::string nodePath;            /** Full path to the node. */
        HashTableParams hashTableParams; /** Parameters of the hash table. */
    };

    /** A wrapper for the acquire operation result. */
    class StorageAcquisitionResult
    {
    public:
        /** Status of the operation. */
        enum class Status
        {
            kOk,               /** Everything is fine. */
            kVolumeLoadError,  /** Error during volume loading. */
            kStorageLoadError, /** Error during storage node loading. */
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

    /** Options for release operation. */
    struct StorageReleaseOptions
    {
        IStorageImpl* storage = nullptr; /** Pointer to the storage to release. */
    };

    /** A wrapper for the release operation result. */
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

    /**
     * An interface of the storage registry.
     * Storage registry is a "storage of the storages".
     * It manages lifetime of the nodes an volumes
     * probably with reference counting under the hood.
     */
    class IStorageRegistry
    {
    public:
        virtual ~IStorageRegistry() = default;

        virtual StorageAcquisitionResult acquireStorage(const StorageAcquisitionOptions& options) = 0;
        virtual StorageReleaseResult releaseStorage(const StorageReleaseOptions& options) = 0;
    };
}
