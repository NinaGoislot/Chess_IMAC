#pragma once
#include <random>
#include <cmath>
#include "UniformDistribution.hpp"

class ExponentialDistribution {
private:
    double _lambda; // Le taux. Ex: 0.1 signifie qu'il y a environ 10% de chance que ça arrive à chaque tour.
    UniformDistribution _uniform; // Générateur de nombres aléatoires uniformes

public:
    // Constructeur
    explicit ExponentialDistribution(double taux_lambda) : _lambda(taux_lambda), _uniform(0.0, 1.0) {}

    // La méthode de tirage (toujours en passant le moteur par référence)
    int operator()(std::mt19937& gen) const {
        double U = _uniform(gen);
        
        // La formule magique de la transformée inverse
        double temps = -std::log(1.0 - U) / _lambda;
        
        return static_cast<int>(std::round(temps));
    }
};