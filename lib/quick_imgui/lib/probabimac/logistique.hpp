#pragma once
#include <random>
#include <cmath> // Nécessaire pour std::log et std::floor
#include "uniforme.hpp"

class LoiGeometrique {
private:
    double      _p;       // Probabilité de succès (0 < p <= 1)
    LoiUniforme _uniform; // Générateur de nombres aléatoires uniformes
public:
    explicit LoiGeometrique(double p) : _p(p), _uniform(0.0, 1.0) {}

    // Fonction de génération d'un nombre de tentatives avant le premier succès selon une loi géométrique
    int generateGeometricRandom(std::mt19937& gen) const
    {
        // On tire un nombre aléatoire uniforme U entre 0 et 1
        double U = _uniform.generateUniformRandom(gen);

        // On utilise la formule inverse de la fonction de répartition de la loi géométrique :
        // X = floor(log(1 - U) / log(1 - p)) + 1
        // Le "+ 1" est nécessaire car la loi géométrique compte à partir de 1 (nombre de tentatives)
        return static_cast<int>(std::floor(std::log(1 - U) / std::log(1 - _p))) + 1;
    }
};