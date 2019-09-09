
#pragma once

#include <limits>
#include <list>
#include "utility/MGMutex.hpp"

namespace kvs
{
    class MGLockGuard
    {
    public:
        MGLockGuard() = default;
        MGLockGuard(MGMutex& lock, LockType type)
            : m_lock(&lock)
        {
            m_lock->lock(type);
        }

        MGLockGuard(MGLockGuard&& other)
            : m_lock(other.m_lock)
        {
        }

        ~MGLockGuard()
        {
            if (m_lock) {
                m_lock->unlock();
            }
        }

    private:
        MGMutex* m_lock = nullptr;
    };

    class MGMultiLockGuard
    {
    public:
        ~MGMultiLockGuard()
        {
            while (!m_locks.empty()) {
                m_locks.pop_back();
            }
        }

        void addLock(MGMutex& lock, LockType type) { m_locks.emplace_back(lock, type); }

    private:
        std::list<MGLockGuard> m_locks;
    };
}
