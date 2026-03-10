#pragma once
#include <cmath> // Pour std::log (logarithme népérien) et std::pow (puissance)
#include <random>


class LoiWeibull {
private:
    double _k;      // Paramètre de forme (shape)
    double _lambda; // Paramètre d'échelle (scale)

public:
    LoiWeibull(double shape_k, double scale_lambda) : _k(shape_k), _lambda(scale_lambda) {};
    // Fonction de génération d'une durée de vie selon une loi de Weibull, à partir de sa forme (shape_k) et de son échelle (scale_lambda)

    // fonction de génération d'une durée de vie selon une loi de Weibull, à partir de sa forme (shape_k) et de son échelle (scale_lambda)
    int generateWeibullLifespan(std::mt19937& gen) const
    {
        // 1. On tire un nombre aléatoire uniforme U entre 0 et 1
        std::uniform_real_distribution<double> uniform_dist(0.00001, 1.0);
        double                                 U = uniform_dist(gen);

        // 2. On applique la formule mathématique de la transformée inverse :
        // x = lambda * (-ln(U)) ^ (1 / k)
        double lifespan = _lambda * std::pow(-std::log(U), 1.0 / _k);

        // 3. On arrondit le résultat pour avoir un nombre entier de "coups"
        return static_cast<int>(std::round(lifespan));
    }
};