#include <iostream>
#include <iomanip>
#include "benchmark.h"
#include "option.h"
#include "market_parameters.h"
#include "black_scholes.h"
#include "monte_carlo.h"
#include "discrete_greeks.h"

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

void runGreeksBenchmark() {
    std::cout << "\n== Greeks Benchmark ==\n\n";

    const Option call{105, Option::Type::CALL, 1.0};
    const MarketParameters market{100, 0.05, 0.2};

    const BlackScholesEngine bs_engine;
    const auto [delta, gamma, vega, theta, rho] = bs_engine.price(call, market).greeks;

    std::cout << "Analytical Greeks (Black-Scholes):\n";
    std::cout << "  Delta: " << std::fixed << std::setprecision(6) << delta.value() << "\n";
    std::cout << "  Gamma: " << gamma.value() << "\n";
    std::cout << "  Vega:  " << vega.value() << "\n";
    std::cout << "  Theta: " << theta.value() << "\n";
    std::cout << "  Rho:   " << rho.value() << "\n\n";

    // Performance comparison: Price vs Price+Greeks
    std::cout << "Performance Comparison:\n";
    std::cout << std::setw(25) << "Method"
            << std::setw(15) << "Time (ms)"
            << std::setw(15) << "Greeks Cost"
            << std::setw(15) << "Overhead %\n";
    std::cout << std::string(70, '-') << "\n";

    Benchmark benchmark;

    // Black-Scholes (greeks included)
    {
        constexpr int iterations{100};

        const auto result = benchmark.run(
            "BS Analytical",
            [&]() { return bs_engine.price(call, market).price; },
            iterations
        );

        std::cout << std::setw(25) << "Black-Scholes"
                << std::setw(15) << std::fixed << std::setprecision(4)
                << (result.time_ms / iterations)
                << std::setw(15) << "Included"
                << std::setw(15) << "0.0%\n";
    }

    // Monte Carlo Greeks with different path counts
    for (const int paths: {10000, 50000, 100000}) {
        constexpr int iterations{5};

        SimulationParameters params{paths, 42};
        MonteCarloEngine mc_engine{params};

        // Price only
        const auto price_only = benchmark.run(
            "MC Price " + std::to_string(paths),
            [&]() { return mc_engine.price(call, market).price; },
            iterations
        );

        // Price + Greeks
        FiniteDifferenceGreeks greeks_calc{mc_engine, 0.01};
        const auto with_greeks = benchmark.run(
            "MC Greeks " + std::to_string(paths),
            [&]() { return greeks_calc.calculate(call, market).delta.value(); },
            iterations
        );

        const double price_time = price_only.time_ms / iterations;
        const double greeks_time = with_greeks.time_ms / iterations;
        const double overhead = ((greeks_time - price_time) / price_time) * 100;

        std::cout << std::setw(25) << ("MC (" + std::to_string(paths) + " paths)")
                << std::setw(15) << std::fixed << std::setprecision(1) << price_time
                << std::setw(15) << std::fixed << std::setprecision(1) << greeks_time
                << std::setw(15) << std::fixed << std::setprecision(1) << overhead << "%\n";
    }

    // Accuracy comparison: Analytical vs Numerical Greeks
    std::cout << "\n\nAccuracy Comparison (Monte Carlo vs Analytical):\n";
    std::cout << std::setw(10) << "Paths"
            << std::setw(12) << "Delta Err"
            << std::setw(12) << "Gamma Err"
            << std::setw(12) << "Vega Err"
            << std::setw(12) << "Theta Err"
            << std::setw(12) << "Rho Err\n";
    std::cout << std::string(70, '-') << "\n";

    for (const int paths: {10000, 50000, 100000, 5000000}) {
        SimulationParameters params{paths, 42};
        MonteCarloEngine mc_engine{params};
        FiniteDifferenceGreeks greeks_calc{mc_engine, 0.01};

        const auto [mc_delta, mc_gamma, mc_vega, mc_theta, mc_rho] = greeks_calc.calculate(call, market);

        const double delta_err = std::abs(mc_delta.value() - delta.value());
        const double gamma_err = std::abs(mc_gamma.value() - gamma.value());
        const double vega_err = std::abs(mc_vega.value() - vega.value());
        const double theta_err = std::abs(mc_theta.value() - theta.value());
        const double rho_err = std::abs(mc_rho.value() - rho.value());

        std::cout << std::setw(10) << paths
                << std::setw(12) << std::scientific << std::setprecision(2) << delta_err
                << std::setw(12) << gamma_err
                << std::setw(12) << vega_err
                << std::setw(12) << theta_err
                << std::setw(12) << rho_err << "\n";
    }
}

int main() {
    try {
        runConvergenceBenchmark();
        runPerformanceBenchmark();
        runGreeksBenchmark();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
