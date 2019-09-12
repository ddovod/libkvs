
#include <VirtualStorage.hpp>
#include <doctest.h>
#include "ICompositeStorage.hpp"
#include "Status.hpp"
#include "Value.hpp"

TEST_CASE("Priorities")
{
    auto storage = std::make_unique<kvs::VirtualStorage>();

    kvs::MountOptions options;
    options.volumeFilePath = "test_data/priorities_volume.json";
    options.hashTableParams.maxRecordSizeBytes = 1024;

    auto doMount = [&](const std::string& source, const std::string& destination, int priority) {
        options.storageMountPath = destination;
        options.nodePath = source;
        options.priority = priority;
        auto result = storage->mount(options);
        CHECK(result.isOk());
        auto mountId = result.getMountId();

        options.storageMountPath = source;
        result = storage->mount(options);
        CHECK(result.isOk());

        return mountId;
    };

    const std::string key1 = "key_1";
    const std::string key2 = "key_2";

    const int value1 = 1;
    const int value2 = 2;
    const int value3 = 3;
    const int value4 = 4;

    kvs::Value value;
    kvs::Status status;

    auto mountIdAK = doMount("K", "A", -1);
    auto mountIdAL = doMount("L", "A", 0);
    auto mountIdAM = doMount("M", "A", 1);

    {
        status = storage->putValue({key1, "A"}, value1);
        CHECK(status.isOk());

        status = storage->getValue({key1, "A"}, &value);
        CHECK(status.isOk());
        CHECK(value.getValue<int>() == value1);

        status = storage->getValue({key1, "M"}, &value);
        CHECK(status.isOk());
        CHECK(value.getValue<int>() == value1);

        status = storage->getValue({key1, "K"}, &value);
        CHECK(status.getFailReason() == kvs::Status::FailReason::kKeyNotFound);

        status = storage->getValue({key1, "L"}, &value);
        CHECK(status.getFailReason() == kvs::Status::FailReason::kKeyNotFound);
    }

    {
        status = storage->putValue({key1, "L"}, value2);
        CHECK(status.isOk());

        status = storage->getValue({key1, "A"}, &value);
        CHECK(status.isOk());
        CHECK(value.getValue<int>() == value1);
    }

    {
        status = storage->putValue({key2, "L"}, value3);
        CHECK(status.isOk());

        status = storage->getValue({key2, "A"}, &value);
        CHECK(status.isOk());
        CHECK(value.getValue<int>() == value3);
    }

    {
        status = storage->deleteValue({key1, "A"});
        CHECK(status.isOk());

        status = storage->getValue({key1, "A"}, &value);
        CHECK(status.isOk());
        CHECK(value.getValue<int>() == value2);

        status = storage->deleteValue({key1, "A"});
        CHECK(status.isOk());

        status = storage->getValue({key1, "A"}, &value);
        CHECK(status.getFailReason() == kvs::Status::FailReason::kKeyNotFound);
    }

    {
        status = storage->putValue({key2, "A"}, value4);
        CHECK(status.isOk());

        status = storage->getValue({key2, "L"}, &value);
        CHECK(status.isOk());
        CHECK(value.getValue<int>() == value4);
    }

    auto unmountResult = storage->unmount({mountIdAL});
    CHECK(unmountResult.isOk());

    {
        status = storage->getValue({key2, "A"}, &value);
        CHECK(status.getFailReason() == kvs::Status::FailReason::kKeyNotFound);

        status = storage->putValue({key2, "A"}, value1);
        CHECK(status.isOk());

        status = storage->getValue({key2, "M"}, &value);
        CHECK(status.isOk());
        CHECK(value.getValue<int>() == value1);
    }
}
