#pragma once
#include <optional>
#include <vector>
#include "utilities/Vector2D.hpp"

// UI-facing selection snapshot: selected tile and highlighted legal moves.
struct SelectionState {
    // Currently selected tile, if any.
    std::optional<Vector2D> selected;
    // Tiles highlighted as legal destinations.
    std::vector<Vector2D>   highlighted;

    // Clears both selection and highlighted tiles.
    void clear()
    {
        selected.reset();
        highlighted.clear();
    }
};