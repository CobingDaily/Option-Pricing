#include <iostream>
#include <iomanip>
#include "benchmark.h"
#include "option.h"
#include "market_parameters.h"
#include "black_scholes.h"
#include "monte_carlo.h"

void runConvergenceBenchmark() {
    std::cout << "\n== Convergence Benchmark == \n\n";

    const Option call{105, Option::Type::CALL, 1.0};
    const MarketParameters market{100, 0.05, 0.2};

    // Exact analytical price
    const BlackScholesEngine bs_engine;
    const auto bs_result = bs_engine.price(call, market);
    const double true_price = bs_result.price;

    std::cout << "Black-Scholes Price: $"
            << std::fixed << std::setprecision(4)
            << true_price << "\n\n";

    std::cout << std::setw(10) << "Paths"
            << std::setw(15) << "Price"
            << std::setw(15) << "Error"
            << std::setw(15) << "Rel Error %"
            << std::setw(15) << "Time (ms)\n";

    std::cout << std::string(70, '-') << "\n";

    Benchmark benchmark;

    for (const int paths: {1000, 5000, 10000, 50000, 100000, 500000}) {
        SimulationParameters parameters{paths, 42};
        MonteCarloEngine mc_engine{parameters};

        const auto result = benchmark.run(
            "MC " + std::to_string(paths),
            [&]() { return mc_engine.price(call, market).price; },
            1
        );

        const double error = std::abs(result.price - true_price);
        const double rel_error = (error / true_price) * 100;

        std::cout << std::setw(10) << paths
                << std::setw(15) << result.price
                << std::setw(15) << error
                << std::setw(15) << rel_error
                << std::setw(15) << result.time_ms << "\n";
    }
}

void runPerformanceBenchmark() {
    std::cout << "\n== Performance Benchmark == \n\n";

    const Option call{105, Option::Type::CALL, 1.0};
    const MarketParameters market{100, 0.05, 0.2};

    Benchmark benchmark;

    // Benchmark Black-Scholes
    {
        const BlackScholesEngine engine;
        constexpr int iterations{100};

        const auto result = benchmark.run(
            "Black-Scholes",
            [&]() { return engine.price(call, market).price; },
            iterations
        );

        std::cout << "Black-Scholes:\n";
        std::cout << "  Time per pricing: " << (result.time_ms / iterations) << " ms\n";
        std::cout << "  Pricings per second: " << std::scientific
                << result.iterations_per_second() << "\n\n";
    }

    // Benchmark Monte Carlo with different paths
    for (const int paths: {1000, 10000, 100000}) {
        SimulationParameters params{paths, 42};
        MonteCarloEngine engine{params};
        constexpr int iterations{10};

        auto result = benchmark.run(
            "Monte Carlo " + std::to_string(paths),
            [&]() { return engine.price(call, market).price; },
            iterations
        );

        std::cout << "Monte Carlo (" << paths << " paths):\n";
        std::cout << "  Time per pricing: " << (result.time_ms / 10) << " ms\n";
        std::cout << "  Paths per second: " << std::scientific
                << (paths * 10) / (result.time_ms / 1000.0) << "\n\n";
    }
}

int main() {
    try {
        runConvergenceBenchmark();
        runPerformanceBenchmark();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
