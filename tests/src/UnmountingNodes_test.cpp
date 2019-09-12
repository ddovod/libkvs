
#include <doctest.h>
#include <memory>
#include "ICompositeStorage.hpp"
#include "Status.hpp"
#include "Value.hpp"
#include "VirtualStorage.hpp"

TEST_CASE("Unmount")
{
    auto storage = std::make_unique<kvs::VirtualStorage>();
    kvs::MountOptions options;
    options.volumeFilePath = "test_data/unmount_volume.json";
    options.hashTableParams.maxRecordSizeBytes = 1024;

    const std::string key1 = "key_1";
    const std::string key2 = "key_2";
    const std::string key3 = "key_3";
    const std::string key4 = "key_4";

    int value1 = 756;
    int value2 = 834;
    int value3 = 950;
    int value4 = 285;

    options.storageMountPath = "A";
    options.nodePath = "K";
    auto result = storage->mount(options);
    CHECK(result.isOk());

    auto status = storage->putValue({key1, "A"}, value1);
    CHECK(status.isOk());

    options.storageMountPath = "A";
    options.nodePath = "L";
    result = storage->mount(options);
    auto mountIdAL = result.getMountId();
    CHECK(result.isOk());

    options.storageMountPath = "B";
    options.nodePath = "L";
    result = storage->mount(options);
    auto mountIdBL = result.getMountId();
    CHECK(result.isOk());

    status = storage->putValue({key2, "B"}, value2);
    CHECK(status.isOk());

    kvs::Value value;
    status = storage->getValue({key1, "A"}, &value);
    CHECK(status.isOk());
    CHECK(value.getValue<int>() == value1);

    status = storage->getValue({key2, "A"}, &value);
    CHECK(status.isOk());
    CHECK(value.getValue<int>() == value2);

    kvs::UnmountOptions unmountOptions;
    unmountOptions.mountId = mountIdBL;
    auto unmountResult = storage->unmount(unmountOptions);
    CHECK(unmountResult.isOk());

    status = storage->getValue({key2, "B"}, &value);
    CHECK(status.getFailReason() == kvs::Status::FailReason::kNodeNotFound);

    status = storage->getValue({key2, "A"}, &value);
    CHECK(status.isOk());
    CHECK(value.getValue<int>() == value2);

    unmountOptions.mountId = mountIdAL;
    unmountResult = storage->unmount(unmountOptions);
    CHECK(unmountResult.isOk());

    status = storage->getValue({key2, "A"}, &value);
    CHECK(status.getFailReason() == kvs::Status::FailReason::kKeyNotFound);

    status = storage->getValue({key1, "A"}, &value);
    CHECK(status.isOk());
    CHECK(value.getValue<int>() == value1);

    unmountOptions.mountId = mountIdAL;
    unmountResult = storage->unmount(unmountOptions);
    CHECK(unmountResult.getStatus() == kvs::UnmountResult::Status::kInvalidMountId);
}
