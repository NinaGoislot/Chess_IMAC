#pragma once

#include <memory>
#include "Scenes/Scene.hpp"

class SceneManager {
public:
    static SceneManager& instance();
    SceneManager(const SceneManager&)            = delete;
    SceneManager(SceneManager&&)                 = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager& operator=(SceneManager&&)      = delete;
    ~SceneManager()                              = default;

    void init();
    void renderCurrentScene();

    void launchMenuScene();
    void launchGameScene();

    // Flag pour feermeture de l'app
    void requestQuit();
    bool shouldQuit() const;

private:
    SceneManager() = default;
    std::unique_ptr<Scene> _currentScene;
    bool                   quitRequested = false;
};
