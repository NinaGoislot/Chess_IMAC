#pragma once
#include <random>

class UniformDistribution {
private:
    double _a; // Borne inférieure de l'intervalle
    double _b; // Borne supérieure de l'intervalle

public:
    UniformDistribution(double a, double b) : _a(a), _b(b) {}

    // Fonction de génération d'un nombre aléatoire selon une loi uniforme
    int operator()(std::mt19937& gen) const
    {
        // 1. On récupère les bornes du générateur de nombres entiers
        double min_gen = static_cast<double>(gen.min());
        double max_gen = static_cast<double>(gen.max());

        // 2. On génère une valeur brute et on la normalise entre [0, 1)
        // Le "+ 1.0" garantit que la valeur 1.0 stricte n'est jamais atteinte (comportement standard)
        double val_brute = static_cast<double>(gen());
        double u = (val_brute - min_gen) / (max_gen - min_gen + 1.0);

        // 3. On met à l'échelle sur l'intervalle [_a, _b)
        return static_cast<int>(_a + (_b - _a) * u);
    }
};
