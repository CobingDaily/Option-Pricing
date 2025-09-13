#ifndef OPTION_PRICING_BLACK_SCHOLES_H
#define OPTION_PRICING_BLACK_SCHOLES_H

#include "option.h"
#include "pricing_engine.h"

class BlackScholesEngine : public PricingEngine {
private:
    static Greeks calculateAnalyticalGreeks(
        const Option &option,
        const MarketParameters &market_parameters,
        double d1,
        double d2
    ) ;

public:
    [[nodiscard]] PricingResult price(
        const Option &option,
        const MarketParameters &market_parameters
    ) const override;

    [[nodiscard]] std::string getName() const override;
};

#endif //OPTION_PRICING_BLACK_SCHOLES_H
