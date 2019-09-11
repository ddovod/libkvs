
#pragma once

#include <memory>
#include <string>
#include "HashTableParams.hpp"
#include "IStorage.hpp"

namespace kvs
{
    struct MountOptions
    {
        std::string storageMountPath;
        std::string volumeFilePath;
        std::string nodePath;
        int priority = 0;
        HashTableParams hashTableParams;
    };

    class MountResult
    {
    public:
        enum class Status
        {
            kOk,
            kVolumeLoadError,
            kStorageLoadError,
        };

        explicit MountResult(Status status, uint64_t mountId)
            : m_status(status)
            , m_mountId(mountId)
        {
        }

        bool isOk() const { return m_status == Status::kOk; }
        Status getStatus() const { return m_status; }
        uint64_t getMountId() const { return m_mountId; }

    private:
        Status m_status = Status::kOk;
        uint64_t m_mountId;
    };

    struct UnmountOptions
    {
        uint64_t mountId;
    };

    class UnmountResult
    {
    public:
        enum class Status
        {
            kOk,
            kInvalidMountId,
        };

        explicit UnmountResult(Status status)
            : m_status(status)
        {
        }

        bool isOk() const { return m_status == Status::kOk; }

    private:
        Status m_status = Status::kOk;
    };

    class ICompositeStorage : public IStorage
    {
    public:
        virtual ~ICompositeStorage() = default;

        virtual MountResult mount(const MountOptions& mountOptions) = 0;
        virtual UnmountResult unmount(const UnmountOptions& unmountOptions) = 0;
    };
}
