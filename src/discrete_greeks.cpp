#include "discrete_greeks.h"

Greeks FiniteDifferenceGreeks::calculate(
    const Option &option,
    const MarketParameters &market_parameters
) const {
    Greeks greeks;

    const double base_price = engine_.price(option, market_parameters).price;

    greeks.delta = calculateDelta(option, market_parameters, base_price);
    greeks.gamma = calculateGamma(option, market_parameters);
    greeks.vega = calculateVega(option, market_parameters, base_price);
    greeks.theta = calculateTheta(option, market_parameters, base_price);
    greeks.rho = calculateRho(option, market_parameters, base_price);

    return greeks;
}

double FiniteDifferenceGreeks::calculateDelta(
    const Option &option,
    const MarketParameters &market_parameters,
    const double base_price
) const {
    const double finite_incr = market_parameters.spot_price * epsilon_;

    const MarketParameters market_up{
        market_parameters.spot_price + finite_incr,
        market_parameters.risk_free_rate,
        market_parameters.volatility
    };

    const double price_incr = engine_.price(option, market_up).price;

    return (price_incr - base_price) / finite_incr;
}

double FiniteDifferenceGreeks::calculateGamma(
    const Option &option,
    const MarketParameters &market_parameters
) const {
    const double finite_incr = market_parameters.spot_price * epsilon_;

    const MarketParameters market_up{
        market_parameters.spot_price + finite_incr,
        market_parameters.risk_free_rate,
        market_parameters.volatility
    };

    const MarketParameters market_down{
        market_parameters.spot_price - finite_incr,
        market_parameters.risk_free_rate,
        market_parameters.volatility
    };

    const double price_up = engine_.price(option, market_up).price;
    const double price_center = engine_.price(option, market_parameters).price;
    const double price_down = engine_.price(option, market_down).price;

    return (price_up - 2 * price_center + price_down) / (finite_incr * finite_incr);
}

double FiniteDifferenceGreeks::calculateVega(
    const Option &option,
    const MarketParameters &market_parameters,
    const double base_price
) const {
    const double vol_up = epsilon_;

    const MarketParameters market_vol_up{
        market_parameters.spot_price,
        market_parameters.risk_free_rate,
        market_parameters.volatility + vol_up
    };

    const double price = engine_.price(option, market_vol_up).price;

    return (price - base_price) / vol_up / 100.0;
}

double FiniteDifferenceGreeks::calculateTheta(
    const Option &option,
    const MarketParameters &market_parameters,
    const double base_price
) const {
    constexpr double time_bump = 1 / 365.0;

    if (option.getExpiry() <= time_bump) {
        return 0.0;
    }

    const Option option_less_time{
        option.getStrike(),
        option.getType(),
        option.getExpiry() - time_bump
    };

    const double price_less_time = engine_.price(option_less_time, market_parameters).price;

    return (price_less_time - base_price) / time_bump / 365.0;
}

double FiniteDifferenceGreeks::calculateRho(
    const Option &option,
    const MarketParameters &market_parameters,
    const double base_price
) const {
    const double rate_bump = epsilon_;

    const MarketParameters market_rate_up{
        market_parameters.spot_price,
        market_parameters.risk_free_rate + rate_bump,
        market_parameters.volatility
    };

    const double price_up = engine_.price(option, market_rate_up).price;

    return (price_up - base_price) / rate_bump / 100.0;
}

