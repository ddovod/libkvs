
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
        acquireOptions.volumeFilePath = "lh1";
        acquireOptions.volumeMountPath = "A";

        auto acquireResult = registry.acquireStorage(acquireOptions);
        CHECK(acquireResult.isOk());
        storagePtr = acquireResult.getStorage();

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
        acquireOptions.volumeFilePath = "lh1";
        acquireOptions.volumeMountPath = "A";

        auto acquireResult = registry.acquireStorage(acquireOptions);
        CHECK(acquireResult.isOk());

        acquireResult = registry.acquireStorage(acquireOptions);
        CHECK(acquireResult.isOk());

        kvs::StorageReleaseOptions releaseOptions;
        releaseOptions.storage = acquireResult.getStorage();
        auto releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(releaseResult.isOk());

        releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(releaseResult.isOk());

        releaseResult = registry.releaseStorage(releaseOptions);
        CHECK(!releaseResult.isOk());
        CHECK(releaseResult.getStatus() == kvs::StorageReleaseResult::Status::kStorageNotFound);
    }
}
