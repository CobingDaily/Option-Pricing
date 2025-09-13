#include "financial_math.h"
#include "stdexcept"

/**
 * Acklam's approximation for inverse normal CDF
 * - Relative error: < 1.15e-9
 * - Source: https://stackedboxes.org/2017/05/01/acklams-normal-quantile-function/
 * - Original: https://web.archive.org/web/20151030215612/http://home.online.no/~pjacklam/notes/invnorm/
 */
double FinancialMath::normalQuantile(const double p) {
    if (p <= 0.0 || p >= 1.0) throw std::invalid_argument("Probability must be between 0 and 1");

    static constexpr double a0 = -3.969683028665376e+01;
    static constexpr double a1 =  2.209460984245205e+02;
    static constexpr double a2 = -2.759285104469687e+02;
    static constexpr double a3 =  1.383577518672690e+02;
    static constexpr double a4 = -3.066479806614716e+01;
    static constexpr double a5 =  2.506628277459239e+00;

    static constexpr double b1 = -5.447609879822406e+01;
    static constexpr double b2 =  1.615858368580409e+02;
    static constexpr double b3 = -1.556989798598866e+02;
    static constexpr double b4 =  6.680131188771972e+01;
    static constexpr double b5 = -1.328068155288572e+01;

    static constexpr double c0 = -7.784894002430293e-03;
    static constexpr double c1 = -3.223964580411365e-01;
    static constexpr double c2 = -2.400758277161838e+00;
    static constexpr double c3 = -2.549732539343734e+00;
    static constexpr double c4 =  4.374664141464968e+00;
    static constexpr double c5 =  2.938163982698783e+00;

    static constexpr double d1 =  7.784695709041462e-03;
    static constexpr double d2 =  3.224671290700398e-01;
    static constexpr double d3 =  2.445134137142996e+00;
    static constexpr double d4 =  3.754408661907416e+00;

    // Define break-points
    static constexpr double p_low = 0.02425;
    static constexpr double p_high = 1.0 - p_low;

    double x;

    if (p < p_low) {
        // Rational approximation for lower region
        double q = std::sqrt(-2.0 * std::log(p));
        x = (((((c0*q + c1)*q + c2)*q + c3)*q + c4)*q + c5) /
            ((((d1*q + d2)*q + d3)*q + d4)*q + 1.0);
    }
    else if (p <= p_high) {
        // Rational approximation for central region
        double q = p - 0.5;
        double r = q * q;
        x = (((((a0*r + a1)*r + a2)*r + a3)*r + a4)*r + a5)*q /
            (((((b1*r + b2)*r + b3)*r + b4)*r + b5)*r + 1.0);
    }
    else {
        // Rational approximation for upper region
        double q = std::sqrt(-2.0 * std::log(1.0 - p));
        x = -(((((c0*q + c1)*q + c2)*q + c3)*q + c4)*q + c5) /
             ((((d1*q + d2)*q + d3)*q + d4)*q + 1.0);
    }

    return x;
}

/**
 * For a confidence interval, we want the z-score such that
 *
 * P(-z < Z < z) = confidence_level
 *
 * This means P(Z < z) = (1 + confidence_level) / 2
 */
double FinancialMath::getZScore(const double confidence_level) {
    if (confidence_level <= 0.0 || confidence_level >= 1.0)
        throw std::invalid_argument("Confidence level must be between 0 and 1");

    const double probability = (1.0 + confidence_level) / 2.0;
    return normalQuantile(probability);
}
