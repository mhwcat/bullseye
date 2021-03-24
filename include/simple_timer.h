#ifndef BULLSEYE_SIMPLE_TIMER_H
#define BULLSEYE_SIMPLE_TIMER_H

#include <chrono>

namespace bullseye::simple_timer {
    class SimpleTimer
    {
        public:
            SimpleTimer();
            void start();

            uint64_t get_milliseconds_since_start();
            uint64_t get_microseconds_since_start();

        private:
            std::chrono::high_resolution_clock::time_point begin;
    };
}

#endif