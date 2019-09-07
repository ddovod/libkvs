
#include <doctest.h>
#include "IStorage.hpp"
#include "IStorageRegistry.hpp"
#include "LinearHashStorageRegistry.hpp"

TEST_CASE("Basic LinearHashStorageRegistry")
{
    kvs::LinearHashStorageRegistry registry;

    kvs::IStorage* storagePtr = nullptr;
    {
        kvs::StorageAcquisitionOptions acquireOptions;
        acquireOptions.volumeFilePath = "test_data/lh1.json";
        acquireOptions.volumePath = "A";

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
        kvs::StorageAcquisitionOptions acquireOptions;
        acquireOptions.volumeFilePath = "test_data/lh1";
        acquireOptions.volumePath = "A";

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
