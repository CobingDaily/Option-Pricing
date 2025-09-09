#include "option.h"
#include <iostream>

int main() {
    const Option call{105, Option::Type::CALL, 1};
    const Option put{105, Option::Type::PUT, 1};

    constexpr double spot{110};

    std::cout << "Spot price: $" << spot << "\n";
    std::cout << "Call payoff: $" << call.payoff(spot) << "\n";
    std::cout << "Put payoff: $" << put.payoff(spot) << "\n";

    return 0;
}
