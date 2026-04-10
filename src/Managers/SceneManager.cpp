#include "SceneManager.hpp"
#include <iostream>
#include "Scenes/GameScene.hpp"
#include "Scenes/MenuScene.hpp"

SceneManager& SceneManager::instance()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::init()
{
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

void SceneManager::launchGameScene()
{
    _currentScene = std::make_unique<GameScene>(*this);
}

void SceneManager::requestQuit()
{
    quitRequested = true;
}

bool SceneManager::shouldQuit() const
{
    return quitRequested;
}