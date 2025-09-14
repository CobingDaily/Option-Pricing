#ifndef OPTION_PRICING_BENCHMARK_H
#define OPTION_PRICING_BENCHMARK_H

#include <vector>
#include <string>
#include <functional>
#include "timer.h"

struct BenchmarkResult {
    std::string name;
    double time_ms;
    double price;
    int iterations;

    double iterations_per_second() const {
        return iterations / (time_ms / 1000.0);
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

        const double elapsed = timer.stop();

        BenchmarkResult result{name, elapsed, last_price, iterations};
        results_.push_back(result);
        return result;
    }

    const std::vector<BenchmarkResult> &getResults() const { return results_; }

    void clear() { results_.clear(); }
};

#endif //OPTION_PRICING_BENCHMARK_H
