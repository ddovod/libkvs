
#include <VirtualStorage.hpp>
#include <doctest.h>
#include <memory>
#include <unordered_set>
#include "ICompositeStorage.hpp"
#include "KeysRange.hpp"
#include "Value.hpp"
#include "utility/StringUtils.hpp"

TEST_CASE("Key range query")
{
    auto storage = std::make_unique<kvs::VirtualStorage>();

    kvs::MountOptions options;
    options.hashTableParams.maxRecordSizeBytes = 1024;
    options.volumeFilePath = "test_data/key_range_volume.json";

    options.storageMountPath = "A";
    options.nodePath = "A";
    auto result = storage->mount(options);
    CHECK(result.isOk());

    std::unordered_map<std::string, std::string> dataA;
    for (size_t i = 0; i < 10; i++) {
        dataA["key_" + std::to_string(i)] = kvs::utility::generateString();
    }
    for (const auto& el : dataA) {
        auto status = storage->putValue({el.first, "A"}, {el.second});
        CHECK(status.isOk());
    }

    kvs::KeysRange keysRange;
    auto status = storage->getKeysRange({"A", 0, 10}, &keysRange);
    CHECK(status.isOk());
    std::unordered_set<std::string> keysSetA;
    for (const auto& key : keysRange.getKeys()) {
        auto found = dataA.find(key);
        CHECK(found != dataA.end());
        CHECK(keysSetA.find(key) == keysSetA.end());
        keysSetA.insert(key);

        kvs::Value val;
        status = storage->getValue({key, "A"}, &val);
        CHECK(status.isOk());
        CHECK(val.getValue<std::string>() == found->second);
    }
    CHECK(keysSetA.size() == keysRange.getKeys().size());

    options.storageMountPath = "A";
    options.nodePath = "B";
    result = storage->mount(options);
    CHECK(result.isOk());

    options.storageMountPath = "B";
    options.nodePath = "B";
    result = storage->mount(options);
    CHECK(result.isOk());

    std::unordered_map<std::string, std::string> dataB;
    for (size_t i = 7; i < 16; i++) {
        dataB["key_" + std::to_string(i)] = kvs::utility::generateString();
    }
    for (const auto& el : dataB) {
        status = storage->putValue({el.first, "B"}, {el.second});
        CHECK(status.isOk());
    }

    status = storage->getKeysRange({"A", 5, 5}, &keysRange);
    CHECK(status.isOk());
    for (const auto& key : keysRange.getKeys()) {
        auto found = dataA.find(key);
        CHECK(found != dataA.end());

        kvs::Value val;
        status = storage->getValue({key, "A"}, &val);
        CHECK(status.isOk());
        CHECK(val.getValue<std::string>() == found->second);
    }

    status = storage->getKeysRange({"A", 7, 10}, &keysRange);
    CHECK(status.isOk());
    for (const auto& key : keysRange.getKeys()) {
        kvs::Value val;
        status = storage->getValue({key, "A"}, &val);
        CHECK(status.isOk());

        auto foundA = dataA.find(key);
        if (foundA == dataA.end()) {
            auto foundB = dataB.find(key);
            CHECK(foundB != dataB.end());
            CHECK(val.getValue<std::string>() == foundB->second);
        } else {
            CHECK(val.getValue<std::string>() == foundA->second);
        }
    }

    options.storageMountPath = "A";
    options.nodePath = "C";
    options.priority = 1;
    result = storage->mount(options);
    CHECK(result.isOk());

    options.storageMountPath = "C";
    options.nodePath = "C";
    result = storage->mount(options);
    CHECK(result.isOk());

    std::unordered_map<std::string, std::string> dataC;
    for (size_t i = 0; i < 20; i++) {
        dataC["key_" + std::to_string(i)] = kvs::utility::generateString();
    }
    for (const auto& el : dataC) {
        status = storage->putValue({el.first, "C"}, {el.second});
        CHECK(status.isOk());
    }

    status = storage->getKeysRange({"A", 0, 30}, &keysRange);
    CHECK(status.isOk());
    std::unordered_set<std::string> keysSetC;
    for (const auto& key : keysRange.getKeys()) {
        auto found = dataC.find(key);
        CHECK(found != dataC.end());
        CHECK(keysSetC.find(key) == keysSetC.end());
        keysSetC.insert(key);

        kvs::Value val;
        status = storage->getValue({key, "A"}, &val);
        CHECK(status.isOk());
        CHECK(val.getValue<std::string>() == found->second);
    }
    CHECK(keysSetC.size() == keysRange.getKeys().size());
}
