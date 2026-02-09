#pragma once
#include <../src/utilities/Vector2D.hpp>
#include <stack>
#include <utility>

struct Move {
    Move(const Vector2D& direction, int maxSteps)
    {
        move.push(std::make_pair(direction, maxSteps));
    }
    std::stack<std::pair<Vector2D, int>> move;
};