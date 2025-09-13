#ifndef OPTION_PRICING_PRICING_RESULT_H
#define OPTION_PRICING_PRICING_RESULT_H

#include <optional>
#include <string>

struct PricingResult {
    double price;
    std::optional<double> standard_error;
    std::optional<int> paths_used;
    std::string method_name;

    explicit PricingResult(const double p, const std::string& method = "Analytical")
        : price{p}, method_name{method} { }

    PricingResult(const double p, const double std_err, const int paths, const std::string& method = "Simulation")
        : price{p}, standard_error{std_err}, paths_used{paths}, method_name{method} { }

    std::pair<double, double> getConfidenceInterval(const double confidence_interval = 0.95) const {
        if (!standard_error.has_value()) {
            return {price, price};
        }

        // either 95 or 99 CI
        const double z_score = (confidence_interval == 0.95) ? 1.96 : 2.576;
        const double margin = standard_error.value() * z_score;
        return {price - margin, price + margin};
    }

    bool hasUncertainty() const {
        return standard_error.has_value();
    }
};

#endif //OPTION_PRICING_PRICING_RESULT_H