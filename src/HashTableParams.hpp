
#pragma once

#include <cstddef>

namespace kvs
{
    struct HashTableParams
    {
        size_t bucketCapacity = 32;
        size_t maxRecordSizeBytes = 1024 * 100;          // 100kb
        size_t bucketCacheSizeBytes = 1024 * 1024 * 64;  // 64Mb
        size_t minBucketsCount = 32;
        double minLoadFactor = 0.75;
        double maxLoadFactor = 0.95;
    };
}
