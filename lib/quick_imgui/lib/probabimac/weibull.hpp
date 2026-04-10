#pragma once
#include <cmath> // Pour std::log (logarithme népérien) et std::pow (puissance)
#include <random>
#include "uniforme.hpp"


class LoiWeibull {
private:
    double _k;      // Paramètre de forme (shape) = 1 : distribution exponentielle, < 1 : décroissance rapide, > 1 : décroissance lente
    double _lambda; // Paramètre d'échelle (scale) = durée de vie moyenne
    LoiUniforme _uniform; // Générateur de nombres aléatoires uniformes

public:
    LoiWeibull(double shape_k, double scale_lambda) : _k(shape_k), _lambda(scale_lambda), _uniform(0.0, 1.0) {}
    // Fonction de génération d'une durée de vie selon une loi de Weibull, à partir de sa forme (shape_k) et de son échelle (scale_lambda)

    // fonction de génération d'une durée de vie selon une loi de Weibull, à partir de sa forme (shape_k) et de son échelle (scale_lambda)
    int generateWeibullLifespan(std::mt19937& gen) const
    {
        // 1. On tire un nombre aléatoire uniforme U entre 0 et 1
        double U = _uniform.generateUniformRandom(gen);

        // 2. On applique la formule mathématique de la transformée inverse :
        // x = lambda * (-ln(U)) ^ (1 / k)
        double lifespan = _lambda * std::pow(-std::log(U), 1.0 / _k);

        // 3. On arrondit le résultat pour avoir un nombre entier de "coups"
        return static_cast<int>(std::round(lifespan));
    }
};