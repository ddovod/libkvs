
#include "LinearHashVolume.hpp"
#include <cassert>
#include <fstream>
#include <ios>
#include <json.hpp>
#include <memory>
#include "LinearHashStorage.hpp"
#include "StorageNode.hpp"
#include "utility/FilesystemUtils.hpp"
#include "utility/StringUtils.hpp"

namespace kvs
{
    LinearHashVolume::LinearHashVolume(const std::string& filepath)
    {
        m_filepath = filepath;
        m_volumeName = path{filepath}.filename();

        auto parentPath = path{filepath}.parent_path();
        if (parentPath.string() != filepath) {
            m_parentPath = parentPath.string();
            if (!parentPath.exists()) {
                mkdirp(getcwd(), parentPath);
            }
        }

        std::fstream volumeFile{filepath, std::fstream::in | std::fstream::out | std::fstream::trunc};
        if (!volumeFile.is_open()) {
            m_isLoaded = false;
            return;
        }

        std::string volumeString((std::istreambuf_iterator<char>(volumeFile)), std::istreambuf_iterator<char>());

        if (!volumeString.empty()) {
            json nodesJson;
            volumeFile >> nodesJson;

            if (nodesJson.is_object()) {
                fromJson(nodesJson, m_rootNode);
            }
        }

        m_isLoaded = true;
    }

    bool LinearHashVolume::isOk() const { return m_isLoaded; }

    LoadStorageResult LinearHashVolume::loadStorage(const LoadStorageOptions& options)
    {
        auto treeNode = findTreeNode(options.path);
        assert(treeNode != nullptr);

        auto resultNode = std::make_unique<StorageNode>();
        resultNode->volumeFilePath = m_filepath;
        resultNode->nodePath = options.path;
        loadStorage(*resultNode, *treeNode, options.hashTableParams);

        saveNodes();

        return LoadStorageResult{LoadStorageResult::Status::kOk, std::move(resultNode)};
    }

    void LinearHashVolume::loadStorage(StorageNode& storageNode,
        TreeNode& treeNode,
        const HashTableParams& hashTableParams)
    {
        if (!treeNode.storage) {
            LinearHashStorageParams storageParams;
            storageParams.hashTableParams = hashTableParams;
            storageParams.primaryFilepath = treeNode.node.primaryFilepath;
            storageParams.overflowFilepath = treeNode.node.overflowFilepath;
            treeNode.storage = std::make_unique<LinearHashStorage>(storageParams);
        }

        storageNode.storage = treeNode.storage.get();
        for (auto& child : treeNode.children) {
            auto childNode = std::make_unique<StorageNode>();
            childNode->volumeFilePath = storageNode.volumeFilePath;
            childNode->nodePath = storageNode.nodePath + "/" + child.first;
            loadStorage(*childNode, *child.second, hashTableParams);
            storageNode.children[child.first] = std::move(childNode);
        }
    }

    UnloadStorageResult LinearHashVolume::unloadStorage(const UnloadStorageOptions& options)
    {
        auto treeNode = findTreeNode(options.path);
        assert(treeNode != nullptr);

        treeNode->storage.reset();

        return UnloadStorageResult{UnloadStorageResult::Status::kOk};
    }

    void LinearHashVolume::fromJson(const json& j, TreeNode& node)
    {
        node.node.primaryFilepath = j["node"]["primaryFilepath"];
        node.node.overflowFilepath = j["node"]["overflowFilepath"];
        for (const auto& child : j["children"]) {
            const std::string childKey = child["key"];
            const std::string childValue = child["value"];

            auto childNode = std::make_unique<TreeNode>();
            fromJson(childValue, *childNode);
            node.children[childKey] = std::move(childNode);
        }
    }

    void LinearHashVolume::toJson(json& j, const TreeNode& node)
    {
        j["node"]["primaryFilepath"] = node.node.primaryFilepath;
        j["node"]["overflowFilepath"] = node.node.overflowFilepath;
        for (const auto& child : node.children) {
            json childJson;
            toJson(childJson, *child.second);

            json pairJson;
            pairJson["key"] = child.first;
            pairJson["value"] = childJson;
            j["children"].push_back(pairJson);
        }
    }

    void LinearHashVolume::saveNodes()
    {
        assert(m_isLoaded);
        if (!m_isDirty) {
            return;
        }

        std::ofstream volumeFile{m_filepath};
        assert(volumeFile.is_open());

        json volumeJson;
        toJson(volumeJson, m_rootNode);

        volumeFile << volumeJson;

        m_isDirty = false;
    }

    LinearHashVolume::TreeNode* LinearHashVolume::findTreeNode(const std::string& path)
    {
        TreeNode* node = &m_rootNode;
        std::string name;
        for (const auto& comp : split(path, "/")) {
            if (!name.empty()) {
                name.append("_");
            }
            name.append(comp);
            auto found = node->children.find(comp);
            if (found == node->children.end()) {
                auto newNode = std::make_unique<TreeNode>();
                newNode->node.primaryFilepath =
                    (kvs::path(m_parentPath) / kvs::path(m_volumeName + "_primary_" + name)).string();
                newNode->node.overflowFilepath =
                    (kvs::path(m_parentPath) / kvs::path(m_volumeName + "_overflow_" + name)).string();
                found = node->children.emplace(comp, std::move(newNode)).first;
                m_isDirty = true;
            }
            node = found->second.get();
        }
        return node;
    }
}
