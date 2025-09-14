#ifndef OPTION_PRICING_BENCHMARK_H
#define OPTION_PRICING_BENCHMARK_H

#include <vector>
#include <string>
#include <functional>
#include "timer.h"

struct BenchmarkResult {
    std::string name;
    double time_microseconds;
    double price;
    int iterations;

    [[nodiscard]] double time_per_iteration_microseconds() const {
        return time_microseconds / iterations;
    }

    [[nodiscard]] double time_per_iteration_milliseconds() const {
        return time_microseconds / (iterations * 1000.0);
    }

    [[nodiscard]] double iterations_per_second() const {
        return iterations / (time_microseconds / 1000000.0);
    }
};

class Benchmark {
private:
    std::vector<BenchmarkResult> results_;

public:
    template<typename Func>
    BenchmarkResult run(const std::string &name, Func func, const int iterations = 1) {
        Timer timer;

        // warmup run
        func();

        // actual run
        timer.start();

        double last_price = 0;
        for (int i = 0; i < iterations; ++i) {
            last_price = func();
        }

        const double elapsed_us = timer.stop();

        BenchmarkResult result{name, elapsed_us, last_price, iterations};
        results_.push_back(result);
        return result;
    }

    [[nodiscard]] const std::vector<BenchmarkResult> &getResults() const { return results_; }

    void clear() { results_.clear(); }
};

#endif //OPTION_PRICING_BENCHMARK_H
