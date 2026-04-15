#pragma once
#include <optional>
#include <vector>
#include "utilities/Vector2D.hpp"

// UI-facing selection snapshot
struct SelectionState {
    std::optional<Vector2D> selected;
    std::vector<Vector2D>   highlighted;

    void clear()
    {
        selected.reset();
        highlighted.clear();
    }
};