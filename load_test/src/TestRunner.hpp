
#pragma once

#include <VirtualStorage.hpp>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

struct TestOptions
{
    size_t threadsCount = 0;
    size_t cacheSize = 0;
    size_t recordSize = 0;
    size_t dataSize = 0;
    size_t maxNodes = 0;
    size_t readsWeight = 0;
    size_t writesWeight = 0;
    size_t deletesWeight = 0;
    size_t mountsWeight = 0;
    size_t unmountsWeight = 0;
    size_t krqsWeight = 0;
    size_t testTime = 0;
    size_t windowSize = 0;
    size_t keyLength = 0;
    uint32_t rndSeed;
};

struct TestResult
{
    struct Sample
    {
        size_t totalOps = 0;
        size_t readOps = 0;
        size_t writeOps = 0;
        size_t deleteOps = 0;
        size_t mountOps = 0;
        size_t unmountOps = 0;
        size_t keyRangeQueryOps = 0;
    };

    struct SampleCounters
    {
        std::atomic<size_t> totalOps = 0;
        std::atomic<size_t> readOps = 0;
        std::atomic<size_t> writeOps = 0;
        std::atomic<size_t> deleteOps = 0;
        std::atomic<size_t> mountOps = 0;
        std::atomic<size_t> unmountOps = 0;
        std::atomic<size_t> keyRangeQueryOps = 0;

        Sample reset()
        {
            Sample sample;
            sample.totalOps = totalOps.exchange(0);
            sample.readOps = readOps.exchange(0);
            sample.writeOps = writeOps.exchange(0);
            sample.deleteOps = deleteOps.exchange(0);
            sample.mountOps = mountOps.exchange(0);
            sample.unmountOps = unmountOps.exchange(0);
            sample.keyRangeQueryOps = keyRangeQueryOps.exchange(0);
            return sample;
        }
    };

    std::vector<Sample> samples;
};

class TestRunner
{
public:
    explicit TestRunner(const TestOptions& options);
    TestResult run();

private:
    enum class Operation
    {
        kRead,
        kWrite,
        kDelete,
        kMount,
        kUnmount,
        kKeyRangeQuery,
    };

    struct OperationWeight
    {
        Operation operation = Operation::kRead;
        size_t weight = 0;
    };

    TestOptions m_opts;
    std::vector<OperationWeight> m_operationWeights;
    size_t m_totalWeight = 0;
    TestResult m_result;
    TestResult::SampleCounters m_counters;
    std::mutex m_resultMutex;
    std::unique_ptr<kvs::VirtualStorage> m_storage;

    mutable std::mutex m_pathMutex;
    std::unordered_map<std::string, int> m_storagePaths;
    std::unordered_map<uint64_t, std::string> m_mountPoints;

    mutable std::mutex m_keyMutex;
    std::unordered_set<std::string> m_keysInUse;

    using ClockT = std::chrono::steady_clock;
    std::chrono::time_point<ClockT> m_testStartTime;
    std::chrono::time_point<ClockT> m_intervalStartTime;

    void threadFunc();
    void performReadOp();
    void performWriteOp();
    void performDeleteOp();
    bool performMountOp();
    bool performUnmountOp();
    void performKeyRangeQueryOp();

    Operation getRandomOperation() const;
    std::string getRandomKey() const;
    std::string getRandomPath() const;
    uint64_t getRandomMountId() const;
};
