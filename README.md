## Design Decisions

### Greeks Calculation Architecture

This project implements a dual approach to Greeks calculation based on the underlying pricing method:

#### Black-Scholes Engine
- **Analytical Greeks**: Greeks are calculated analytically using formulas and included by default
- **Performance**: TODO: Benchmark
- **Accuracy**: Exact mathematical derivatives

#### Monte Carlo Engine
- **Numerical Greeks**: Uses external `FiniteDifferenceGreeks` to calculate greeks numerically
- **Performance**: Greeks calculation requires 5+ additional pricing runs
- **Accuracy**: Approximation based on finite difference epsilon (default: 1%)

#### Performance Comparison

| Method | Price Only | Price + Greeks | Overhead |
|--------|------------|----------------|------|
| Black-Scholes | ~0.0000ms | ~0.0000ms | 0%   |
| Monte Carlo (100K paths) | ~2.8ms | ~22.9ms | ~700%|

#### Usage Pattern
```cpp
// Black-Scholes: Greeks included automatically
const BlackScholesEngine bs_engine;
const auto bs_result = bs_engine.price(option, market);  // Includes analytical Greeks

// Monte Carlo: Greeks calculated separately if needed
const MonteCarloEngine mc_engine;
const auto mc_result = mc_engine.price(option, market);  // Price only

// Add Greeks when needed
const FiniteDifferenceGreeks greeks_calculator{mc_engine, 0.01};  // 1% epsilon
const auto [d, g, v, t, r] = greeks_calculator.calculate(option, market).greeks;
```
## Third-Party Algorithms

This project uses the following third-party algorithms:

- **Acklam's Normal Quantile Function**: Used for calculating confidence intervals
    - Author: Peter John Acklam
    - Source: [An algorithm for computing the inverse normal cumulative distribution function](https://stackedboxes.org/2017/05/01/acklams-normal-quantile-function/)
    - Original: https://web.archive.org/web/20151030215612/http://home.online.no/~pjacklam/notes/invnorm/
    - Accuracy: Relative error < 1.15e-9