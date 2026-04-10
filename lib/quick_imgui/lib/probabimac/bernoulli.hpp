#pragma once
#include <random>
#include "uniforme.hpp"

class LoiBernoulli {
private:
    double _p; // Probabilité de succès (0 <= p <= 1)
    LoiUniforme _uniform; // Générateur de nombres aléatoires uniformes

public:
    explicit LoiBernoulli(double p) : _p(p), _uniform(0.0, 1.0) {}

    // Fonction de génération d'un résultat selon une loi de Bernoulli
    bool generateBernoulliOutcome(std::mt19937& gen) const
    {
        // On tire un nombre aléatoire uniforme U entre 0 et 1
        double U = _uniform.generateUniformRandom(gen);

        // Si U < p, on considère que c'est un succès (true), sinon c'est un échec (false)
        return U < _p;
    }
};