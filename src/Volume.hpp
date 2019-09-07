
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "IStorage.hpp"
#include "LinearHashStorage.hpp"
#include "Node.hpp"
#include "StorageNode.hpp"

namespace kvs
{
    struct LoadStorageOptions
    {
        std::string path;
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

    class Volume
    {
    public:
        explicit Volume(const std::string& filepath);
        bool isOk() const;

        LoadStorageResult loadStorage(const LoadStorageOptions& options);
        UnloadStorageResult unloadStorage(const UnloadStorageOptions& options);

    private:
        struct TreeNode
        {
            std::unique_ptr<Node> node;
            std::unordered_map<std::string, std::unique_ptr<Node>> children;
        };
        TreeNode m_rootNode;
        bool m_loaded = false;
    };
}
