
#include "Volume.hpp"

namespace kvs
{
    Volume::Volume(const std::string& filepath) {}

    bool Volume::isOk() const { return m_loaded; }

    LoadStorageResult Volume::loadStorage(const LoadStorageOptions& options)
    {
        return LoadStorageResult{LoadStorageResult::Status::kOk, {}};
    }

    UnloadStorageResult Volume::unloadStorage(const UnloadStorageOptions& options)
    {
        return UnloadStorageResult{UnloadStorageResult::Status::kOk};
    }
}
