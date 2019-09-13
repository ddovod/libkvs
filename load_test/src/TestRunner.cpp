
#include "TestRunner.hpp"
#include <algorithm>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <sys/types.h>
#include <utility/StringUtils.hpp>
#include "utility.hpp"

#ifdef assert
#undef assert
#endif

#define assert(expr)      \
    do {                  \
        if (!(expr))      \
            std::abort(); \
    } while (false)

TestRunner::TestRunner(const TestOptions& options)
    : m_opts(options)
{
    m_rand = std::mt19937{m_opts.rndSeed};
    m_opts.maxNodes = std::max(1ul, m_opts.maxNodes);
    m_operationWeights = {
        {Operation::kRead, m_opts.readsWeight},
        {Operation::kWrite, m_opts.writesWeight},
        {Operation::kDelete, m_opts.deletesWeight},
        {Operation::kMount, m_opts.mountsWeight},
        {Operation::kUnmount, m_opts.unmountsWeight},
        {Operation::kKeyRangeQuery, m_opts.krqsWeight},
    };
    for (const auto& el : m_operationWeights) {
        m_totalWeight += el.weight;
    }

    m_storage = std::make_unique<kvs::VirtualStorage>();

    kvs::MountOptions mountOptions;
    mountOptions.volumeFilePath = "load_test_data/load_test_volume.json";
    mountOptions.storageMountPath = "A";
    mountOptions.nodePath = "A";
    mountOptions.hashTableParams.bucketCacheSizeBytes = m_opts.cacheSize;
    mountOptions.hashTableParams.maxRecordSizeBytes = m_opts.recordSize;
    auto mountResult = m_storage->mount(mountOptions);
    assert(mountResult.isOk());

    m_storagePaths["A"]++;
    m_mountPoints[mountResult.getMountId()] = "A";
}

TestResult TestRunner::run()
{
    std::vector<std::unique_ptr<std::thread>> threads;
    m_testStartTime = ClockT::now();
    m_intervalStartTime = ClockT::now();
    for (int i = 0; i < m_opts.threadsCount; i++) {
        threads.push_back(std::make_unique<std::thread>([this] { threadFunc(); }));
    }
    for (auto& t : threads) {
        t->join();
    }

    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ClockT::now() - m_intervalStartTime).count();
    m_result.samples.push_back(m_counters.reset(diff));
    return m_result;
}

void TestRunner::threadFunc()
{
    using namespace std::chrono;

    while (duration_cast<milliseconds>(ClockT::now() - m_testStartTime).count() < m_opts.testTime) {
        switch (getRandomOperation()) {
            case Operation::kRead: {
                performReadOp();
                m_counters.readOps++;
                break;
            }
            case Operation::kWrite: {
                performWriteOp();
                m_counters.writeOps++;
                break;
            }
            case Operation::kDelete: {
                performDeleteOp();
                m_counters.deleteOps++;
                break;
            }
            case Operation::kMount: {
                if (!performMountOp()) {
                    continue;
                }
                m_counters.mountOps++;
                break;
            }
            case Operation::kUnmount: {
                if (!performUnmountOp()) {
                    continue;
                }
                m_counters.unmountOps++;
                break;
            }
            case Operation::kKeyRangeQuery: {
                performKeyRangeQueryOp();
                m_counters.keyRangeQueryOps++;
                break;
            }
        }
        m_counters.totalOps++;

        std::lock_guard<std::mutex> lock{m_resultMutex};
        auto now = ClockT::now();
        auto diff = duration_cast<milliseconds>(now - m_intervalStartTime).count();
        if (diff >= m_opts.windowSize) {
            m_result.samples.push_back(m_counters.reset(diff));
            m_intervalStartTime = now;
        }
    }
}

TestRunner::Operation TestRunner::getRandomOperation() const
{
    int rndValue = m_rand() % m_totalWeight + 1;
    size_t i = 0;
    while (true) {
        rndValue -= m_operationWeights[i].weight;
        if (rndValue <= 0) {
            break;
        }
        i++;
    }
    return m_operationWeights[i].operation;
}

void TestRunner::performReadOp()
{
    kvs::Value value;
    m_storage->getValue({getRandomKey(), getRandomPath()}, &value);
}

void TestRunner::performWriteOp()
{
    kvs::Data value;
    value.buffer.resize(m_opts.dataSize);
    m_storage->putValue({kvs::generateString(m_opts.keyLength), generatePath()}, {value});
}

void TestRunner::performDeleteOp()
{
    m_storage->deleteValue({getRandomKey(), getRandomPath()});
}

bool TestRunner::performMountOp()
{
    {
        std::lock_guard<std::mutex> lock{m_pathMutex};
        if (m_storagePaths.size() == m_opts.maxNodes) {
            return false;
        }
    }

    kvs::MountOptions options;
    options.volumeFilePath = "load_test_data/load_test_volume.json";
    options.storageMountPath = generatePath();
    options.nodePath = generatePath();
    options.hashTableParams.bucketCacheSizeBytes = m_opts.cacheSize;
    options.hashTableParams.maxRecordSizeBytes = m_opts.recordSize;
    auto result = m_storage->mount(options);
    assert(result.isOk());

    std::lock_guard<std::mutex> lock{m_pathMutex};
    m_storagePaths[options.storageMountPath]++;
    m_mountPoints[result.getMountId()] = options.storageMountPath;

    return true;
}

bool TestRunner::performUnmountOp()
{
    {
        std::lock_guard<std::mutex> lock{m_pathMutex};
        if (m_storagePaths.size() == 1) {
            return false;
        }
    }

    auto mountId = getRandomMountId();
    auto result = m_storage->unmount({mountId});
    if (result.isOk()) {
        std::lock_guard<std::mutex> lock{m_pathMutex};
        auto found = m_mountPoints.find(mountId);
        m_storagePaths[found->second]--;
        if (m_storagePaths[found->second] == 0) {
            m_storagePaths.erase(found->second);
        }
        m_mountPoints.erase(found);
    }

    return true;
}

void TestRunner::performKeyRangeQueryOp()
{
    size_t offset = m_rand() % 1000;
    size_t count = m_rand() % 1000;
    kvs::KeysRange keysRange;
    m_storage->getKeysRange({getRandomPath(), offset, count}, &keysRange);
}

std::string TestRunner::getRandomKey() const
{
    if (m_rand() % 100 < 3) {
        return kvs::generateString(m_opts.keyLength);
    }
    std::lock_guard<std::mutex> lock{m_keyMutex};
    if (m_keysInUse.empty()) {
        return kvs::generateString(m_opts.keyLength);
    }
    return getRandomElement(m_keysInUse);
}

std::string TestRunner::getRandomPath() const
{
    if (m_rand() % 100 < 3) {
        return generatePath();
    }
    std::lock_guard<std::mutex> lock{m_pathMutex};
    return getRandomElement(m_storagePaths).first;
}

uint64_t TestRunner::getRandomMountId() const
{
    std::lock_guard<std::mutex> lock{m_pathMutex};
    return getRandomElement(m_mountPoints).first;
}
