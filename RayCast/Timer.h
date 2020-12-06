#pragma once


#include <chrono>
#include <string>
#include <vector>

namespace rc {
    /** Shorthand for the standard time point.*/
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> timer_point;
    
     class Timer
    {
    public:
        Timer();
        void start() noexcept;
        void end() noexcept;
        void dump(const std::string& title);
    private:
        timer_point start_time;
        double running_sum;
        uint64_t sample_count;

        timer_point now() noexcept;
    };

}

