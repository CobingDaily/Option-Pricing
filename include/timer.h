#ifndef OPTION_PRICING_TIMER_H
#define OPTION_PRICING_TIMER_H

#include <chrono>

class Timer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    TimePoint start_time_;
    bool is_running_;

public:
    Timer() : is_running_{false} {}

    void start() {
        start_time_ = Clock::now();
        is_running_ = true;
    }

    // returns elapsed millis
    double elapsed() const {
        if (!is_running_) return 0.0;

        const auto end_time = Clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time_
        );
        return duration.count() / 1000.0;
    }

    double stop() {
        const double elapsed_time = elapsed();
        is_running_ = false;
        return elapsed_time;
    }
};

#endif //OPTION_PRICING_TIMER_H