#pragma once
#include <../src/utilities/Vector2D.hpp>
#include <stack>
#include <utility>

struct Move {
    std::stack<std::pair<Vector2D, int>> move;
};