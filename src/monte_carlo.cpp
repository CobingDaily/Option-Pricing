#include "monte_carlo.h"
#include <cmath>
#include <numeric>

MonteCarloEngine::MonteCarloEngine(const unsigned int seed)
    : generator_{seed}, distribution_{0.0, 1.0} {
}

double MonteCarloEngine::simulatePath(const double S0, const double r, const double sigma, const double T) const {
    const double Z = distribution_(generator_);
    return S0 * std::exp((r - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * Z);
}

PricingResult MonteCarloEngine::price(
    const Option &option,
    const double spot,
    const double rate,
    const double volatility,
    const int num_paths
) const {
    if (num_paths <= 0) {
        throw std::invalid_argument("Number of paths must be positive");
    }
    if (spot <= 0) {
        throw new std::invalid_argument("Spot must be positive");
    }
    if (volatility <= 0) {
        throw new std::invalid_argument("Volatility must be positive");
    }

    std::vector<double> payoffs;
    payoffs.reserve(num_paths);

    for (int i = 0; i < num_paths; ++i) {
        const double final_spot = simulatePath(spot, rate, volatility, option.getExpiry());
        const double payoff = option.payoff(final_spot);

        payoffs.push_back(payoff);
    }

    // Calculate the result:
    const double sum = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    const double mean = sum / num_paths;

    double variance{0.0};
    for (const double payoff: payoffs) {
        const double difference = mean - payoff;
        variance += difference * difference;
    }

    // Unbiased estimator of population variance from stats
    variance /= (num_paths - 1);

    const double std_error = std::sqrt(variance / num_paths);

    PricingResult result;
    result.price = mean * std::exp(-rate * option.getExpiry());
    result.standard_error = std_error * std::exp(-rate * option.getExpiry());
    result.paths_used = num_paths;

    return result;
}
