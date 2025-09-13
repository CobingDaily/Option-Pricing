#ifndef OPTION_PRICING_FINANCIAL_MATH_H
#define OPTION_PRICING_FINANCIAL_MATH_H

#include <cmath>

class FinancialMath {
public:
    // Black-Scholes component
    static double calculateD1(
        const double spot,
        const double strike,
        const double rate,
        const double volatility,
        const double time
    ) {
        return (std::log(spot / strike) + (rate + 0.5 * volatility * volatility) * time)
                / (volatility * std::sqrt(time));
    }

    static double calculateD2(const double d1, const double volatility, const double time) {
        return d1 - volatility * std::sqrt(time);
    }

    static double normalCDF(const double x) {
        return 0.5 * std::erfc(-x / std::sqrt(2.0));
    }

    // Monte Carlo
    static double calculateDriftTerm(const double rate, const double volatility, const double time) {
        return (rate - 0.5 * volatility * volatility) * time;
    }

    static double calculateVolatilityTerm(const double volatility, const double time, const double random_shock) {
        return volatility * std::sqrt(time) * random_shock;
    }

    static double simulateGeometricBrownianMotion(const double initial_price, const double drift, const double vol_term) {
        return initial_price * std::exp(drift + vol_term);
    }

    // Discounting
    static double discountToPresent(const double future_value, const double rate, const double time) {
        return future_value * std::exp(-rate * time);
    }

    // Inverse CDF Approximation
    static double normalQuantile(double p);
    static double getZScore(double confidence_level);
};

#endif //OPTION_PRICING_FINANCIAL_MATH_H