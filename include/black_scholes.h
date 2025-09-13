#ifndef OPTION_PRICING_BLACK_SCHOLES_H
#define OPTION_PRICING_BLACK_SCHOLES_H

#include "option.h"
#include "pricing_engine.h"

class BlackScholesEngine : public PricingEngine {
public:
    PricingResult price(
        const Option& option,
        const MarketParameters& market_parameters
    ) const override;

    std::string getName() const override;
};

#endif //OPTION_PRICING_BLACK_SCHOLES_H