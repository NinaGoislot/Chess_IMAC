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
        _currentScene->render();
    }
}

void SceneManager::launchMenuScene()
{
    _currentScene = std::make_unique<MenuScene>(*this);
}

void SceneManager::launchGameScene(GameManager::Mode mode)
{
    _currentScene = std::make_unique<GameScene>(*this, mode);
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
