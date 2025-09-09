#include <iostream>
#include <cmath>

int main() {
    // Call option payoff
    double spot_price { 100 };
    double strike_price { 105 };
    double time_to_expiry { 1 };
    double final_price { 110 };
    double payoff { 0 };

    if (final_price > strike_price) {
        payoff = final_price - strike_price;
    }

    std::cout << "Option Payoff: $" << payoff << "\n";

    return 0;

}
