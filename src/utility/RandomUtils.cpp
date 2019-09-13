
#include "RandomUtils.hpp"
#include <cassert>
#include <mutex>
#include <random>

namespace
{
    thread_local std::mt19937 engine{std::random_device{}()};
}

namespace kvs::utility
{
    size_t random(size_t from, size_t to)
    {
        assert(from < to);
        if (from < to) {
            return from + engine() % (to - from);
        }
        return from;
    }

    size_t random(size_t to) { return random(0, to); }
}
