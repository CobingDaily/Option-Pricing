#include "monte_carlo.h"
#include "financial_math.h"
#include <cmath>
#include <numeric>
#include <vector>

#include "discrete_greeks.h"

double MonteCarloEngine::simulatePath(const MarketParameters &market, const double expiry) const {
    const double Z = distribution_(generator_);
    const double drift = FinancialMath::calculateDriftTerm(market.risk_free_rate, market.volatility, expiry);
    const double vol_term = FinancialMath::calculateVolatilityTerm(market.volatility, expiry, Z);

    return FinancialMath::simulateGeometricBrownianMotion(market.spot_price, drift, vol_term);
}


MonteCarloEngine::MonteCarloEngine(const SimulationParameters &parameters)
    : simulation_parameters_{parameters}, generator_{parameters.random_seed}, distribution_{0.0, 1.0} {}

PricingResult MonteCarloEngine::price(
    const Option &option,
    const MarketParameters &market_parameters
) const {
    const double rate = market_parameters.risk_free_rate;
    const int n = simulation_parameters_.num_paths;
    const double time = option.getExpiry();

    std::vector<double> payoffs;
    payoffs.reserve(n);

    for (int i = 0; i < n; ++i) {
        const double final_spot = simulatePath(market_parameters, time);
        const double payoff = option.payoff(final_spot);
        payoffs.push_back(payoff);
    }

    // Stats
    const double sum = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    const double mean = sum / n;

    double variance{0};
    for (const double payoff: payoffs) {
        const double difference = mean - payoff;
        variance += difference * difference;
    }
    variance /= (n - 1);

    const double std_error = std::sqrt(variance / n);

    const double present_price = FinancialMath::discountToPresent(mean, rate, time);
    const double present_error = FinancialMath::discountToPresent(std_error, rate, time);

    return PricingResult{present_price, present_error, n, "Monte Carlo"};
}

std::string MonteCarloEngine::getName() const {
    return "Monte Carlo (" + std::to_string(simulation_parameters_.num_paths) + " paths)";
}
