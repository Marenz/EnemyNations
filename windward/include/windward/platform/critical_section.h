#pragma once

#ifdef _WIN32
#    include <windows.h>
#else
#    include <mutex>
#endif

namespace windward::platform {

class CriticalSection {
public:
    CriticalSection()
#ifdef _WIN32
    {
        InitializeCriticalSection(&section_);
    }
#else
        = default;
#endif

    ~CriticalSection() noexcept
#ifdef _WIN32
    {
        DeleteCriticalSection(&section_);
    }
#else
        = default;
#endif

    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

    void Lock()
    {
#ifdef _WIN32
        EnterCriticalSection(&section_);
#else
        mutex_.lock();
#endif
    }

    void Unlock() noexcept
    {
#ifdef _WIN32
        LeaveCriticalSection(&section_);
#else
        mutex_.unlock();
#endif
    }

    bool TryLock()
    {
#ifdef _WIN32
        return TryEnterCriticalSection(&section_) != FALSE;
#else
        return mutex_.try_lock();
#endif
    }

private:
#ifdef _WIN32
    CRITICAL_SECTION section_;
#else
    std::recursive_mutex mutex_;
#endif
};

class CriticalSectionLock {
public:
    explicit CriticalSectionLock(CriticalSection& section)
        : section_(section)
    {
        section_.Lock();
    }

    CriticalSectionLock(const CriticalSectionLock&) = delete;
    CriticalSectionLock& operator=(const CriticalSectionLock&) = delete;

    ~CriticalSectionLock()
    {
        section_.Unlock();
    }

private:
    CriticalSection& section_;
};

} // namespace windward::platform

