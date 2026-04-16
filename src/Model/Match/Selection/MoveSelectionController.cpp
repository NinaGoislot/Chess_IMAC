#include "Model/Match/Selection/MoveSelectionController.hpp"
#include "Model/Match/MatchState.hpp"

/**
 * Handles a board tile click and interprets it as either a piece selection or a move attempt.
 * Translates raw input into game logic: "player clicked here, what does that mean?"
 * 
 * Logic flow:
 * 1. If no piece selected → select the clicked piece (if it's an allied piece)
 * 2. If same piece selected again → do nothing (self-click is ignored, right-click deselects)
 * 3. If different tile selected → attempt the move (from → to)
 * 4. If move illegal → select the new piece instead (fast piece switching)
 * 
 * Special: Ignores clicks on Kirby (blocking chaos piece)
 * 
 * @param clickedTile : the board position that was clicked
 * @param matchState : the current game state (needed for legality checks)
 * @return true if a move was successfully executed, false otherwise
 */
bool MoveSelectionController::onTileClicked(Vector2D clickedTile, MatchState& matchState)
{
    // --- SECURITY: Block clicks on chaos special pieces (Kirby) ---
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
            updateHighlights(matchState);  // Show legal moves for this piece
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
        // Move succeeded! Clear selection and report success
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
 * Deselects the current piece and clears all highlights.
 * Clears: selected piece, highlighted legal moves, hovered selectable.
 * 
 * Called when:
 * - Move succeeds (user played a piece)
 * - Right-click deselection (user cancels selection)
 * - Invalid click on empty or enemy square
 * 
 * @return void
 */
void MoveSelectionController::clearSelection()
{
    _selection.clear();
}

/**
 * Updates the "hovered selectable piece" indicator.
 * Shows visual feedback when the mouse hovers over a piece the player can select.
 * 
 * Purpose: Renderer uses this to highlight squares containing allied pieces.
 * 
 * @param hoveredTile : the current mouse position (nullopt if no tile hovered)
 * @param matchState : game state (needed to check if piece is selectable)
 * @return void
 */
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
 * Computes and caches all legal moves from the currently selected piece.
 * Queries the game rules to determine which destinations are valid.
 * 
 * Purpose: Renderer uses this to highlight legal destination squares in green/blue.
 * 
 * Edge case: If selected piece no longer exists (was captured), clears selection.
 * 
 * @param matchState : game state (needed for legality checks and move computation)
 * @return void
 */
void MoveSelectionController::updateHighlights(const MatchState& matchState)
{
    // Clear previous legal move list
    _selection.highlighted.clear();

    // No piece selected → nothing to highlight
    if (!_selection.selected.has_value())
        return;

    const Vector2D origin = _selection.selected.value();

    // Safety check: if selected piece disappeared (shouldn't happen normally)
    if (!matchState.canSelect(origin))
    {
        _selection.selected.reset();
        return;
    }

    // Query the game rules for all legal destinations from this origin
    _selection.highlighted = matchState.getLegalMovesFrom(origin);
}