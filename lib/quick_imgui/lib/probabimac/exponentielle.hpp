#pragma once
#include <random>
#include <cmath>

class LoiExponentielle {
private:
    double _lambda; // Le taux. Ex: 0.1 signifie qu'il y a environ 10% de chance que ça arrive à chaque tour.

public:
    // Constructeur
    explicit LoiExponentielle(double taux_lambda) : _lambda(taux_lambda) {}

    // La méthode de tirage (toujours en passant le moteur par référence)
    int generateExponentialTime(std::mt19937& gen) const {
        std::uniform_real_distribution<double> dist(0.00001, 1.0); // On évite le ln(0)
        double U = dist(gen);
        
        // La formule magique de la transformée inverse
        double temps = -std::log(U) / _lambda;
        
        return static_cast<int>(std::round(temps));
    }
};