#include "black_scholes.h"
#include "financial_math.h"
#include <cmath>


Greeks BlackScholesEngine::calculateAnalyticalGreeks(
    const Option &option,
    const MarketParameters &market_parameters,
    const double d1,
    const double d2
) {
    Greeks greeks;

    const double spot = market_parameters.spot_price;
    const double strike = option.getStrike();
    const double rate = market_parameters.risk_free_rate;
    const double vol = market_parameters.volatility;
    const double expiry = option.getExpiry();

    if (option.getType() == Option::Type::CALL) {
        greeks.delta = FinancialMath::normalCDF(d1);
    } else {
        greeks.delta = FinancialMath::normalCDF(d1) - 1.0;
    }

    const double phi_d1 = FinancialMath::normalPDF(d1);
    greeks.gamma = phi_d1 / (spot * vol * std::sqrt(expiry));

    // Divided by 100 for per 1% change
    greeks.vega = spot * phi_d1 * std::sqrt(expiry) / 100.0;

    const double term1 = -(spot * phi_d1 * vol) / (2.0 * std::sqrt(expiry));

    // Theta time unit = days
    if (option.getType() == Option::Type::CALL) {
        const double term2 = rate * strike * std::exp(-rate * expiry) * FinancialMath::normalCDF(d2);
        greeks.theta = (term1 - term2) / 365.0;
    } else {
        const double term2 = rate * strike * std::exp(-rate * expiry) * FinancialMath::normalCDF(-d2);
        greeks.theta = (term1 + term2) / 365.0;
    }

    const double discount_factor = strike * expiry * std::exp(-rate * expiry) / 100.0;

    if (option.getType() == Option::Type::CALL) {
        greeks.rho = discount_factor * FinancialMath::normalCDF(d2);
    } else {
        greeks.rho = -discount_factor * FinancialMath::normalCDF(-d2);
    }

    return greeks;
}


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

    const Greeks greeks = calculateAnalyticalGreeks(option, market_parameters, d1, d2);

    return PricingResult{option_price, greeks, "Black-Scholes"};
}

std::string BlackScholesEngine::getName() const {
    return "Black-Scholes Analytical";
}
