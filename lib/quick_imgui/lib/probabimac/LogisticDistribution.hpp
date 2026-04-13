#pragma once
#include <cmath> // Nécessaire pour std::log et std::floor
#include <random>
#include "UniformDistribution.hpp"


class LogisticDistribution {
private:
    double _mu; // Paramètre de localisation
    double _s;  // Paramètre d'échelle
    UniformDistribution _uniform; // Générateur de nombres aléatoires uniformes
    
public:
    LogisticDistribution(double mu, double s) : _mu(mu), _s(s), _uniform(0.00001, 0.99999) {}

    double operator()(std::mt19937& gen)
    {
        double u = _uniform(gen); // Génère un nombre aléatoire uniforme entre 0 et 1
        return _mu + _s * std::log(u / (1.0 - u));
    }
};