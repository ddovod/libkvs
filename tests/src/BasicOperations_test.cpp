
#include <doctest.h>
#include <memory>
#include "IStorage.hpp"
#include "IVirtualStorage.hpp"
#include "VirtualStorage.hpp"

TEST_CASE("Basic get/put")
{
    auto storage = std::make_unique<kvs::VirtualStorage>();

    kvs::MountOptions options;
    options.storageMountPath = "A";
    options.volumeFilePath = "basic_volume1.kvs";
    options.volumeMountPath = "K";
    auto result = storage->mount(options);
    CHECK(result.isOk());

    {
        const std::string& key = "key_int32_1";
        const std::string& path = "A";
        int32_t value = 12;
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kInt32);
        CHECK(retrievedValue.getValue<int32_t>() == value);
    }

    {
        const std::string& key = "key_string_1";
        const std::string& path = "A";
        std::string value = "string_value";
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kString);
        CHECK(retrievedValue.getValue<std::string>() == value);
    }
}