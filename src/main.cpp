#include <iostream>
#include <random>
#include <vector>

int main() {
    std::random_device rd;
    std::mt19937 generator(rd());

    std::normal_distribution<double> distribution( 0.0, 1.0);

    return 0;
}
