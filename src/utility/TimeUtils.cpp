
#include "TimeUtils.hpp"
#include <chrono>

namespace kvs
{
    int64_t getTimestampMs()
    {
        using namespace std::chrono;

        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
}
