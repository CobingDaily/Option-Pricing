#ifndef OPTION_PRICING_GREEKS_H
#define OPTION_PRICING_GREEKS_H

#include <optional>

struct Greeks {
    std::optional<double> delta;
    std::optional<double> gamma;
    std::optional<double> vega;
    std::optional<double> theta;
    std::optional<double> rho;

    [[nodiscard]] bool hasGreeks() const {
        return delta.has_value()
               || gamma.has_value()
               || vega.has_value()
               || theta.has_value()
               || rho.has_value();
    }
};

#endif //OPTION_PRICING_GREEKS_H