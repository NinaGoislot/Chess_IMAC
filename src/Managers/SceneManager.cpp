#include "SceneManager.hpp"
#include <stdexcept>
#include "Scenes/GameScene.hpp"
#include "Scenes/MenuScene.hpp"


SceneManager& SceneManager::instance()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::init(const AppConfig& config)
{
    _game = std::make_unique<GameManager>(config);
    launchMenuScene();
}

void SceneManager::renderCurrentScene()
{
    if (_currentScene)
    {
        _isRenderingScene = true;
        _currentScene->render();
        _isRenderingScene = false;
        applyPendingSceneAction();
    }
}

void SceneManager::launchMenuScene()
{
    if (_isRenderingScene)
    {
        _pendingSceneAction = PendingSceneAction::LaunchMenu;
        return;
    }

    _currentScene = std::make_unique<MenuScene>(*this);
}

void SceneManager::launchGameScene(GameManager::Mode mode, bool startNew)
{
    if (_isRenderingScene)
    {
        _pendingSceneAction = PendingSceneAction::LaunchGame;
        _pendingGameMode = mode;
        _pendingStartNewGame = startNew;
        return;
    }

    if (startNew)
        _game->newGame(mode);

    _hasInterruptedMatch = false;
    _currentScene = std::make_unique<GameScene>(*this, mode);
}

void SceneManager::saveInterruptedMatch()
{
    _hasInterruptedMatch = true;
}

void SceneManager::clearInterruptedMatch()
{
    _hasInterruptedMatch = false;
}

bool SceneManager::hasInterruptedMatch() const
{
    return _hasInterruptedMatch;
}

void SceneManager::resumeInterruptedMatch()
{
    if (!_hasInterruptedMatch)
        return;

    launchGameScene(getGame().getMode(), false);
}

void SceneManager::applyPendingSceneAction()
{
    if (_pendingSceneAction == PendingSceneAction::None)
        return;

    const PendingSceneAction action = _pendingSceneAction;
    _pendingSceneAction = PendingSceneAction::None;

    if (action == PendingSceneAction::LaunchMenu)
    {
        launchMenuScene();
        return;
    }

    launchGameScene(_pendingGameMode, _pendingStartNewGame);
}

GameManager& SceneManager::getGame()
{
    if (_game == nullptr)
        throw std::logic_error("SceneManager::getGame() called before SceneManager::init().");

    return *_game;
}

const GameManager& SceneManager::getGame() const
{
    if (_game == nullptr)
        throw std::logic_error("SceneManager::getGame() called before SceneManager::init().");

    return *_game;
}

void SceneManager::requestQuit()
{
    quitRequested = true;
}

bool SceneManager::getShouldQuit() const
{
    return quitRequested;
}
