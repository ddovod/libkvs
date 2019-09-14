
#pragma once

#include <memory>
#include <string>
#include "HashTableParams.hpp"
#include "IStorage.hpp"

namespace kvs
{
    /** Options for mount operation. */
    struct MountOptions
    {
        std::string storageMountPath;    /** A virtual path* */
        std::string volumeFilePath;      /** A file path to the volume. */
        std::string nodePath;            /** A path of the node in the volume. */
        int priority = 0;                /** Priority of the node being mounted. */
        HashTableParams hashTableParams; /** Parameters of the hash table. */
    };

    /** A wrapper for the mount operation result. */
    class MountResult
    {
    public:
        /** Status of the operation. */
        enum class Status
        {
            kOk,               /** Everything is fine. */
            kVolumeLoadError,  /** Error during volume loading. */
            kStorageLoadError, /** Error during storage node loading. */
        };

        explicit MountResult(Status status, uint64_t mountId)
            : m_status(status)
            , m_mountId(mountId)
        {
        }

        /** Convinient method to check is everything is fine. */
        bool isOk() const { return m_status == Status::kOk; }

        /** Returns status of the operation. */
        Status getStatus() const { return m_status; }

        /**
         * Returns a unique id of the mount operation which
         * is used later to unmount the node.
         */
        uint64_t getMountId() const { return m_mountId; }

    private:
        Status m_status = Status::kOk;
        uint64_t m_mountId;
    };

    /** Options for unmount operation. */
    struct UnmountOptions
    {
        uint64_t mountId; /** Id obtained in `MountResult`. */
    };

    /** A wrapper for the unmount operation result. */
    class UnmountResult
    {
    public:
        /** Status of the operation. */
        enum class Status
        {
            kOk,             /** Everything is fine. */
            kInvalidMountId, /** Invalid mount id is used to unmount the node. */
        };

        explicit UnmountResult(Status status)
            : m_status(status)
        {
        }

        bool isOk() const { return m_status == Status::kOk; }
        Status getStatus() const { return m_status; }

    private:
        Status m_status = Status::kOk;
    };

    /**
     * A composite storage interface which can mount/unmount another
     * storages inside this one.
     */
    class ICompositeStorage : public IStorage
    {
    public:
        virtual ~ICompositeStorage() = default;

        virtual MountResult mount(const MountOptions& mountOptions) = 0;
        virtual UnmountResult unmount(const UnmountOptions& unmountOptions) = 0;
    };
}
