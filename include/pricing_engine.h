#ifndef OPTION_PRICING_PRICING_ENGINE_H
#define OPTION_PRICING_PRICING_ENGINE_H

#include "option.h"
#include "market_parameters.h"
#include "pricing_result.h"

class PricingEngine {
public:
    virtual ~PricingEngine() = default;

    virtual PricingResult price(
        const Option& option,
        const MarketParameters& market_parameters
    ) const = 0;

    virtual std::string getName() const = 0;
};

#endif //OPTION_PRICING_PRICING_ENGINE_H