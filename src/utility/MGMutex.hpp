
#pragma once

#include <list>
#include <mutex>
#include <thread>

namespace kvs
{
    enum class LockType
    {
        kNL = 0,
        kIS,
        kIX,
        kS,
        kSIX,
        kX,
    };

    struct LockNode
    {
        std::thread::id threadId;
        LockType lockType = LockType::kNL;
    };

    class MGMutex
    {
    public:
        void lock(LockType lockType);
        void unlock();

    private:
        std::mutex m_mutex;
        LockType m_currentType = LockType::kNL;
        std::list<LockNode> m_runningNodes;
        std::list<LockNode> m_waitingNodes;
    };
}
