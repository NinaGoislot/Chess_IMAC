#pragma once
#include <../src/utilities/Vector2D.hpp>
#include <stack>
#include <utility>

struct Move {
    int _x;
    int _y;
    Move(int x, int y) : _x(x), _y(y){}
};