#pragma once
#include <random>
#include <cmath>
#include <algorithm> // Nécessaire pour std::max
#include "UniformDistribution.hpp"

class PoissonDistribution {
private:
    double _lambda; // Taux moyen d'occurrence (lambda > 0)
    UniformDistribution _uniform; // Générateur de nombres aléatoires uniformes

    // Constante Pi pour Box-Muller
    static constexpr double PI = 3.14159265358979323846;

public:
    explicit PoissonDistribution(double lambda) : _lambda(lambda), _uniform(0.0, 1.0) {}

    int operator()(std::mt19937& gen) const
    {
        // SEUIL : En dessous de 30, on garde la méthode exacte de Knuth
        if (_lambda < 30.0) {
            double L = std::exp(-_lambda);
            double p = 1.0;
            int k = 0;

            do {
                k++;
                p *= _uniform(gen);
            } while (p > L);

            return k - 1;
        } 
        // AU-DESSUS DE 30 : On bascule sur l'approximation Normale
        else {
            // 1. On génère deux variables uniformes
            // On utilise (1.0 - U) pour éviter log(0)
            double u1 = 1.0 - _uniform(gen); 
            double u2 = _uniform(gen);

            // 2. Transformée de Box-Muller pour obtenir Z ~ N(0, 1)
            // Z est une variable normale "centrée réduite" (moyenne 0, écart-type 1)
            double z = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);

            // 3. Transformation affine pour correspondre à notre Poisson
            // On décale la moyenne (+ lambda) et on ajuste l'écart-type (* sqrt(lambda))
            double x = _lambda + z * std::sqrt(_lambda);

            // 4. On arrondit à l'entier le plus proche
            int resultat = static_cast<int>(std::round(x));

            // 5. Sécurité : une loi de Poisson ne peut pas être négative.
            // La loi normale allant de -infini à +infini, on coupe à zéro au cas où.
            return std::max(0, resultat);
        }
    }
};