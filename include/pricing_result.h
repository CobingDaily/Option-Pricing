#ifndef OPTION_PRICING_PRICING_RESULT_H
#define OPTION_PRICING_PRICING_RESULT_H

#include <optional>
#include <string>

#include "financial_math.h"
#include "greeks.h"

struct PricingResult {
    double price;
    std::optional<double> standard_error;
    std::optional<int> paths_used;
    Greeks greeks;
    std::string method_name;

    explicit PricingResult(const double p, const std::string &method = "Analytical")
        : price{p}, method_name{method} {
    }

    PricingResult(const double p, const Greeks &g, const std::string &method = "Analytical")
        : price{p}, greeks{g}, method_name{method} {
    }

    PricingResult(const double p, const double std_err, const int paths, const std::string &method = "Simulation")
        : price{p}, standard_error{std_err}, paths_used{paths}, method_name{method} {
    }

    PricingResult(const double p,
                  const double std_err,
                  const int paths,
                  const Greeks &g,
                  const std::string &method = "Simulation")
        : price{p}, standard_error{std_err}, paths_used{paths}, method_name{method} {
    }

    [[nodiscard]] std::pair<double, double> getConfidenceInterval(const double confidence_interval) const {
        if (!standard_error.has_value()) {
            return {price, price};
        }

        const double z_score = FinancialMath::getZScore(confidence_interval);
        const double margin = standard_error.value() * z_score;
        return {price - margin, price + margin};
    }

    [[nodiscard]] bool hasUncertainty() const {
        return standard_error.has_value();
    }
};

#endif //OPTION_PRICING_PRICING_RESULT_H
