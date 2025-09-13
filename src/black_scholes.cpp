#include "black_scholes.h"
#include <cmath>

double BlackScholes::normalCDF(const double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double BlackScholes::price(
    const Option &option,
    const double spot,
    const double rate,
    const double volatility
) {
    const double K{option.getStrike()};
    const double T{option.getExpiry()};

    const double d1 = (std::log(spot / K) + (rate + 0.5 * volatility * volatility) * T) / (volatility * std::sqrt(T));
    const double d2 = d1 - volatility * std::sqrt(T);

    if (option.getType() == Option::Type::CALL) {
        return spot * normalCDF(d1) - K * std::exp(-rate * T) * normalCDF(d2);
    }

    return K * std::exp(-rate * T) * normalCDF(-d2) - spot * normalCDF(-d1);
}
