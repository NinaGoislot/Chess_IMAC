#include "Managers/GameManager.hpp"
#include <imgui.h>
#include <optional>

/**
 * Note: this file orchestrates the main game loop and holds the match state.
 * Collects board interactions from both 2D and 3D views and merges them into a single update per frame.
 * (we do this because 2D and 3D views ar shown at the same time but render one after the other, so we can get duplicate interactions if we don't filter them)
 */

GameManager::GameManager(const AppConfig& config)
    : _settings()
    , _textures()
    , _renderer(_textures)
    , _match(_textures)
    , _promotionFlow(_match, _textures)
    , _moveSelectionController()
{
    _textures.load(config);
    _renderer.initialize(config);

    newGame(Mode::Classic);
}

void GameManager::newGame(Mode mode)
{
    _mode = mode;
    _moveSelectionController.clearSelection();

    const MatchState::Mode matchMode = (_mode == Mode::Chaos) ? MatchState::Mode::Chaos : MatchState::Mode::Classic;
    _match.newMatch(matchMode);
}

/**
 *
 * Initialise les interactions de la frame avant le rendu des vues
 */
void GameManager::beginBoardViewsFrame()
{
    _pendingClickedTile.reset();
    _pendingHoveredTile.reset();
}

/**
 *
 * Rend la vue 3D
 * @param deltaTimeSeconds : temps ecoulé pour l'animation de la vue 3D. sert a synchro les animations
 * @return Aucun
 */
void GameManager::draw3DBoardView(float deltaTimeSeconds)
{
    const std::optional<BoardClick> clickedCase = _renderer.draw3DView(
        _match.getBoard(),
        _settings,
        _match.getCurrentTurn(),
        deltaTimeSeconds,
        _match.getKirbyPosition(),
        _moveSelectionController.getSelectionState()
    );

    collectBoardInteraction(clickedCase, _renderer.getHoveredTile()); // la vue 3D peut aussi detecter les clics et survols, on les collecte pour le traitement en fin de frame
}

/**
 *
 * Rend la vue 2D
 * @return Aucun
 */
void GameManager::draw2DBoardView()
{
    const std::optional<BoardClick> clickedCase = _renderer.draw2DView(
        _match.getBoard(),
        _settings,
        _match.getKirbyPosition(),
        _moveSelectionController.getSelectionState()
    );

    collectBoardInteraction(clickedCase, _renderer.getHoveredTile());
}

/**
 *
 * appelle et exécute les interactions cumulées pendant la frame
 * @return Aucun
 */
void GameManager::endBoardViewsFrame()
{
    // PRIORITY : Clic droit = déselection
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && _pendingHoveredTile.has_value())
    {
        _moveSelectionController.clearSelection();
        return;
    }

    // clic gauche stocké
    if (_pendingClickedTile.has_value())
    {
        _moveSelectionController.onTileClicked(
            Vector2D(static_cast<float>(_pendingClickedTile->x), static_cast<float>(_pendingClickedTile->y)),
            _match
        );
    }

    // Update le survol (pour surligner les cases disponibles)
    if (_pendingHoveredTile.has_value())
    {
        _moveSelectionController.updateHover(Vector2D(static_cast<float>(_pendingHoveredTile->x), static_cast<float>(_pendingHoveredTile->y)), _match);
    }
    else
    {
        _moveSelectionController.updateHover(std::optional<Vector2D>{}, _match);
    }
}

/**
 *
 * Collecte les interactions detectées pendant la frame
 * @param clickedTile : tuile cliquee detectee
 * @param hoveredTile : tuile survolee detectee
 * @return Aucun
 */
void GameManager::collectBoardInteraction(const std::optional<BoardClick>& clickedTile, const std::optional<BoardClick>& hoveredTile)
{
    // Stocke le PREMIER clic détecté (ignore les suivants)
    if (!_pendingClickedTile.has_value() && clickedTile.has_value())
        _pendingClickedTile = clickedTile;

    // Met à jour le survol
    if (hoveredTile.has_value())
        _pendingHoveredTile = hoveredTile;
}

settings& GameManager::getSettings()
{
    return _settings;
}

const settings& GameManager::getSettings() const
{
    return _settings;
}

const std::vector<std::string>& GameManager::getMoveHistory() const
{
    return _match.getMoveHistory();
}

ChaosOptions& GameManager::getChaosOptionsMutable()
{
    return _match.getChaosOptionsMutable();
}

const ChaosOptions& GameManager::getChaosOptions() const
{
    return _match.getChaosOptions();
}

PromotionFlow& GameManager::getPromotionFlow()
{
    return _promotionFlow;
}

GameManager::Mode GameManager::getMode() const
{
    return _mode;
}

const std::string& GameManager::getWhitePlayerName() const
{
    return _match.getWhitePlayerName();
}

const std::string& GameManager::getBlackPlayerName() const
{
    return _match.getBlackPlayerName();
}

const std::string& GameManager::getActivePlayerName() const
{
    return _match.getActivePlayerName();
}

int GameManager::getFullTurnCount() const
{
    return _match.getFullTurnCount();
}

int GameManager::getCurrentTurnNumber() const
{
    return _match.getCurrentTurnNumber();
}

PieceColor GameManager::getCurrentTurnColor() const
{
    return _match.getCurrentTurn();
}

bool GameManager::getHasWinner() const
{
    return _match.getHasWinner();
}

const Player* GameManager::getWinner() const
{
    return _match.getWinner();
}

void GameManager::addPlayerWhite(const std::string& name)
{
    _match.addPlayerWhite(name);
}

void GameManager::addPlayerBlack(const std::string& name)
{
    _match.addPlayerBlack(name);
}

void GameManager::addMoveToHistory(const std::string& move)
{
    _match.addMoveToHistory(move);
}
