
#include <doctest.h>
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"
#include "linear_hashing/LinearHashStorageRegistry.hpp"

TEST_CASE("Basic LinearHashStorageRegistry")
{
    kvs::LinearHashStorageRegistry registry;
    kvs::StorageAcquisitionOptions acquireOptions;
    acquireOptions.volumeFilePath = "test_data/linear_hash_volume.json";
    acquireOptions.volumePath = "A";
    acquireOptions.hashTableParams.maxRecordSizeBytes = 1024;

    kvs::IStorageImpl* storagePtr = nullptr;
    {
        auto acquireResult = registry.acquireStorage(acquireOptions);
        CHECK(acquireResult.isOk());
        storagePtr = acquireResult.getRoot().storage;

        kvs::StorageReleaseOptions releaseOptions;
        releaseOptions.storage = storagePtr;
        auto releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(releaseResult.isOk());
    }

    {
        kvs::StorageReleaseOptions releaseOptions;
        releaseOptions.storage = storagePtr;
        auto releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(!releaseResult.isOk());
        CHECK(releaseResult.getStatus() == kvs::StorageReleaseResult::Status::kStorageNotFound);
    }

    {
        auto acquireResult = registry.acquireStorage(acquireOptions);
        CHECK(acquireResult.isOk());

        acquireResult = registry.acquireStorage(acquireOptions);
        CHECK(acquireResult.isOk());

        kvs::StorageReleaseOptions releaseOptions;
        releaseOptions.storage = acquireResult.getRoot().storage;
        auto releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(releaseResult.isOk());

        releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(releaseResult.isOk());

        releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(!releaseResult.isOk());
        CHECK(releaseResult.getStatus() == kvs::StorageReleaseResult::Status::kStorageNotFound);
    }
}
