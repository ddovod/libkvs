
#include <VirtualStorage.hpp>
#include <chrono>
#include <doctest.h>
#include <memory>
#include <thread>
#include "ICompositeStorage.hpp"
#include "KeysRange.hpp"
#include "Status.hpp"
#include "Value.hpp"

TEST_CASE("Keys with TTL")
{
    auto storage = std::make_unique<kvs::VirtualStorage>();
    kvs::MountOptions options;
    options.volumeFilePath = "test_data/ttl_volume.json";
    options.hashTableParams.maxRecordSizeBytes = 1024;
    options.storageMountPath = "A";
    options.nodePath = "K";

    const std::string key1 = "key_1";
    const std::string key2 = "key_2";
    int value1 = 849;
    int value2 = 849;

    auto result = storage->mount(options);
    auto mountIdAK = result.getMountId();
    CHECK(result.isOk());

    auto status = storage->putValue({key1, "A", 10}, value1);
    CHECK(status.isOk());

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    kvs::Value value;
    status = storage->getValue({key1, "A"}, &value);
    CHECK(status.isOk());
    CHECK(value.getValue<int>() == value1);

    std::this_thread::sleep_for(std::chrono::milliseconds(7));
    status = storage->getValue({key1, "A"}, &value);
    CHECK(status.getFailReason() == kvs::Status::FailReason::kKeyNotFound);

    status = storage->putValue({key2, "A", 200}, value2);
    CHECK(status.isOk());

    status = storage->getValue({key2, "A"}, &value);
    CHECK(status.isOk());
    CHECK(value.getValue<int>() == value2);

    kvs::UnmountOptions unmountOptions;
    unmountOptions.mountId = mountIdAK;
    auto unmountResult = storage->unmount(unmountOptions);
    CHECK(unmountResult.isOk());

    result = storage->mount(options);
    CHECK(result.isOk());

    status = storage->getValue({key2, "A"}, &value);
    CHECK(status.isOk());
    CHECK(value.getValue<int>() == value2);

    std::this_thread::sleep_for(std::chrono::milliseconds(205));
    status = storage->getValue({key2, "A"}, &value);
    CHECK(status.getFailReason() == kvs::Status::FailReason::kKeyNotFound);

    kvs::KeysRange keysRange;
    status = storage->getKeysRange({"A", 0, 5}, &keysRange);
    CHECK(status.isOk());
    CHECK(keysRange.getKeys().empty());
}
