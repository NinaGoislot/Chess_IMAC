#include "Managers/GameManager.hpp"

GameManager::GameManager(const AppConfig& config)
    : _settings()
    , _textures()
    , _renderer(_textures)
    , _match(_textures)
    , _promotionFlow(_match, _textures)
    , _moveSystem()
{
    _textures.load(config);
    _renderer.initialize(config);

    newGame(Mode::Classic);
}

void GameManager::newGame(Mode mode)
{
    _mode = mode;
    _moveSystem.clearSelection();

    const MatchState::Mode matchMode = (_mode == Mode::Chaos) ? MatchState::Mode::Chaos : MatchState::Mode::Classic;
    _match.newMatch(matchMode);
}

void GameManager::displayBoard(float deltaTimeSeconds)
{
    const std::optional<BoardClick> clickedCase = _renderer.draw(
        _match.getBoard(),
        _settings,
        _match.getCurrentTurn(),
        deltaTimeSeconds,
        _match.getKirbyPosition(),
        _moveSystem.getSelectionState()
    );

    if (!clickedCase.has_value())
        return;

    handleBoardClick(Vector2D(static_cast<float>(clickedCase->x), static_cast<float>(clickedCase->y)));
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

void GameManager::handleBoardClick(Vector2D clickedTile)
{
    const int x = static_cast<int>(clickedTile.getX());
    const int y = static_cast<int>(clickedTile.getY());

    if (_match.getHasKirbyAt(x, y))
        return;

    _moveSystem.onTileClicked(clickedTile, _match);
}
