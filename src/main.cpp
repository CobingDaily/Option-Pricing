#include "monte_carlo.h"
#include "option.h"
#include <iostream>
#include <iomanip>

#include "black_scholes.h"

void printResult(const PricingResult &result) {
    std::cout << "  Price: $" << std::fixed << std::setprecision(4) << result.price << "\n";
    std::cout << "  Standard Error: " << std::scientific << std::setprecision(2) << result.standard_error << "\n";
    std::cout << "  95% CI: "
            << std::fixed << std::setprecision(4)
            << result.price - 1.96 * result.standard_error << ", "
            << result.price + 1.96 * result.standard_error << "]\n\n";
}

int main() {
    const Option call{105, Option::Type::CALL, 1};


    constexpr double spot{110};
    constexpr double rate{0.05};
    constexpr double volatility{0.2};

    const MonteCarloEngine engine{42};

    std::cout << "Monte-Carlo Convergence\n";

    for (const int paths: {1000, 10000, 100000, 1000000}) {
        try {
            PricingResult result = engine.price(call, spot, rate, volatility, paths);
            std::cout << "Result with " << paths << " paths:\n";
            printResult(result);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Black-Scholes Price\n";
    const double bs_price = BlackScholes::price(call, spot, rate, volatility);
    std::cout << "  Price: $" << std::fixed << std::setprecision(4) << bs_price << "\n";

    return 0;
}
