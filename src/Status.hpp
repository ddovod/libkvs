
#pragma once

namespace kvs
{
    /** A wrapper of the status of common storage operation. */
    class Status
    {
    public:
        /** Status of the operation. */
        enum class FailReason
        {
            kOk,           /** Everything is fine. */
            kNodeNotFound, /** Target node not found. */
            kKeyNotFound,  /** Target key not found. */
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
