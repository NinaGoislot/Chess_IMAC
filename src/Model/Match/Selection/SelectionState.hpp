#pragma once
#include <optional>
#include <vector>
#include "utilities/Vector2D.hpp"

/**
 * UI-facing snapshot of the current piece selection state.
 *
 * Encapsulates what the user is selecting and what the renderer should display:
 * - selected: which piece (if any) is currently selected
 * - hoveredSelectable: which square the cursor is hovering over (if selectable)
 * - highlighted: which destination squares are legal moves (for visual feedback)
 *
 * This state flows from MoveSelectionController → Renderer to display
 * selection highlights, legal moves, and hover feedback.
 */
struct SelectionState {
    std::optional<Vector2D> selected;          // The currently selected piece
    std::optional<Vector2D> hoveredSelectable; // Hovered tile that can be selected
    std::vector<Vector2D>   highlighted;       // List of legal destination squares

    /**
     * Clears all selection state.
     * Resets: selected piece, hovered indicator, and highlighted moves.
     */
    void clear()
    {
        selected.reset();
        hoveredSelectable.reset();
        highlighted.clear();
    }
};
