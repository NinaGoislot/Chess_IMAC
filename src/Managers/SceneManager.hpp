#pragma once

#include <memory>
#include "Managers/Game.hpp"
#include "Scenes/Scene.hpp"
#include "utilities/AppConfig.hpp"


class SceneManager {
public:
    static SceneManager& instance();
    SceneManager(const SceneManager&)            = delete;
    SceneManager(SceneManager&&)                 = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager& operator=(SceneManager&&)      = delete;
    ~SceneManager()                              = default;

    void init(const AppConfig& config);
    void renderCurrentScene();

    void launchMenuScene();
    void launchGameScene(Game::Mode mode = Game::Mode::Classic);

    // Flag pour feermeture de l'app
    void requestQuit();
    bool shouldQuit() const;

private:
    SceneManager() = default;
    std::unique_ptr<Scene> _currentScene;
    bool                   quitRequested = false;
};
