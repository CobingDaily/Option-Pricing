#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include "benchmark.h"
#include "option.h"
#include "market_parameters.h"
#include "black_scholes.h"
#include "monte_carlo.h"
#include "discrete_greeks.h"

namespace BenchmarkConfig {
    // Test parameters
    constexpr double SPOT_PRICE{100.0};
    constexpr double STRIKE_PRICE{105.0};
    constexpr double RISK_FREE_RATE{0.05};
    constexpr double VOLATILITY{0.20};
    constexpr double TIME_TO_EXPIRY{1.0};
    constexpr unsigned int RANDOM_SEED{42};

    // Benchmark iterations
    constexpr int BS_ITERATIONS{1000};
    constexpr int MC_ITERATIONS{10};
    constexpr int GREEKS_ITERATIONS{5};

    // Monte Carlo path configurations
    const std::vector CONVERGENCE_PATHS = {1000, 5000, 10000, 50000, 100000, 500000};
    const std::vector PERFORMANCE_PATHS = {1000, 10000, 100000};
    const std::vector GREEKS_PATHS = {10000, 50000, 100000};
    const std::vector ACCURACY_PATHS = {10000, 50000, 100000, 500000};

    // Finite difference epsilon
    constexpr double FD_EPSILON{0.01};

    // Output formatting
    constexpr int SEPARATOR_WIDTH{80};
    constexpr int PRICE_PRECISION{4};
    constexpr int GREEKS_PRECISION{6};
    constexpr int PERCENT_PRECISION{2};
}

void printSectionHeader(const std::string &title) {
    std::cout << "\n" << std::string(BenchmarkConfig::SEPARATOR_WIDTH, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(BenchmarkConfig::SEPARATOR_WIDTH, '=') << "\n\n";
}

void printSubsectionHeader(const std::string &title) {
    std::cout << "\n" << title << "\n";
    std::cout << std::string(BenchmarkConfig::SEPARATOR_WIDTH, '-') << "\n";
}

void printTableSeparator() {
    std::cout << std::string(BenchmarkConfig::SEPARATOR_WIDTH, '-') << "\n";
}

std::string formatMicroseconds(const double microseconds) {
    if (microseconds < 1) {
        return std::to_string(static_cast<int>(microseconds * 1000)) + " ns";
    }
    if (microseconds < 1000) {
        return std::to_string(static_cast<int>(microseconds)) + " μs";
    }
    if (microseconds < 1000000) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << (microseconds / 1000) << " ms";
        return oss.str();
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << (microseconds / 1000000) << " s";
    return oss.str();
}

std::string formatNumber(const double value, const int precision = 2) {
    std::ostringstream oss;
    if (std::abs(value) < 1e-3 || std::abs(value) > 1e6) {
        oss << std::scientific << std::setprecision(precision) << value;
    } else {
        oss << std::fixed << std::setprecision(precision) << value;
    }
    return oss.str();
}

Option createTestOption() {
    return Option{
        BenchmarkConfig::STRIKE_PRICE,
        Option::Type::CALL,
        BenchmarkConfig::TIME_TO_EXPIRY
    };
}

MarketParameters createTestMarket() {
    return MarketParameters{
        BenchmarkConfig::SPOT_PRICE,
        BenchmarkConfig::RISK_FREE_RATE,
        BenchmarkConfig::VOLATILITY
    };
}


void runConvergenceBenchmark() {
    printSectionHeader("CONVERGENCE BENCHMARK");

    const auto call = createTestOption();
    const auto market = createTestMarket();

    const BlackScholesEngine bs_engine;
    const auto bs_result = bs_engine.price(call, market);
    const double true_price = bs_result.price;

    std::cout << "Reference Price (Black-Scholes): $"
            << std::fixed << std::setprecision(BenchmarkConfig::PRICE_PRECISION)
            << true_price << "\n\n";

    std::cout << std::left
            << std::setw(12) << "Paths"
            << std::setw(12) << "Price"
            << std::setw(12) << "Abs Error"
            << std::setw(12) << "Rel Error"
            << std::setw(12) << "Std Error"
            << std::setw(15) << "Time"
            << "\n";

    printTableSeparator();

    Benchmark benchmark;

    for (const int paths: BenchmarkConfig::CONVERGENCE_PATHS) {
        SimulationParameters params{paths, BenchmarkConfig::RANDOM_SEED};
        MonteCarloEngine mc_engine{params};

        const auto bench_result = benchmark.run(
            "MC_" + std::to_string(paths),
            [&]() { return mc_engine.price(call, market).price; },
            1
        );

        const auto pricing_result = mc_engine.price(call, market);

        const double error = std::abs(bench_result.price - true_price);
        const double rel_error = (error / true_price) * 100;

        std::cout << std::left
                << std::setw(12) << paths
                << std::setw(12) << formatNumber(bench_result.price, BenchmarkConfig::PRICE_PRECISION)
                << std::setw(12) << formatNumber(error, BenchmarkConfig::PRICE_PRECISION)
                << std::setw(12) << formatNumber(rel_error, BenchmarkConfig::PERCENT_PRECISION) + "%"
                << std::setw(12) << formatNumber(pricing_result.standard_error.value(), 4)
                << std::setw(15) << formatMicroseconds(bench_result.time_microseconds)
                << "\n";
    }
}

void runPerformanceBenchmark() {
    printSectionHeader("PERFORMANCE BENCHMARK");

    const auto call = createTestOption();
    const auto market = createTestMarket();

    Benchmark benchmark;

    // Black-Scholes Performance
    printSubsectionHeader("Analytical Pricing (Black-Scholes)"); {
        const BlackScholesEngine engine;
        const auto result = benchmark.run(
            "Black-Scholes",
            [&]() { return engine.price(call, market).price; },
            BenchmarkConfig::BS_ITERATIONS
        );

        std::cout << "  Iterations:          " << BenchmarkConfig::BS_ITERATIONS << "\n";
        std::cout << "  Total time:          " << formatMicroseconds(result.time_microseconds) << "\n";
        std::cout << "  Time per pricing:    " << formatMicroseconds(result.time_per_iteration_microseconds()) << "\n";
        std::cout << "  Pricings per second: " << formatNumber(result.iterations_per_second(), 0) << "\n";
    }

    // Monte Carlo Performance
    printSubsectionHeader("Monte Carlo Simulation");

    std::cout << std::left
            << std::setw(15) << "Paths"
            << std::setw(20) << "Time/Pricing"
            << std::setw(20) << "Paths/Second"
            << std::setw(20) << "Relative Speed"
            << "\n";
    printTableSeparator();

    double baseline_time = 0;

    for (size_t i = 0; i < BenchmarkConfig::PERFORMANCE_PATHS.size(); ++i) {
        const int paths = BenchmarkConfig::PERFORMANCE_PATHS[i];
        SimulationParameters params{paths, BenchmarkConfig::RANDOM_SEED};
        MonteCarloEngine engine{params};

        const auto result = benchmark.run(
            "MC_" + std::to_string(paths),
            [&]() { return engine.price(call, market).price; },
            BenchmarkConfig::MC_ITERATIONS
        );

        const double time_per_iter = result.time_per_iteration_microseconds();
        const double paths_per_second = (paths * BenchmarkConfig::MC_ITERATIONS) /
                                        (result.time_microseconds / 1000000.0);

        if (i == 0) baseline_time = time_per_iter;
        const double relative_speed = baseline_time / time_per_iter;

        std::cout << std::left
                << std::setw(15) << paths
                << std::setw(20) << formatMicroseconds(time_per_iter)
                << std::setw(20) << formatNumber(paths_per_second, 0)
                << std::setw(20) << formatNumber(relative_speed, 2) + "x"
                << "\n";
    }
}

void printGreeksRow(const std::string &label, const Greeks &greeks) {
    std::cout << std::left << std::setw(20) << label;

    auto printGreek = [](const std::optional<double> &value, const std::string &name) {
        std::cout << "  " << name << ": ";
        if (value.has_value()) {
            std::cout << std::setw(10) << std::fixed
                    << std::setprecision(BenchmarkConfig::GREEKS_PRECISION)
                    << value.value();
        } else {
            std::cout << std::setw(10) << "N/A";
        }
    };

    printGreek(greeks.delta, "Δ");
    printGreek(greeks.gamma, "Γ");
    printGreek(greeks.vega, "ν");
    printGreek(greeks.theta, "Θ");
    printGreek(greeks.rho, "ρ");
    std::cout << "\n";
}

void runGreeksBenchmark() {
    printSectionHeader("GREEKS BENCHMARK");

    const auto call = createTestOption();
    const auto market = createTestMarket();

    // Calculate analytical Greeks as reference
    const BlackScholesEngine bs_engine;
    const auto bs_result = bs_engine.price(call, market);

    printSubsectionHeader("Reference Greeks (Black-Scholes Analytical)");
    printGreeksRow("Analytical", bs_result.greeks);

    // Performance comparison
    printSubsectionHeader("Performance Impact of Greeks Calculation");

    std::cout << std::left
            << std::setw(25) << "Method"
            << std::setw(15) << "Price Only"
            << std::setw(15) << "With Greeks"
            << std::setw(15) << "Overhead"
            << std::setw(15) << "Factor"
            << "\n";
    printTableSeparator();

    Benchmark benchmark;

    // Black-Scholes (Greeks are essentially free)
    {
        const auto result = benchmark.run(
            "BS_Greeks",
            [&]() { return bs_engine.price(call, market).price; },
            BenchmarkConfig::BS_ITERATIONS
        );

        std::cout << std::left
                << std::setw(25) << "Black-Scholes"
                << std::setw(15) << formatMicroseconds(result.time_per_iteration_microseconds())
                << std::setw(15) << formatMicroseconds(result.time_per_iteration_microseconds())
                << std::setw(15) << "~0%"
                << std::setw(15) << "1.0x"
                << "\n";
    }

    // Monte Carlo with finite differences
    for (const int paths: BenchmarkConfig::GREEKS_PATHS) {
        SimulationParameters params{paths, BenchmarkConfig::RANDOM_SEED};
        MonteCarloEngine mc_engine{params};
        FiniteDifferenceGreeks greeks_calc{mc_engine, BenchmarkConfig::FD_EPSILON};

        // Price only
        const auto price_only = benchmark.run(
            "MC_Price_" + std::to_string(paths),
            [&]() { return mc_engine.price(call, market).price; },
            BenchmarkConfig::GREEKS_ITERATIONS
        );

        // With Greeks (calculates all Greeks)
        const auto with_greeks = benchmark.run(
            "MC_Greeks_" + std::to_string(paths),
            [&]() {
                auto g = greeks_calc.calculate(call, market);
                return g.delta.value_or(0.0);
            },
            1
        );

        const double price_time = price_only.time_per_iteration_microseconds();
        const double greeks_time = with_greeks.time_microseconds;
        const double overhead_percent = ((greeks_time - price_time) / price_time) * 100;
        const double factor = greeks_time / price_time;

        std::cout << std::left
                << std::setw(25) << ("Monte Carlo (" + std::to_string(paths) + ")")
                << std::setw(15) << formatMicroseconds(price_time)
                << std::setw(15) << formatMicroseconds(greeks_time)
                << std::setw(15) << formatNumber(overhead_percent, 1) + "%"
                << std::setw(15) << formatNumber(factor, 1) + "x"
                << "\n";
    }

    // Accuracy comparison
    printSubsectionHeader("Greeks Accuracy (Finite Differences vs Analytical)");

    std::cout << std::left
            << std::setw(12) << "Paths"
            << std::setw(12) << "Delta Err"
            << std::setw(12) << "Gamma Err"
            << std::setw(12) << "Vega Err"
            << std::setw(12) << "Theta Err"
            << std::setw(12) << "Rho Err"
            << "\n";
    printTableSeparator();

    for (const int paths: BenchmarkConfig::ACCURACY_PATHS) {
        SimulationParameters params{paths, BenchmarkConfig::RANDOM_SEED};
        MonteCarloEngine mc_engine{params};
        FiniteDifferenceGreeks greeks_calc{mc_engine, BenchmarkConfig::FD_EPSILON};

        const auto [mc_delta, mc_gamma, mc_vega, mc_theta, mc_rho] = greeks_calc.calculate(call, market);

        auto calcError = [](const std::optional<double> &mc, const std::optional<double> &bs) {
            if (mc.has_value() && bs.has_value()) {
                return std::abs(mc.value() - bs.value());
            }
            return 0.0;
        };

        std::cout << std::left
                << std::setw(12) << paths
                << std::setw(12) << formatNumber(calcError(mc_delta, bs_result.greeks.delta), 4)
                << std::setw(12) << formatNumber(calcError(mc_gamma, bs_result.greeks.gamma), 4)
                << std::setw(12) << formatNumber(calcError(mc_vega, bs_result.greeks.vega), 4)
                << std::setw(12) << formatNumber(calcError(mc_theta, bs_result.greeks.theta), 4)
                << std::setw(12) << formatNumber(calcError(mc_rho, bs_result.greeks.rho), 4)
                << "\n";
    }
}

void printSummary() {
    printSectionHeader("BENCHMARK SUMMARY");

    std::cout << "Test Configuration:\n";
    std::cout << "  Option Type:    European Call\n";
    std::cout << std::setprecision(2);
    std::cout << "  Spot Price:     $" << BenchmarkConfig::SPOT_PRICE << "\n";
    std::cout << "  Strike Price:   $" << BenchmarkConfig::STRIKE_PRICE << "\n";
    std::cout << "  Risk-Free Rate: " << (BenchmarkConfig::RISK_FREE_RATE * 100) << "%\n";
    std::cout << "  Volatility:     " << (BenchmarkConfig::VOLATILITY * 100) << "%\n";
    std::cout << "  Time to Expiry: " << BenchmarkConfig::TIME_TO_EXPIRY << " year(s)\n";
    std::cout << "  Random Seed:    " << BenchmarkConfig::RANDOM_SEED << "\n";
}

int main() {
    try {
        std::cout << std::fixed;

        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                      OPTION PRICING BENCHMARK SUITE                        ║\n";
        std::cout << "║                    Monte Carlo vs Black-Scholes Analysis                   ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════════╝\n";

        runConvergenceBenchmark();
        runPerformanceBenchmark();
        runGreeksBenchmark();

        printSummary();
    } catch (const std::exception &e) {
        std::cerr << "\nError: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
