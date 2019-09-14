
#pragma once

#include <list>
#include <mutex>
#include <thread>
#include <unordered_set>

namespace kvs
{
    /** A locking mode. */
    enum class LockType
    {
        kNL = 0, /** Null lock. */
        kIS,     /** Intension shared lock. */
        kIX,     /** Intension exclusive lock. */
        kS,      /** Shared lock. */
        kSIX,    /** Shared intention exclusive lock. */
        kX,      /** Exclusive lock. */
    };

    /** A multiple granularity locking mutex. */
    class MGMutex
    {
    public:
        void lock(LockType lockType);
        void unlock();

    private:
        struct LockNode
        {
            std::thread::id threadId;
            LockType lockType = LockType::kNL;
        };

        std::mutex m_mutex;
        LockType m_currentType = LockType::kNL;
        std::list<LockNode> m_runningThreads;
        std::list<LockNode> m_waitingThreads;
    };
}
