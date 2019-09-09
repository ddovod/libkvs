
#include "MGMutex.hpp"
#include <array>
#include <cassert>
#include <mutex>
#include <thread>

namespace
{
    bool isCompatible(kvs::LockType lockType1, kvs::LockType lockType2)
    {
        // clang-format off
        static const bool compatibilityMatrix[6][6] = {
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
        return compatibilityMatrix[i1][i2];
    }
}

namespace kvs
{
    void MGMutex::lock(LockType lockType)
    {
        LockNode node;
        node.lockType = lockType;
        node.threadId = std::this_thread::get_id();

        std::unique_lock<std::mutex> lock{m_mutex};
        if (m_waitingNodes.empty() && ::isCompatible(m_currentType, lockType)) {
            m_runningNodes.push_back(node);
            if (m_currentType == LockType::kNL) {
                m_currentType = lockType;
            }
        } else {
            m_waitingNodes.push_back(node);
            lock.unlock();
            while (true) {
                lock.lock();
                bool quit = false;
                for (const auto& el : m_runningNodes) {
                    if (el.threadId == std::this_thread::get_id()) {
                        quit = true;
                        break;
                    }
                }
                if (quit) {
                    lock.unlock();
                    break;
                } else {
                    lock.unlock();
                    std::this_thread::yield();
                }
            }
        }
    }

    void MGMutex::unlock()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        for (auto it = m_runningNodes.begin(); it != m_runningNodes.end(); ++it) {
            if (it->threadId == std::this_thread::get_id()) {
                m_runningNodes.erase(it);
                break;
            }
        }

        if (m_runningNodes.empty()) {
            m_currentType = LockType::kNL;
        }

        if (m_runningNodes.empty() && !m_waitingNodes.empty()) {
            while (!m_waitingNodes.empty()) {
                auto it = m_waitingNodes.begin();
                if (m_runningNodes.empty()) {
                    m_currentType = it->lockType;
                    m_runningNodes.push_back(*it);
                    m_waitingNodes.erase(it);
                } else if (::isCompatible(m_currentType, it->lockType)) {
                    m_runningNodes.push_back(*it);
                    m_waitingNodes.erase(it);
                } else {
                    break;
                }
            }
        }
    }
}
