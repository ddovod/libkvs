
#pragma once

#include <memory>
#include <string>
#include "IStorage.hpp"

namespace kvs
{
    struct MountOptions
    {
        std::string storageMountPath;
        std::string volumeFilePath;
        std::string volumeMountPath;
        int priority = 0;
    };

    class MountResult
    {
    public:
        enum class Status
        {
            kOk,
            kFileNotFound,
            kNodeNotFound,
            kUnknownError,
        };

        explicit MountResult(Status status)
            : m_status(status)
        {
        }

        bool isOk() const { return m_status == Status::kOk; }
        Status getStatus() const { return m_status; }

    private:
        Status m_status = Status::kOk;
    };

    class ICompositeStorage
    {
    public:
        virtual ~ICompositeStorage() = default;

        virtual MountResult mount(const MountOptions& mountOptions) = 0;
    };
}
