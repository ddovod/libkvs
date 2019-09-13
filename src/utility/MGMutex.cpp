
#include "MGMutex.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <mutex>
#include <thread>

namespace
{
    bool isCompatible(kvs::LockType lockType1, kvs::LockType lockType2)
    {
        // clang-format off
        static const bool compatibilityMatrix[6][6] = { // NOLINT
            {true, true,  true,  true,  true,  true},
            {true, true,  true,  true,  true,  false},
            {true, true,  true,  false, false, false},
            {true, true,  false, true,  false, false},
            {true, true,  false, false, false, false},
            {true, false, false, false, false, false},
        };
        // clang-format on

        auto i1 = static_cast<size_t>(lockType1);
        auto i2 = static_cast<size_t>(lockType2);
        assert(i1 < 6);
        assert(i2 < 6);
        return compatibilityMatrix[i1][i2];  // NOLINT
    }
}

namespace kvs
{
    void MGMutex::lock(LockType lockType)
    {
        const auto threadId = std::this_thread::get_id();

        LockNode node;
        node.lockType = lockType;
        node.threadId = threadId;

        std::unique_lock<std::mutex> lock{m_mutex};
        if (m_waitingThreads.empty() && ::isCompatible(m_currentType, lockType)) {
            m_runningThreads.push_back(node);
            if (m_currentType == LockType::kNL) {
                m_currentType = lockType;
            }
        } else {
            m_waitingThreads.push_back(node);
            lock.unlock();
            while (true) {
                lock.lock();
                auto found = std::find_if(m_runningThreads.begin(), m_runningThreads.end(), [threadId](const auto& el) {
                    return el.threadId == threadId;
                });
                if (found != m_runningThreads.end()) {
                    lock.unlock();
                    break;
                }
                lock.unlock();
                std::this_thread::yield();
            }
        }
    }

    void MGMutex::unlock()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        auto found = std::find_if(m_runningThreads.begin(), m_runningThreads.end(), [](const auto& el) {
            return el.threadId == std::this_thread::get_id();
        });
        assert(found != m_runningThreads.end());
        m_runningThreads.erase(found);

        if (m_runningThreads.empty()) {
            m_currentType = LockType::kNL;
        }

        if (m_runningThreads.empty() && !m_waitingThreads.empty()) {
            while (!m_waitingThreads.empty()) {
                auto it = m_waitingThreads.begin();
                if (m_runningThreads.empty()) {
                    m_currentType = it->lockType;
                    m_runningThreads.push_back(*it);
                    m_waitingThreads.erase(it);
                } else if (::isCompatible(m_currentType, it->lockType)) {
                    m_runningThreads.push_back(*it);
                    m_waitingThreads.erase(it);
                } else {
                    break;
                }
            }
        }
    }
}
