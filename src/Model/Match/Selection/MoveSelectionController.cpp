#include "Model/Match/Selection/MoveSelectionController.hpp"
#include "Model/Match/MatchState.hpp"

/**
 * Handles a board tile click and interprets it, translates raw input into game logic in brief
 *
 * Special: Ignores clicks on Kirby
 *
 * @param clickedTile : the board position that was clicked
 * @param matchState : the current game state
 * @return true if a move was successfully executed
 */
bool MoveSelectionController::onTileClicked(Vector2D clickedTile, MatchState& matchState)
{
    // Block clicks on  Kirby
    const int x = static_cast<int>(clickedTile.getX());
    const int y = static_cast<int>(clickedTile.getY());
    if (matchState.getHasKirbyAt(x, y))
        return false;

    // --- CASE 1: No piece currently selected ---
    if (!_selection.selected.has_value())
    {
        // Try to select the clicked piece (must be allied and piece must exist)
        if (matchState.canSelect(clickedTile))
        {
            _selection.selected = clickedTile;
            updateHighlights(matchState); // Show legal moves for this piece
        }
        return false;
    }

    // --- CASE 2: User clicked the same piece again ---
    // (Right-click deselection happens in GameManager, not here)
    if (_selection.selected.value() == clickedTile)
    {
        return false;
    }

    // --- CASE 3: User clicked a different tile → attempt move ---
    const Vector2D from = _selection.selected.value();
    if (matchState.tryMove(from, clickedTile))
    {
        clearSelection();
        return true;
    }

    // --- CASE 4: Move failed → try to select the new piece instead ---
    // Allows fast piece switching without explicit deselection
    if (matchState.canSelect(clickedTile))
    {
        _selection.selected = clickedTile;
        updateHighlights(matchState);
        return false;
    }

    // --- CASE 5: Click was invalid (empty square, enemy piece) → clear selection ---
    clearSelection();
    return false;
}

/**
 * Deselects the current piece and clears all highlights
 *
 * @return void
 */
void MoveSelectionController::clearSelection()
{
    _selection.clear();
}

void MoveSelectionController::updateHover(std::optional<Vector2D> hoveredTile, const MatchState& matchState)
{
    // Clear previous hover state
    _selection.hoveredSelectable.reset();

    if (!hoveredTile.has_value())
        return;

    // Only mark as hoverable if it contains an allied piece
    if (matchState.canSelect(hoveredTile.value()))
        _selection.hoveredSelectable = hoveredTile.value();
}

/**
 * highlight legal destination
 *
 * @param matchState : game state
 * @return Aucun
 */
void MoveSelectionController::updateHighlights(const MatchState& matchState)
{
    // Clear previous legal move list
    _selection.highlighted.clear();

    // No piece selected → nothing to highlight
    if (!_selection.selected.has_value())
        return;

    const Vector2D origin = _selection.selected.value();

    // Safety check: if selected piece disappeared (just in case, beeacause chaos mode exists)
    if (!matchState.canSelect(origin))
    {
        _selection.selected.reset();
        return;
    }

    _selection.highlighted = matchState.getLegalMovesFrom(origin);
}