
#pragma once

namespace kvs
{
    class Status
    {
    public:
        enum class FailReason
        {
            kOk,
            kNodeNotFound,
            kKeyNotFound,
        };

        Status() = default;
        explicit Status(FailReason failReason)
            : m_failReason(failReason)
        {
        }

        bool isOk() const { return m_failReason == FailReason::kOk; }
        FailReason getFailReason() const { return m_failReason; }

    private:
        FailReason m_failReason = FailReason::kOk;
    };
}
