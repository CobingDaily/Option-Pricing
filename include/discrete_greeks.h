#ifndef OPTION_PRICING_DISCRETE_GREEKS_H
#define OPTION_PRICING_DISCRETE_GREEKS_H

#include "greeks.h"
#include "option.h"
#include "market_parameters.h"
#include "pricing_engine.h"

class FiniteDifferenceGreeks {
private:
    const PricingEngine& engine_;
    double epsilon_;

public:
    explicit FiniteDifferenceGreeks(const PricingEngine& engine, const double epsilon = 0.01)
        : engine_{engine}, epsilon_{epsilon} {}

    [[nodiscard]] Greeks calculate(const Option& option, const MarketParameters& market_parameters) const;

private:
    [[nodiscard]] double calculateDelta(const Option& option, const MarketParameters& market_parameters, double base_price) const;
    [[nodiscard]] double calculateGamma(const Option& option, const MarketParameters& market_parameters) const;
    [[nodiscard]] double calculateVega(const Option& option, const MarketParameters& market_parameters, double base_price) const;
    [[nodiscard]] double calculateTheta(const Option& option, const MarketParameters& market_parameters, double base_price) const;
    [[nodiscard]] double calculateRho(const Option& option, const MarketParameters& market_parameters, double base_price) const;
};

#endif //OPTION_PRICING_DISCRETE_GREEKS_H