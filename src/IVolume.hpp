
#pragma once

#include <string>
#include "HashTableParams.hpp"
#include "StorageNode.hpp"

namespace kvs
{
    /** Options for load of the storage node operation. */
    struct LoadStorageOptions
    {
        std::string path;                /** Node path. */
        HashTableParams hashTableParams; /** Parameters of the hash table. */
    };

    /** A wrapper for the load operation result. */
    class LoadStorageResult
    {
    public:
        enum class Status
        {
            kOk,
        };

        LoadStorageResult(Status status, std::unique_ptr<StorageNode>&& root)
            : m_status(status)
            , m_root(std::move(root))
        {
        }

        bool isOk() const { return m_status == Status::kOk; }

        /** Returns the root node of the loaded subtree. */
        std::unique_ptr<StorageNode>& getRoot() { return m_root; }

    private:
        Status m_status = Status::kOk;
        std::unique_ptr<StorageNode> m_root;
    };

    /** Options for the unload operation. */
    struct UnloadStorageOptions
    {
        std::string path; /** Path to the node to unload. */
    };

    /** A wrapper for the unload operation result. */
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

    /** An interface of the volume. */
    class IVolume
    {
    public:
        virtual ~IVolume() = default;

        virtual bool isOk() const = 0;
        virtual LoadStorageResult loadStorage(const LoadStorageOptions& options) = 0;
        virtual UnloadStorageResult unloadStorage(const UnloadStorageOptions& options) = 0;
    };
}
