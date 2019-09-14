
#pragma once

#include <cstddef>

namespace kvs::utility
{
    /** Generates a random value in [from, to) interval. */
    size_t random(size_t from, size_t to);

    /** Generates a random value in [0, to) interval. */
    size_t random(size_t to);
}
