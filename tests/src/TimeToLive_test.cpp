
#include <VirtualStorage.hpp>
#include <chrono>
#include <doctest.h>
#include <memory>
#include <thread>
#include "ICompositeStorage.hpp"
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
    int value1 = 849;

    auto result = storage->mount(options);
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
    ;
}
