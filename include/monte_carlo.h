#ifndef OPTION_PRICING_MONTE_CARLO_H
#define OPTION_PRICING_MONTE_CARLO_H

#include "option.h"
#include <random>
#include <vector>

struct PricingResult {
    double price;
    double standard_error;
    int paths_used;

    PricingResult() : price{0}, standard_error{0}, paths_used{0} {}
};

class MonteCarloEngine {
private:
    mutable std::mt19937 generator_;
    mutable std::normal_distribution<double> distribution_;

    double simulatePath(double S0, double r, double sigma, double T) const;
public:
    explicit MonteCarloEngine(unsigned int seed = 42);

    PricingResult price(
        const Option& option,
        double spot,
        double rate,
        double volatility,
        int num_paths
    ) const;
};

#endif //OPTION_PRICING_MONTE_CARLO_H