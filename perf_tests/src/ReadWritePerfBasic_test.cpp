
#include <chrono>
#include <doctest.h>
#include <iostream>
#include "Data.hpp"
#include "VirtualStorage.hpp"
#include "utility/StringUtils.hpp"

TEST_CASE("Read/write performance basic")
{
    using namespace std::chrono;

    auto storage = std::make_unique<kvs::VirtualStorage>();

    kvs::MountOptions options;
    options.storageMountPath = "A";
    options.volumeFilePath = "test_data/read_write_basic.json";
    options.volumePath = "P";
    options.hashTableParams.maxRecordSizeBytes = 1024;
    auto result = storage->mount(options);
    CHECK(result.isOk());

    std::cout << "Running simple perf test (avg reads and writes of 100 bytes with 8/1 ratio per second)" << std::endl;
    auto start = high_resolution_clock::now();
    int testLengthMs = 5000;
    int totalReadOps = 0;
    int totalWriteOps = 0;
    const std::string path = "A";
    while (duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < testLengthMs) {
        const std::string key = kvs::generateString(15);
        if (rand() % 100 < 11) {  // approx 1/9 writes
            kvs::Data value;
            value.buffer.resize(100);
            storage->putValue({key, path}, {value});
            totalWriteOps++;
        } else {
            kvs::Value retrievedValue;
            storage->getValue({key, path}, &retrievedValue);
            totalReadOps++;
        }
    }

    totalReadOps /= 5;
    totalWriteOps /= 5;
    std::cout << "Done (r/w): " << totalReadOps + totalWriteOps << "(" << totalReadOps << "/" << totalWriteOps
              << ") per sec" << std::endl;
}
