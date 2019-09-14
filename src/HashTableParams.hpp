
#pragma once

#include <cstddef>

namespace kvs
{
    /** Parameters for hash table. */
    struct HashTableParams
    {
        size_t bucketCapacity = 32;                     /** Maximum records in each bucket. */
        size_t maxRecordSizeBytes = 1024 * 100;         /** Maximum record size, in bytes. */
        size_t bucketCacheSizeBytes = 1024 * 1024 * 64; /** Maximum in-memory cache size, in bytes. */
        size_t minBucketsCount = 32;                    /** Minimum buckets in the hash table. */
        double minLoadFactor = 0.75;                    /** Minimum load factor, used to shrink the table. */
        double maxLoadFactor = 0.95;                    /** Maximum load factor, used to extend the table. */
    };
}
