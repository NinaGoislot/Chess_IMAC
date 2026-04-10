#pragma once
#include <random>
#include <cmath>
#include "uniforme.hpp"

class LoiExponentielle {
private:
    double _lambda; // Le taux. Ex: 0.1 signifie qu'il y a environ 10% de chance que ça arrive à chaque tour.
    LoiUniforme _uniform; // Générateur de nombres aléatoires uniformes

public:
    // Constructeur
    explicit LoiExponentielle(double taux_lambda) : _lambda(taux_lambda), _uniform(0.0, 1.0) {}

    // La méthode de tirage (toujours en passant le moteur par référence)
    int generateExponentialTime(std::mt19937& gen) const {
        double U = _uniform.generateUniformRandom(gen);
        
        // La formule magique de la transformée inverse
        double temps = -std::log(U) / _lambda;
        
        return static_cast<int>(std::round(temps));
    }
};