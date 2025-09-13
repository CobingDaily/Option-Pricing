#include "monte_carlo.h"
#include "option.h"
#include <iostream>
#include <iomanip>

#include "black_scholes.h"

void printResult(const PricingResult& result) {
    std::cout << "  Method: " << result.method_name << "\n";
    std::cout << "  Price: $" << std::fixed << std::setprecision(4) << result.price << "\n";

    if (result.hasUncertainty()) {
        std::cout << "  Standard Error: "
                << std::scientific << std::setprecision(2)
                << result.standard_error.value() << "\n";

        for (const double conf_level : {0.90, 0.95, 0.99, 0.999}) {
            auto [lower, upper] = result.getConfidenceInterval(conf_level);

            std::cout << "  " << std::fixed << std::setprecision(1) << (conf_level * 100)
                      << "% CI: [" << std::setprecision(4) << lower << ", " << upper
                      << "] (width: " << std::setprecision(4) << (upper - lower) << ")\n";
        }

        if (result.paths_used.has_value()) {
            std::cout << "  Paths Used: " << result.paths_used.value() << "\n";
        }
    }
    std::cout << "\n";
}

int main() {
    try {
        const Option call{105, Option::Type::CALL, 1};
        const MarketParameters market{110, 0.05, 0.2};

        std::cout << "Option Pricing Comparison\n";

        std::cout << "Analytical Pricing:\n";
        const BlackScholesEngine bs_engine;
        const auto bs_result = bs_engine.price(call, market);
        printResult(bs_result);

        std::cout << "Monte Carlo Convergence:\n";
        for (const int paths : {1000, 10000, 100000, 1000000}) {
            SimulationParameters simulation_parameters{paths, 42};
            const MonteCarloEngine mc_engine{simulation_parameters};

            const auto mc_result = mc_engine.price(call, market);
            printResult(mc_result);

            const double error = std::abs(bs_result.price - mc_result.price);
            std::cout << "    Error vs Black-Scholes: $" << std::fixed << std::setprecision(4) << error << "\n\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
