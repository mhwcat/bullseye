#include "simple_timer.h"

#include <chrono>

namespace bullseye::simple_timer {
    SimpleTimer::SimpleTimer()
    {
        // Initialize begin value to prevent counting delta using undefined value
        begin = std::chrono::high_resolution_clock::now();
    }

    void SimpleTimer::start()
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    uint64_t SimpleTimer::get_milliseconds_since_start()
    {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    }

    uint64_t SimpleTimer::get_microseconds_since_start()
    {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
    }
}