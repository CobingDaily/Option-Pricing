#ifndef OPTION_PRICING_BLACK_SCHOLES_H
#define OPTION_PRICING_BLACK_SCHOLES_H

#include "option.h"

class BlackScholes {
private:
    static double normalCDF(double x);
public:
    static double price(
        const Option& option,
        double spot,
        double rate,
        double volatility
    ) ;
};

#endif //OPTION_PRICING_BLACK_SCHOLES_H