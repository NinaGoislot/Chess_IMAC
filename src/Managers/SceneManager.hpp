#pragma once

#include <memory>
#include "Managers/GameManager.hpp"
#include "Scenes/Scene.hpp"
#include "utilities/AppConfig.hpp"


// Singleton that owns scene switching and application quit state.
class SceneManager {
public:
    // Getters
    static SceneManager& instance();
    // Constructors
    SceneManager(const SceneManager&)            = delete;
    SceneManager(SceneManager&&)                 = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager& operator=(SceneManager&&)      = delete;
    ~SceneManager()                              = default;

    // Init function: prepares startup scene and shared configuration.
    void init(const AppConfig& config);
    // Render function: draws currently active scene.
    void renderCurrentScene();

    // Launches the main menu scene.
    void launchMenuScene();
    // Launches a game scene in selected mode.
    void launchGameScene(GameManager::Mode mode = GameManager::Mode::Classic);

    // Accessors for the unique app controller instance.
    GameManager&       getGame();
    const GameManager& getGame() const;

    // Requests application shutdown at end of frame.
    void requestQuit();
    bool getShouldQuit() const;

private:
    SceneManager() = default;
    // Currently active scene object.
    std::unique_ptr<Scene> _currentScene;
    // Main app controller shared by scenes.
    std::unique_ptr<GameManager> _game;
    // Global app-quit flag shared with main loop.
    bool                   quitRequested = false;
};
