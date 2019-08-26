
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
        bool createVolumeIfMissing = true;
        int priority = 0;
    };

    class MountResult
    {
    public:
        bool isOk() const { return true; }
    };

    class IVirtualStorage : public IStorage
    {
    public:
        virtual ~IVirtualStorage() = default;

        virtual MountResult mount(const MountOptions& mountOptions) = 0;
    };
}
