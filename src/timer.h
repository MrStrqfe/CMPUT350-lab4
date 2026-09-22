#include <chrono>
#include <cstdint>
#include <ratio>

// Author: Jensen Lee

#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
    // Get the times
    using Nanos = std::chrono::nanoseconds;
    using Micros = std::chrono::microseconds;
    using Millis = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
    using Minutes = std::chrono::minutes;
    using Hours = std::chrono::hours;

    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    // Default constructor, the timer begins running immediately
    Timer(): m_start(Clock::now()) {}

    void restart() {
        m_start = Clock::now();
    }

    /** 
        click(): Returns durations in unit T and updates the reference point
    */
    template <typename T> uint64_t click() {
        const auto now = Clock::now();
        const uint64_t elapsed = static_cast<uint64_t>(
            std::chrono::duration_cast<T>(now - m_start).count()
        );
        m_start = now;
        return elapsed;
    }

    /**
        glance(): Returns duration without updating the reference point
        It is const because it does not modify the internal timer state
    */
    template <typename T> uint64_t glance() {
        const auto now = Clock::now();
        return static_cast<uint64_t>(
            std::chrono::duration_cast<T>(now - m_start).count()
        );
    }
private:
    TimePoint m_start;
};

#endif  // TIMER_H
