
#include <doctest.h>
#include <limits>
#include <memory>
#include "Data.hpp"
#include "IStorage.hpp"
#include "VirtualStorage.hpp"

TEST_CASE("Basic get/put")
{
    auto storage = std::make_unique<kvs::VirtualStorage>();

    kvs::MountOptions options;
    options.storageMountPath = "A";
    options.volumeFilePath = "test_data/basic_volume.json";
    options.volumePath = "K";
    options.hashTableParams.maxRecordSizeBytes = 1024;
    auto result = storage->mount(options);
    CHECK(result.isOk());

    SUBCASE("int32")
    {
        const std::string& key = "key_int32_1";
        const std::string& path = "A";
        int32_t value = -12;
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kInt32);
        CHECK(retrievedValue.getValue<int32_t>() == value);

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }

    SUBCASE("uint32")
    {
        const std::string& key = "key_uint32_1";
        const std::string& path = "A";
        uint32_t value = 34;
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kUInt32);
        CHECK(retrievedValue.getValue<uint32_t>() == value);

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }

    SUBCASE("int64")
    {
        const std::string& key = "key_int64_1";
        const std::string& path = "A";
        int64_t value = -1243;
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kInt64);
        CHECK(retrievedValue.getValue<int64_t>() == value);

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }

    SUBCASE("uint64")
    {
        const std::string& key = "key_uint64_1";
        const std::string& path = "A";
        uint64_t value = 3465;
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kUInt64);
        CHECK(retrievedValue.getValue<uint64_t>() == value);

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }

    SUBCASE("float")
    {
        const std::string& key = "key_float_1";
        const std::string& path = "A";
        float value = 549.043f;
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kFloat);
        CHECK(std::abs(retrievedValue.getValue<float>() - value) < std::numeric_limits<float>::epsilon());

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }

    SUBCASE("double")
    {
        const std::string& key = "key_double_1";
        const std::string& path = "A";
        double value = 54549.31043;
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kDouble);
        CHECK(std::abs(retrievedValue.getValue<double>() - value) < std::numeric_limits<double>::epsilon());

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }

    SUBCASE("string")
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

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }

    SUBCASE("Data")
    {
        const std::string& key = "key_data_1";
        const std::string& path = "A";
        kvs::Data value;
        value.buffer = {54, 95, 154, 24, 0, 248, 255, 99};
        auto status = storage->putValue({key, path}, {value});
        CHECK(status.isOk());

        kvs::Value retrievedValue;
        status = storage->getValue({key, path}, &retrievedValue);
        CHECK(status.isOk());
        CHECK(retrievedValue.getType() == kvs::ValueType::kData);
        CHECK(retrievedValue.getValue<kvs::Data>().buffer == value.buffer);

        status = storage->deleteValue({key, path});
        CHECK(status.isOk());
    }
}
