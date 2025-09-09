#include "option.h"
#include <algorithm>

Option::Option(const double strike, const Type type, const double expiry)
    : strike_{strike}, type_{type}, expiry_{expiry} {
}

double Option::payoff(const double spot) const {
    return (type_ == Type::CALL)
               ? std::max(spot - strike_, 0.0)
               : std::max(strike_ - spot, 0.0);
}
