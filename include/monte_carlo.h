#ifndef OPTION_PRICING_MONTE_CARLO_H
#define OPTION_PRICING_MONTE_CARLO_H

#include "option.h"
#include "pricing_engine.h"
#include <random>

struct SimulationParameters {
    int num_paths;
    unsigned int random_seed;

    explicit SimulationParameters(const int paths = 100000, const unsigned int seed = 42)
        : num_paths{paths}, random_seed{seed} {
        validate();
    }

    void validate() const {
        if (num_paths <= 0) throw std::invalid_argument("Number of paths must be positive");
    }
};

class MonteCarloEngine : public PricingEngine {
private:
    SimulationParameters simulation_parameters_;
    mutable std::mt19937 generator_;
    mutable std::normal_distribution<double> distribution_;

    double simulatePath(const MarketParameters& market, double expiry) const;

public:
    explicit MonteCarloEngine(const SimulationParameters& parameters = SimulationParameters{});

    PricingResult price(
        const Option& option,
        const MarketParameters& market_parameters
    ) const override;

    std::string getName() const override;
};

#endif //OPTION_PRICING_MONTE_CARLO_H