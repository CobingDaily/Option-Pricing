## Overview

### Option-Pricing + Greek Calculation with Black-Scholes and Monte-Carlo

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

#### Black-Scholes Engine
- **Analytical Greeks**: Greeks are calculated analytically using formulas and included by default
- **Performance**: ~26 ns per pricing ($4×10^7$ pricings/second) - Greeks included with zero overhead
- **Accuracy**: Exact mathematical derivatives

#### Monte Carlo Engine
- **Numerical Greeks**: Uses external `FiniteDifferenceGreeks` to calculate greeks numerically
- **Performance**: Greeks calculation requires 5+ additional pricing runs
- **Accuracy**: Approximation based on finite difference epsilon (default: 1%), converges with more paths

### Performance Comparison

| Method | Price Only | Price + Greeks | Overhead | Speed Factor |
|--------|------------|----------------|----------|--------------|
| Black-Scholes | 26 ns | 26 ns | ~0% | 1.0x |
| Monte Carlo (10K paths) | 280 μs | 2.25 ms | 700% | 8.0x |
| Monte Carlo (100K paths) | 2.82 ms | 22.55 ms | 700% | 8.0x |

### Greeks Accuracy (Finite Difference vs Analytical)

| Paths | Delta Error | Gamma Error | Vega Error | Theta Error | Rho Error | Overall Accuracy |
|-------|-------------|-------------|------------|-------------|-----------|------------------|
| 10,000 | 0.198 | 0.202 | 0.231 | 0.159 | 0.075 | Poor |
| 50,000 | 0.032 | 0.023 | 0.100 | 0.008 | 0.026 | Good |
| 100,000 | 0.008 | 0.192 | 0.015 | 0.005 | 0.022 | Very Good* |
| 500,000 | 0.025 | 0.053 | 0.030 | 0.017 | 0.020 | Very Good |

*Note: Gamma shows higher error at 100K paths

**Observation:**
- **Delta and Vega** converge most reliably with increased paths
- **Gamma** converges nicely towards 50K paths, but diverges after around 100K
- **Theta and Rho** generally improve with more paths
- **Trade-off**: Higher accuracy requires significantly more computation time

## Third-Party Algorithms

This project uses the following third-party algorithms:

- **Black-Scholes Model:** Black & Scholes (1973), Merton (1973) - Nobel Prize Winners in Finance Theory
- **Acklam's Normal Quantile Function**: Used for calculating confidence intervals
    - Author: Peter John Acklam
    - Source: [An algorithm for computing the inverse normal cumulative distribution function](https://stackedboxes.org/2017/05/01/acklams-normal-quantile-function/)
    - Original: https://web.archive.org/web/20151030215612/http://home.online.no/~pjacklam/notes/invnorm/
    - Accuracy: Relative error < 1.15e-9
- **Monte Carlo Methods:** Geometric Brownian motion simulation using C++17 random number generators