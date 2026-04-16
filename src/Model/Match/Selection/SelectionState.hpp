#pragma once
#include <optional>
#include <vector>
#include "utilities/Vector2D.hpp"

/**
 * UI-facing selection state.
 *
 */
struct SelectionState {
    std::optional<Vector2D> selected;          // The currently selected piece
    std::optional<Vector2D> hoveredSelectable; // Hovered tile that can be selected
    std::vector<Vector2D>   highlighted;       // List of legal destination squares

    /**
     * Clears all selection state.
     */
    void clear()
    {
        selected.reset();
        hoveredSelectable.reset();
        highlighted.clear();
    }
};
