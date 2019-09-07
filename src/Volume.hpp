
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "IStorage.hpp"
#include "LinearHashStorage.hpp"
#include "Node.hpp"
#include "StorageNode.hpp"
#include "json.hpp"

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
            Node node;
            std::unique_ptr<IStorage> storage;
            std::unordered_map<std::string, std::unique_ptr<TreeNode>> children;
        };

        std::string m_filepath;
        TreeNode m_rootNode;
        bool m_isLoaded = false;
        bool m_isDirty = false;

        void fromJson(const json& j, TreeNode& node);
        void toJson(json& j, const TreeNode& node);
        void saveNodes();
        TreeNode* findTreeNode(const std::string& path);
        void loadStorage(StorageNode& storageNode, TreeNode& treeNode, const HashTableParams& hashTableParams);
    };
}
