#include "monte_carlo.h"
#include "option.h"
#include "black_scholes.h"
#include "discrete_greeks.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

void printUsage() {
    std::cout << "Usage: ./pricer <spot> <strike> <rate> <vol> <expiry> <type> <method> [paths]\n"
            << "  type: call|put\n"
            << "  method: bs|mc\n"
            << "  paths: number of MC paths (default: 100000)\n"
            << "Example: ./pricer 100 105 0.05 0.2 1.0 call bs\n";
}

Option::Type parseOptionType(const std::string &type_str) {
    std::string lower = type_str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "call" || lower == "c") {
        return Option::Type::CALL;
    }
    if (lower == "put" || lower == "p") {
        return Option::Type::PUT;
    }
    throw std::invalid_argument("Invalid option type");
}

void printResult(const PricingResult &result, const Greeks& greeks) {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Price: $" << result.price;

    if (result.standard_error.has_value()) {
        std::cout << " ± " << result.standard_error.value();
        if (result.paths_used.has_value()) {
            std::cout << " (" << result.paths_used.value() << " paths)";
        }
    }

    std::cout << " [" << result.method_name << "]\n";

    if (greeks.hasGreeks()) {
        std::cout << "Greeks: ";
        if (greeks.delta.has_value()) {
            std::cout << "Δ=" << std::setprecision(4) << greeks.delta.value() << " ";
        }
        if (greeks.gamma.has_value()) {
            std::cout << "Γ=" << std::setprecision(4) << greeks.gamma.value() << " ";
        }
        if (greeks.vega.has_value()) {
            std::cout << "ν=" << std::setprecision(4) << greeks.vega.value() << " ";
        }
        if (greeks.theta.has_value()) {
            std::cout << "Θ=" << std::setprecision(4) << greeks.theta.value() << " ";
        }
        if (greeks.rho.has_value()) {
            std::cout << "ρ=" << std::setprecision(4) << greeks.rho.value() << "\n";
        }
        std::cout << "\n";
    }
}

int main(const int argc, const char *argv[]) {
    try {
        // CLI mode
        if (argc == 2) {
            printUsage();
            return 0;
        }
        if (argc >= 8) {
            const double spot = std::stod(argv[1]);
            const double strike = std::stod(argv[2]);
            const double rate = std::stod(argv[3]);
            const double volatility = std::stod(argv[4]);
            const double expiry = std::stod(argv[5]);
            const Option::Type option_type = parseOptionType(argv[6]);
            std::string method = argv[7];
            std::transform(method.begin(), method.end(), method.begin(), ::tolower);

            int paths{100000};
            if (argc >= 9) {
                paths = std::stoi(argv[8]);
            }

            const Option option{strike, option_type, expiry};
            const MarketParameters market{spot, rate, volatility};

            if (method == "bs") {
                const BlackScholesEngine engine;
                const auto result = engine.price(option, market);
                printResult(result, result.greeks);
            } else if (method == "mc") {
                const SimulationParameters sim_params{paths, 42};
                const MonteCarloEngine engine{sim_params};
                const auto result = engine.price(option, market);

                FiniteDifferenceGreeks greeks_calc{engine, 0.01};
                const auto greeks = greeks_calc.calculate(option, market);

                printResult(result, greeks);
            } else {
                throw std::invalid_argument("Method must be 'bs' or 'mc'");
            }

            return 0;
        }

        // Demo mode
        const Option call{105, Option::Type::CALL, 1};
        const MarketParameters market{110, 0.05, 0.2};

        std::cout << "Option Pricing Demo\n";
        std::cout << "Configuration: Call $105 strike, $110 spot, 5% rate, 20% vol, 1Y expiry\n\n";

        std::cout << "Black-Scholes:\n";
        const BlackScholesEngine bs_engine;
        const auto bs_result = bs_engine.price(call, market);
        printResult(bs_result, bs_result.greeks);

        std::cout << "\nMonte Carlo Convergence:\n";
        for (const int paths: {10000, 100000, 1000000}) {
            SimulationParameters simulation_parameters{paths, 42};
            const MonteCarloEngine mc_engine{simulation_parameters};
            const auto mc_result = mc_engine.price(call, market);

            FiniteDifferenceGreeks greeks_calc{mc_engine, 0.01};
            const auto greeks = greeks_calc.calculate(call, market);

            printResult(mc_result, greeks);
        }
    } catch (const std::exception &e) {
        if (argc >= 2) {
            std::cerr << "Error: " << e.what() << "\n";
            printUsage();
            return 1;
        }
        std::cerr << "Demo error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
