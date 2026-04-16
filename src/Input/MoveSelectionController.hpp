#pragma once

#include <optional>
#include "Model/Match/SelectionState.hpp"
#include "utilities/Vector2D.hpp"

class MatchState;

// Translates board click intent into selection/reselection/move actions.
class MoveSelectionController {
public:
    bool onTileClicked(Vector2D clickedTile, MatchState& matchState);
    void updateHover(std::optional<Vector2D> hoveredTile, const MatchState& matchState);

    const SelectionState& getSelectionState() const { return _selection; }
    void                  clearSelection();

private:
    void updateHighlights(const MatchState& matchState);

    SelectionState _selection;
};
