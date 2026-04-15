#pragma once
#include <../src/utilities/Vector2D.hpp>
#include <stack>
#include <utility>

// Small coordinate holder
struct Move {
    int _x;
    int _y;

    // Constructors
    Move(int x, int y) : _x(x), _y(y) {}
};

// TO DO : A supprimer, superflu, vector 2D peut faire le taff.