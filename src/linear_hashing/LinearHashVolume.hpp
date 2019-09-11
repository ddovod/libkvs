
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "HashTableParams.hpp"
#include "IStorageImpl.hpp"
#include "IVolume.hpp"
#include "Node.hpp"
#include "StorageNode.hpp"
#include "json.hpp"

namespace kvs
{
    class LinearHashVolume : public IVolume
    {
    public:
        explicit LinearHashVolume(const std::string& filepath);

        bool isOk() const override;
        LoadStorageResult loadStorage(const LoadStorageOptions& options) override;
        UnloadStorageResult unloadStorage(const UnloadStorageOptions& options) override;

    private:
        struct TreeNode
        {
            Node node;
            std::unique_ptr<IStorageImpl> storage;
            std::unordered_map<std::string, std::unique_ptr<TreeNode>> children;
        };

        std::string m_filepath;
        std::string m_volumeName;
        std::string m_parentPath;
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
