
#include <VirtualStorage.hpp>
#include <atomic>
#include <chrono>
#include <doctest.h>
#include <iostream>
#include <memory>
#include <thread>
#include "utility/RandomUtils.hpp"
#include "utility/StringUtils.hpp"

TEST_CASE("Simple multithreaded operations test")
{
    using namespace std::chrono;

    for (size_t i = 1; i <= 4; i++) {
        auto storage = std::make_unique<kvs::VirtualStorage>();

        kvs::MountOptions options;
        options.volumeFilePath = "test_data/mt_volume_" + std::to_string(i) + ".json";
        options.storageMountPath = "A";
        options.nodePath = "A";
        options.hashTableParams.maxRecordSizeBytes = 1024;
        options.hashTableParams.bucketCacheSizeBytes = 1024 * 1024;
        storage->mount(options);

        options.storageMountPath = "A/B";
        options.nodePath = "C/D";
        options.hashTableParams.maxRecordSizeBytes = 1024;
        storage->mount(options);

        options.storageMountPath = "A/B/C";
        options.nodePath = "E/F/G";
        options.hashTableParams.maxRecordSizeBytes = 1024;
        storage->mount(options);

        const std::vector<std::string> storagePaths = {
            "A",
            "A/B",
            "A/B/C",
        };

        std::atomic<int> totalReadOps = 0;
        std::atomic<int> totalWriteOps = 0;
        std::atomic<int> totalDeleteOps = 0;
        auto threadFunc = [&] {
            auto start = high_resolution_clock::now();
            int testLengthMs = 5000;
            while (duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < testLengthMs) {
                const std::string key = kvs::generateString(5);
                const std::string& path = storagePaths[kvs::utility::random(storagePaths.size())];
                auto rndValue = kvs::utility::random(100);
                if (rndValue < 6) {
                    kvs::Data value;
                    value.buffer.resize(100);
                    storage->putValue({key, path}, {value});
                    totalWriteOps++;
                } else if (rndValue < 11) {
                    storage->deleteValue({key, path});
                    totalDeleteOps++;
                } else {
                    kvs::Value retrievedValue;
                    storage->getValue({key, path}, &retrievedValue);
                    totalReadOps++;
                }
            }
        };

        std::cout << "Starting test with " << i << " threads: 8/9 reads, 1/9 writes-deletes" << std::endl;
        std::vector<std::unique_ptr<std::thread>> threads;
        totalReadOps = 0;
        totalWriteOps = 0;
        totalDeleteOps = 0;
        for (int i = 0; i < 4; i++) {
            threads.push_back(std::make_unique<std::thread>(threadFunc));
        }
        for (auto& t : threads) {
            t->join();
        }

        std::cout << "Done " << i << ": ";
        std::cout << (totalReadOps + totalWriteOps + totalDeleteOps) / 5 << " -> " << totalReadOps / 5 << "r/"
                  << totalWriteOps / 5 << "w/" << totalDeleteOps / 5 << "d" << std::endl;
    }
}
