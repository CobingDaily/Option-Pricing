#include "black_scholes.h"
#include "financial_math.h"
#include <cmath>

PricingResult BlackScholesEngine::price(
    const Option &option,
    const MarketParameters &market_parameters
) const {
    const double spot = market_parameters.spot_price;
    const double rate = market_parameters.risk_free_rate;
    const double vol = market_parameters.volatility;
    const double strike = option.getStrike();
    const double expiry = option.getExpiry();

    const double d1 = FinancialMath::calculateD1(spot, strike, rate, vol, expiry);
    const double d2 = FinancialMath::calculateD2(d1, vol, expiry);

    double option_price;
    if (option.getType() == Option::Type::CALL) {
        option_price = spot * FinancialMath::normalCDF(d1) - strike * std::exp(-rate * expiry) * FinancialMath::normalCDF(d2);
    } else {
        option_price = strike * std::exp(-rate * expiry) * FinancialMath::normalCDF(-d2) - spot * FinancialMath::normalCDF(-d1);
    }
    return PricingResult{option_price, "Black-Scholes"};
}

std::string BlackScholesEngine::getName() const {
    return "Black-Scholes Analytical";
}
