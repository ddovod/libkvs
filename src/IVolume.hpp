
#pragma once

#include <string>
#include "HashTableParams.hpp"
#include "StorageNode.hpp"

namespace kvs
{
    struct LoadStorageOptions
    {
        std::string path;
        HashTableParams hashTableParams;
    };

    class LoadStorageResult
    {
    public:
        enum class Status
        {
            kOk,
        };

        LoadStorageResult(Status status, StorageNode root)
            : m_status(status)
            , m_root(std::move(root))
        {
        }

        bool isOk() const { return m_status == Status::kOk; }
        StorageNode& getRoot() { return m_root; }

    private:
        Status m_status = Status::kOk;
        StorageNode m_root;
    };

    struct UnloadStorageOptions
    {
        std::string path;
    };

    class UnloadStorageResult
    {
    public:
        enum class Status
        {
            kOk,
        };

        explicit UnloadStorageResult(Status status)
            : m_status(status)
        {
        }

        bool isOk() const { return m_status == Status::kOk; }

    private:
        Status m_status = Status::kOk;
    };

    class IVolume
    {
    public:
        virtual ~IVolume() = default;

        virtual bool isOk() const = 0;
        virtual LoadStorageResult loadStorage(const LoadStorageOptions& options) = 0;
        virtual UnloadStorageResult unloadStorage(const UnloadStorageOptions& options) = 0;
    };
}
