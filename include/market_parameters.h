#ifndef OPTION_PRICING_MARKET_PARAMETERS_H
#define OPTION_PRICING_MARKET_PARAMETERS_H

#include <stdexcept>

struct MarketParameters {
    double spot_price;
    double risk_free_rate;
    double volatility;

    MarketParameters(const double spot, const double rate, const double vol)
        : spot_price{spot}, risk_free_rate{rate}, volatility{vol} {
        validate();
    }

    void validate() const {
        if (spot_price <= 0) throw std::invalid_argument("Spot price must be positive");
        if (volatility <= 0) throw std::invalid_argument("Volatility price must be positive");
    }
};

 #endif //OPTION_PRICING_MARKET_PARAMETERS_H