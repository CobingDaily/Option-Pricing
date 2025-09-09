#ifndef OPTION_PRICING_OPTION_H
#define OPTION_PRICING_OPTION_H

class Option {
public:
    enum class Type {
        CALL,
        PUT
    };

private:
    double strike_;
    Type type_;
    double expiry_;

public:
    Option(double strike, Type type, double expiry);

    [[nodiscard]] double payoff(double spot) const;
    [[nodiscard]] double getStrike() const { return strike_; }
    [[nodiscard]] Type getType() const { return type_; }
    [[nodiscard]] double getExpiry() const { return expiry_; }
};

#endif //OPTION_PRICING_OPTION_H
