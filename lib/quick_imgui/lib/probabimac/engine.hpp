#pragma once
#include <random>

class StochasticEngine {
private:
    std::mt19937 gen;

public:
    StochasticEngine()
    {
        std::random_device random;
        gen = std::mt19937(random());
    };

    std::mt19937& getGenerator() { return gen; }
};