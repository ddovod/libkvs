
#include <doctest.h>
#include "Value.hpp"
#include "ValueType.hpp"
#include "VirtualStorage.hpp"

TEST_CASE("Mount")
{
    auto storage = std::make_unique<kvs::VirtualStorage>();
    kvs::MountOptions options;
    options.volumeFilePath = "test_data/mount_volume.json";
    options.hashTableParams.maxRecordSizeBytes = 1024;

    const std::string key1 = "key_1";
    const std::string key2 = "key_2";
    const std::string key3 = "key_3";
    const std::string key4 = "key_4";

    int value1 = 123;
    int value2 = 234;
    int value4 = 543;

    options.storageMountPath = "A/A";
    options.nodePath = "K/L";
    auto result = storage->mount(options);
    CHECK(result.isOk());

    options.storageMountPath = "A/B";
    options.nodePath = "K/L";
    result = storage->mount(options);
    CHECK(result.isOk());

    auto status = storage->putValue({key1, "A/A"}, {value1});
    CHECK(status.isOk());

    kvs::Value retrievedValue;
    status = storage->getValue({key1, "A/B"}, &retrievedValue);
    CHECK(status.isOk());
    CHECK(retrievedValue.getType() == kvs::ValueType::kInt32);
    CHECK(retrievedValue.getValue<int32_t>() == value1);

    options.storageMountPath = "A/C";
    options.nodePath = "K";
    result = storage->mount(options);
    CHECK(result.isOk());

    options.storageMountPath = "A/D";
    options.nodePath = "K";
    result = storage->mount(options);
    CHECK(result.isOk());

    status = storage->putValue({key2, "A/C/L"}, {value2});
    CHECK(status.isOk());

    status = storage->getValue({key2, "A/D/L"}, &retrievedValue);
    CHECK(status.isOk());
    CHECK(retrievedValue.getType() == kvs::ValueType::kInt32);
    CHECK(retrievedValue.getValue<int32_t>() == value2);

    status = storage->getValue({key2, "A/B"}, &retrievedValue);
    CHECK(status.isOk());
    CHECK(retrievedValue.getValue<int32_t>() == value2);

    status = storage->getValue({key1, "A/C/L"}, &retrievedValue);
    CHECK(status.isOk());
    CHECK(retrievedValue.getValue<int32_t>() == value1);

    status = storage->getValue({key3, "A/B"}, &retrievedValue);
    CHECK(!status.isOk());

    options.storageMountPath = "A/B/C/D";
    options.nodePath = "K/L/M";
    result = storage->mount(options);
    CHECK(result.isOk());

    status = storage->putValue({key4, "A/B/C/D"}, {value4});
    CHECK(status.isOk());

    status = storage->getValue({key4, "A/A/M"}, &retrievedValue);
    CHECK(status.isOk());
    CHECK(retrievedValue.getValue<int32_t>() == value4);

    status = storage->getValue({key4, "A/C/L/M"}, &retrievedValue);
    CHECK(status.isOk());
    CHECK(retrievedValue.getValue<int32_t>() == value4);
}
