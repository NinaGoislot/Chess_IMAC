#pragma once
#include <random>
#include <cmath>
#include "UniformDistribution.hpp"

class CauchyDistribution {
private:
    double _x0;    // Le centre (généralement 0 pour un saut autour de la position actuelle)
    double _gamma; // L'échelle (qui définit à quel point les sauts extrêmes sont fréquents)
    UniformDistribution _uniform; // Générateur de nombres aléatoires uniformes

    // On définit Pi pour la formule trigonométrique
    static constexpr double PI = 3.14159265358979323846;

public:
    CauchyDistribution(double location_x0, double scale_gamma) 
        : _x0(location_x0), _gamma(scale_gamma), _uniform(0.00001, 0.99999) {}

    int operator()(std::mt19937& gen) const {

        // On évite 0.0 et 1.0 par sécurité pour ne pas faire exploser la fonction tangente
        double U = _uniform(gen);
        
        // La formule mathématique de la transformée inverse de Cauchy
        double distance = _x0 + _gamma * std::tan(PI * (U - 0.5));
        
        return static_cast<int>(std::round(distance));
    }
};