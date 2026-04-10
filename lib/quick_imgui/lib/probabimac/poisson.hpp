#pragma once
#include <random>
#include <cmath> // Nécessaire pour std::exp
#include "uniforme.hpp"

class LoiPoisson {
private:
    double _lambda; // Taux moyen d'occurrence (lambda > 0)
    LoiUniforme _uniform; // Générateur de nombres aléatoires uniformes

public:
    explicit LoiPoisson(double lambda) : _lambda(lambda), _uniform(0.0, 1.0) {}

    // Fonction de génération d'un nombre d'événements selon une loi de Poisson
    int generatePoissonRandom(std::mt19937& gen) const
    {
        // Implémentation directe de l'algorithme de Knuth
        double L = std::exp(-_lambda);
        double p = 1.0;
        int k = 0;

        do {
            k++;
            p *= _uniform.generateUniformRandom(gen);
        } while (p > L);

        return k - 1;
    }
};