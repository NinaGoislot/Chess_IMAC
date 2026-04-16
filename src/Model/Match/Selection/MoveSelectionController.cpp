#include "Model/Match/Selection/MoveSelectionController.hpp"
#include "Model/Match/MatchState.hpp"

/**
 *
 * Traduit un clic gauche en selection ou tentative de coup.
 * Ignore les clics sur les cases contenant des pieces speciales (Kirby).
 * @param clickedTile : tuile cliquee.
 * @param matchState : etat courant du match.
 * @return Vrai si un coup a ete valide.
 */
bool MoveSelectionController::onTileClicked(Vector2D clickedTile, MatchState& matchState)
{
    // Ignore les clics sur les pieces speciales
    const int x = static_cast<int>(clickedTile.getX());
    const int y = static_cast<int>(clickedTile.getY());
    if (matchState.getHasKirbyAt(x, y))
        return false;

    if (!_selection.selected.has_value())
    {
        if (matchState.canSelect(clickedTile))
        {
            _selection.selected = clickedTile;
            updateHighlights(matchState);
        }
        return false;
    }

    if (_selection.selected.value() == clickedTile)
    {
        return false;
    }

    const Vector2D from = _selection.selected.value();
    if (matchState.tryMove(from, clickedTile))
    {
        clearSelection();
        return true;
    }

    if (matchState.canSelect(clickedTile))
    {
        _selection.selected = clickedTile;
        updateHighlights(matchState);
        return false;
    }

    clearSelection();
    return false;
}

void MoveSelectionController::clearSelection()
{
    _selection.clear();
}

void MoveSelectionController::updateHover(std::optional<Vector2D> hoveredTile, const MatchState& matchState)
{
    _selection.hoveredSelectable.reset();

    if (!hoveredTile.has_value())
        return;

    if (matchState.canSelect(hoveredTile.value()))
        _selection.hoveredSelectable = hoveredTile.value();
}

void MoveSelectionController::updateHighlights(const MatchState& matchState)
{
    _selection.highlighted.clear();

    if (!_selection.selected.has_value())
        return;

    const Vector2D origin = _selection.selected.value();

    if (!matchState.canSelect(origin))
    {
        _selection.selected.reset();
        return;
    }

    _selection.highlighted = matchState.getLegalMovesFrom(origin);
}