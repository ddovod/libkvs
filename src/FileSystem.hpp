
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace kvs
{
    struct VolumeFile
    {
        struct NodeInfo
        {
            std::string uuid;
            std::string dataFilePath;
            std::string indexFilePath;
            std::unordered_map<std::string, std::unique_ptr<NodeInfo>> children;
        };

        NodeInfo rootNode;
    };

    struct VolumeLockFile
    {
    };

    struct NodeDataFile
    {
    };

    struct NodeIndexFile
    {
    };

    struct ReadOptions
    {
        std::string filePath;
    };

    template <typename T>
    struct ReadResult
    {
        std::unique_ptr<T> data;
    };

    class FileSystem
    {
    public:
        static ReadResult<VolumeFile> readVolume(const ReadOptions& options);
    };
}
